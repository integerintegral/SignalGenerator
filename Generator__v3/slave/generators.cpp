#include "generators.h"
#include <Arduino.h> 
#include "FastFuncs.h"
//#include "TimerOne_atmega8.h"
#include "TimerOne.h"

impulseGenerator::impulseGenerator(uint8_t pin1, uint8_t pin2) {
	_pin1 = pin1;
	_pin2 = pin2;
	pinMode(_pin1, OUTPUT);
	pinMode(_pin2, OUTPUT);
	Timer1.initialize(100);
}

void impulseGenerator::setFreq(float freq) {
	_period = (long)1000000 / freq;
	_duty = 1023 / _period;
	if (_period > 1023) _duty = 1;
}

void impulseGenerator::setModulation(float modul) {
	if (modul == 0.0) _m_period = 0;
	else _m_period = lround(500 / modul);
}

void impulseGenerator::tick() {
	if (_m_period == 0) _state = 1;
	else {
		if (millis() - _timer >= _m_period) {
			(_state) ? _state = 0 : _state = 1;
			_timer = millis();
		} 		
	}	
	if (_state) {
		pinMode(_pin1, OUTPUT);
		pinMode(_pin2, OUTPUT);
	} else {
		pinMode(_pin1, INPUT);
		pinMode(_pin2, INPUT);
	} 		
}

void impulseGenerator::setOuts(uint8_t st) {
	_allow_p1 = bitRead(st, 0);	
	_allow_p2 = bitRead(st, 1);

	(_allow_p1) ? pinMode(_pin1, OUTPUT) : pinMode(_pin1, INPUT);
	
	(_allow_p2) ? pinMode(_pin2, OUTPUT) : pinMode(_pin2, INPUT);
}

void impulseGenerator::enable() {
	Timer1.initialize();
	
	if (_allow_p1) Timer1.pwm(_pin1, _duty, _period);
	else Timer1.disablePwm(_pin1);		
	
	if (_allow_p2) Timer1.pwm(_pin2, _duty, _period);
	else Timer1.disablePwm(_pin2);
}

void impulseGenerator::disable() {
	Timer1.disablePwm(_pin1);
	Timer1.disablePwm(_pin2);
} 

PWM_1::PWM_1(uint8_t pin1, uint8_t pin2) {
	_pin1 = pin1;
	_pin2 = pin2;
	pinMode(_pin1, OUTPUT);
	pinMode(_pin2, OUTPUT);
	Serial.println(pin1);
	Serial.println(pin2);
}

void PWM_1::setFreq(uint16_t freq) {
	if (freq > 0) _period = (1000000 / freq);
}

void PWM_1::setDuty(uint8_t duty) {
	_on_p = _period * duty / 100;
	_off_p = _period - _on_p;
}

void PWM_1::tick() {
	
	if (!_is_working) {
		writeLow(_pin1);
		writeLow(_pin2);
		return;		
	}
	if ((micros() - _timer <= _on_p) && _state) {
		writeHigh(_pin1);
		writeHigh(_pin2);
		_state = 1;
	} else if ((micros() - _timer) > _on_p && _state) {
		_timer = micros();
		_state = 0;
	} else if ((micros() - _timer) <= _off_p && !_state) {
		if (_pin1_mode) writeHigh(_pin1);
		else writeLow(_pin1);
		if (_pin2_mode) writeHigh(_pin2);
		else writeLow(_pin2);
		_state = 0;
	} else if ((micros() - _timer) > _off_p && !_state) {
		_timer = micros();
		_state = 1;		
	}
	
}

void PWM_1::setOuts(bool pin1Mode, bool pin2Mode) {
	_pin1_mode = pin1Mode;
	_pin2_mode = pin2Mode;
}

void PWM_1::enable() {
	_is_working = 1;
	if (_pin1_mode)
		writeHigh(_pin1);
	if (_pin2_mode)
		writeLow(_pin2);
}

void PWM_1::disable() {
	_is_working = 0;
}

PWM_2::PWM_2(uint8_t pin1, uint8_t pin2) {
	_pin1 = pin1;
	_pin2 = pin2;
	pinMode(_pin1, OUTPUT);
	pinMode(_pin2, OUTPUT);
}

void PWM_2::setFreq(uint16_t freq) {
	if (freq > 0) _period = (1000000 / freq);
}

void PWM_2::setDuty(uint8_t duty) {
	_on_p = _period * duty / 255;
	_off_p = _period - _on_p;
}

void PWM_2::tick() {
	if (_is_working == 0) {
		writeLow(_pin1);
		writeLow(_pin2);
	} else { 
		if ((micros() - _timer <= _on_p) && _state) {
			if (_allow_p1 && _is_working) writeHigh(_pin1);
			else writeLow(_pin1);
			if (_allow_p2 && _is_working) writeHigh(_pin2);
			else writeLow(_pin2);
			_state = 1;
		} else if ((micros() - _timer) > _on_p && _state) {
			_timer = micros();
			_state = 0;		
		} else if ((micros() - _timer) <= _off_p && !_state) {
			writeLow(_pin1);
			writeLow(_pin2);
			_state = 0;
		} else if ((micros() - _timer) > _off_p && !_state) {
			_timer = micros();
			_state = 1;
		}
	}
}

void PWM_2::setOuts(uint8_t st) {
	_allow_p1 = bitRead(st, 0);
	_allow_p2 = bitRead(st, 1);
}

void PWM_2::enable() {
	_is_working = 1;
}

void PWM_2::disable() {
	_is_working = 0;
}
