// slave
#include "generators.h"
#include "FlexiTimer2.h"
#include "configuration.h"
#include "Wire.h"
#include <iarduino_I2C_connect.h>
#include "uart_bytes.h"



uint8_t dataStorage[STORAGE_SIZE]  = {
	
	0, 	// номер слейва, для которого передаются данные 0
	0,	// флаг полученных данных						1
	0, 	// оба пина генератора							2
	0,	// первый байт частоты генерации				3
	0, // целая часть частоты генерации (51 гц)		4
	0,	// целая часть модуляции (4 гц)					5
	0, 	// первый байт времени генерации				6
	0, // второй байт времени генерации (300 сек)		7
	
	0, 	// оба пина шим2								8
	0,	// первый байт предела частоты шим2				9
	0,	// второй байт предела частоты шим2 (1 гц)		10
	0,	// скважность шим2 (50%)						11
	0,	// первый байт времени шим2						12
	0, // второй байт времени шим2 (300 сек)			13
	0,	// режим считывающих пинов						14
	0, // период опроса первого (5 сек)					15
	0, // период опроса второго (2 сек)					16
	
	0,	// счетчик 1									17
	0,	// счетчик 1									18
	0,	// счетчик 2									19
	0,	// счетчик 2									20
	0,	// бит включенности								21
	0,	// флаг переключателя режимов					22
	0,	// 3 канаооа работают							23
	0,	// дробная часть частоты генерации				24
	0,	// дробная часть частоты модуляции				25
	0,	// нужно ли вывести данные с первого входа		26
	0,	// нужно ли вывести данные со второго входа 	27
};	 
		
bool power;
volatile uint16_t counter1, counter2, analogInput1, analogInput2, analogInput3, analogInput4;
uint16_t upper_bound;
uint32_t generatorsTimer, inputTimer, generatorsTimeLimit, changefreq_timer;
uint32_t voltage;

impulseGenerator generator(FREQ_OUT1, FREQ_OUT2);
PWM_1 pwm1(PWM2_OUT1, PWM2_OUT2, PWM1_OUT1_MODE, PWM1_OUT2_MODE);
PWM_2 pwm2(PWM1_OUT1, PWM1_OUT2);


iarduino_I2C_connect I2C2;

void setup() {
	Serial.begin(9600);
	Wire.begin(MY_ADDRESS);
	I2C2.begin(dataStorage);
	pinMode(A1, INPUT);
	pinMode(A0, INPUT);
	pinMode(13, OUTPUT);
	pinMode(INTERRUPT_PIN, OUTPUT);
	Serial.print("My address: ");
	Serial.println(MY_ADDRESS);
}

void loop() {
	if (dataStorage[RX_FLAG] == 1 && dataStorage[SLAVE_NUMBER] == MY_ADDRESS) {
		if (power && dataStorage[POWER_STATE] == 0) {
			turnOff();
		} else if (!power) {
			refresh();
		} 		
		dataStorage[RX_FLAG] = 0;
	}
	generator.tick();
	pwm1.tick(); 
	pwm2.tick(); 
	checkTimers();

}

void checkTimers() {
	if (millis() - changefreq_timer >= 1000 && dataStorage[PWM2_MODE]) {
		setPWM2Random(upper_bound);
		changefreq_timer = millis();
	}

	if (millis() - generatorsTimer >= generatorsTimeLimit) {
		generator.disable();
		pwm1.disable();
		pwm2.disable();
		power = false;
		dataStorage[IS_WORKING_NOW] = false;
		digitalWrite(INTERRUPT_PIN, 0);
		digitalWrite(INTERRUPT_PIN, 1);
		digitalWrite(INTERRUPT_PIN, 0);
	}
	
	if (millis() - inputTimer >= 10) {
		analogInput1 = (long)analogRead(ANALOG_1) * 5000 / 1023;
		analogInput2 = (long)analogRead(ANALOG_2) * 5000 / 1023;
		analogInput3 = (long)analogRead(ANALOG_3) * 5000 / 1023;
		analogInput4 = (long)analogRead(ANALOG_4) * 5000 / 1023;

		dataStorage[COUNTER_1_HIGHBYTE] = highByte(counter1);
		dataStorage[COUNTER_1_LOWBYTE] = lowByte(counter1);	
		dataStorage[COUNTER_2_HIGHBYTE] = highByte(counter2);
		dataStorage[COUNTER_2_LOWBYTE] = lowByte(counter2);	
		
		dataStorage[ANALOG_1_HIGHBYTE] = highByte(analogInput1);
		dataStorage[ANALOG_1_LOWBYTE] = lowByte(analogInput1);	
		
		dataStorage[ANALOG_2_HIGHBYTE] = highByte(analogInput2);
		dataStorage[ANALOG_2_LOWBYTE] = lowByte(analogInput2);	
		
		dataStorage[ANALOG_3_HIGHBYTE] = highByte(analogInput3);
		dataStorage[ANALOG_3_LOWBYTE] = lowByte(analogInput3);	
		
		dataStorage[ANALOG_4_HIGHBYTE] = highByte(analogInput4);
		dataStorage[ANALOG_4_LOWBYTE] = lowByte(analogInput4);	
	
		inputTimer = millis();
	}	
}

void refresh() {
	float freq, mod;
	freq = (float)(dataStorage[INT_GEN_FREQ_HIGHBYTE] << 8 | dataStorage[INT_GEN_FREQ_LOWBYTE]) + (dataStorage[FRACT_GEN_FREQ] * 0.1);
	mod = (float)(dataStorage[INT_GEN_MODUL]) + (dataStorage[FRACT_GEN_MODUL] * 0.1);
	generatorsTimeLimit = (long)((dataStorage[GENERATORS_TIME_HIGHBYTE] << 8) | dataStorage[GENERATORS_TIME_LOWBYTE]) * 1000;
	
	generator.setFreq(freq);
	generator.setModulation(mod);

	if (dataStorage[PWM2_MODE]) { // 1 - рандом, 0 - не рандом
		upper_bound = ((dataStorage[PWM2_FREQ_HIGHBYTE] << 8) | dataStorage[PWM2_FREQ_LOWBYTE]) + 1;  
		setPWM2Random(upper_bound);
	} else {
		pwm2.setFreq((dataStorage[PWM2_FREQ_HIGHBYTE] << 8) | dataStorage[PWM2_FREQ_LOWBYTE]);
	}
	
	pwm1.setFreq(PWM1_FREQ);
	pwm1.setDuty(PWM1_DUTY);

	if (dataStorage[POWER_STATE] && !dataStorage[IS_WORKING_NOW]) turnOn();	
}

void inputCounter1() {
	counter1++;
}

void inputCounter2() {
	counter2++;
}

void turnOff() {
	generator.disable();
	pwm1.disable();
	pwm2.disable();
	dataStorage[IS_WORKING_NOW] = 0;
	power = 0;
}

void turnOn() {
	generatorsTimer = millis();
	inputTimer = millis();
	power = true;
	dataStorage[IS_WORKING_NOW] = 1;
	
	generator.setOuts(dataStorage[GEN_PINS]);
	generator.enable();
	
	pwm1.setOuts(3);
	pwm1.enable();

	pwm2.setOuts(dataStorage[PWM2_PINS]);
	pwm2.enable();
}

void setPWM2Freq(uint16_t freq) {
	pwm2.setFreq(freq);
	pwm2.setDuty(dataStorage[PWM2_DUTY]);
}

void setPWM2Random(uint16_t upper_bound) {
	uint8_t rand_number = random(1, upper_bound);
	setPWM2Freq(rand_number);
}