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

#include "Wire.h"
#include <iarduino_I2C_connect.h>
#include "GyverButton.h"
#include "configuration.h"
#include "uart_bytes.h"
#include <avr/interrupt.h>

GButton pwr(POWER_BTN);

uint8_t data[STORAGE_SIZE], index;
uint16_t new_number;
uint8_t code, parsingStage, buzz_steps;
uint16_t fractpart, value;
float fvalue;
uint32_t parsingTimer, buzz_timer, send_timer;
bool startParsing, successful, allow_play, sig_type, _interrupt, first_start;
bool isSettingUp;
bool enableBuzzer = true, isPowerButtonSingle = true;

iarduino_I2C_connect I2C2;

void setup() {	
	Wire.begin();	
	pinMode(BUZZ_PIN, OUTPUT);
	attachInterrupt(0, interrupt, RISING);
	Serial.begin(BAUD_RATE);
}

void loop() {
	parsing();
	sendUART();
	buzzTick();
	pwr.tick();	

	if ((pwr.isSingle() && isPowerButtonSingle) || (pwr.isDouble() && !isPowerButtonSingle))
		power();

	if (_interrupt) {
		uint8_t workingSlaves = getWorkingSlavesCount();
		if (workingSlaves == 0)
			playLong();
		else
			playShort();
		_interrupt = 0;
	}
}

uint8_t getWorkingSlavesCount(){
	uint8_t c;
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++)
		c += I2C2.readByte(addresses[i], IS_WORKING_NOW);
	return c;
}


void configure_slave() {
	I2C2.writeByte(data[SLAVE_NUMBER], RX_FLAG, 1);
	I2C2.writeByte(data[SLAVE_NUMBER], SLAVE_NUMBER, data[SLAVE_NUMBER]);
	for (uint8_t j = 2; j <= FRACT_GEN_MODUL; j++)
		I2C2.writeByte(data[SLAVE_NUMBER], j, data[j]);	
}

void power() {
	uint8_t workingSlavesCount;
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++) workingSlavesCount += I2C2.readByte(addresses[i], IS_WORKING_NOW);
	(workingSlavesCount > 0) ? turnOff() : turnOn();
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++) I2C2.writeByte(addresses[i], RX_FLAG, 1);
	workingSlavesCount = 0;
}

void turnOn() {
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++) I2C2.writeByte(addresses[i], POWER_STATE, 1);
	playShort();
}

void turnOff() {
	for (uint8_t i = 0; i < SLAVE_AMOUNT; i++) I2C2.writeByte(addresses[i], POWER_STATE, 0);
	playLong();
}

void interrupt() {
	_interrupt = 1;
}
