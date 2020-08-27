// master

/*
распределение ячеек в массиве: 
	0, 	// номер слейва, для которого передаются данные 0
	0,	// флаг полученных данных						1
	3, 	// оба пина генератора							2
	0,	// первый байт частоты генерации				3
	51, // целая часть частоты генерации (51 гц)		4
	0,	// целая часть модуляции (4 гц)					5
	1, 	// первый байт времени генерации				6
	44, // второй байт времени генерации (300 сек)		7
	
	3, 	// оба пина шим2								8
	0,	// первый байт предела частоты шим2				9
	100,	// второй байт предела частоты шим2 (1 гц)	10
	50,	// скважность шим2 (50%)						11
	1,	// первый байт времени шим2						12
	44, // второй байт времени шим2 (300 сек)			13
	3,	// режим считывающих пинов						14
	50, // период опроса первого (5 сек)				15
	20, // период опроса второго (2 сек)				16
	
	0,	// счетчик 1									17
	0,	// счетчик 1									18
	0,	// счетчик 2									19
	0,	// счетчик 2									20
	1,	// бит включенности								21
	0,	// флаг переключателя режимов					22
	3,	// 3 канаооа работают							23
	5,	// дробная часть частоты генерации				24
	0	// дробная часть частоты модуляции				25
*/ 
#define ON true
#define OFF false

#include "Wire.h"
#include <iarduino_I2C_connect.h>
#include "GyverButton.h"
#include "configuration.h"
#include "uart_bytes.h"
#include <avr/interrupt.h>

GButton pwr(POWER_BTN);
GButton feedback(FEEDBACK_PIN);

uint8_t data[STORAGE_SIZE], index;
uint16_t new_number;
uint8_t code, parsingStage, buzz_steps;
uint16_t fractpart, value;
float fvalue;
uint32_t parsingTimer, send_timer;
bool startParsing, successful, sig_type;
bool enableBuzzer = true, isPowerButtonSingle = true;

iarduino_I2C_connect I2C2;

void setup() {	
	Wire.begin();	
	pinMode(BUZZ_PIN, OUTPUT);
	Serial.begin(BAUD_RATE);
	feedback.setDebounce(10);
	feedback.setType(LOW_PULL); 
}

void loop() {
	parsing();
	sendUART();
	pwr.tick();	
	feedback.tick();
	buzzerTick();

	if (isAnySlaveEnabled() && checkButtonWithButtonMode()) {
		turnOff();
	}
	else if (!isAnySlaveEnabled() && pwr.isSingle()) {	
		turnOn();
	}

	if (feedback.isClick()){
		uint8_t workingSlavesCount = getWorkingSlavesCount();
		if (workingSlavesCount == 0)
			playLong();			
		else
			playDoubleShort();
	}
}

bool checkButtonWithButtonMode(){
	return (pwr.isSingle() && isPowerButtonSingle) || (pwr.isDouble() && !isPowerButtonSingle);
}

uint8_t getWorkingSlavesCount() {
	uint8_t c;
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++)
		c += I2C2.readByte(addresses[i], IS_WORKING_NOW);
	return c;
}

void configure_slave() {
	for (uint8_t j = 2; j <= FRACT_GEN_MODUL; j++)
		I2C2.writeByte(data[SLAVE_NUMBER], j, data[j]);	
	I2C2.writeByte(data[SLAVE_NUMBER], RX_FLAG, 1);
	I2C2.writeByte(data[SLAVE_NUMBER], SLAVE_NUMBER, data[SLAVE_NUMBER]);
	I2C2.writeByte(data[SLAVE_NUMBER], IS_SETTING_UP, 1);
}

bool isAnySlaveEnabled(){
	uint8_t workingSlavesCount = getWorkingSlavesCount();
	return workingSlavesCount > 0;
}

void turnOn() {
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++) I2C2.writeByte(addresses[i], POWER_STATE, ON);
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++) I2C2.writeByte(addresses[i], RX_FLAG, 1);
	playShort();
}

void turnOff() {
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++) I2C2.writeByte(addresses[i], POWER_STATE, OFF);
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++) I2C2.writeByte(addresses[i], RX_FLAG, 1);
	playLong();
}