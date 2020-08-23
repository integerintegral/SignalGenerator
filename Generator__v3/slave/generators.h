// Описание классов генераторов 
#ifndef GENERATORS
#define GENERATORS
#include <Arduino.h>

class impulseGenerator
{
	public:
	impulseGenerator(uint8_t pin1, uint8_t pin2);
	void setFreq(float freq);
	void setModulation(float modulation);
	void tick();
	void setOuts(uint8_t st);
	void enable();
	void disable();
	
	private:
	uint32_t _timer, _period, _m_period;
	bool _state, _allow_p1, _allow_p2;
	uint16_t _duty, _freq;
	uint8_t _pin1, _pin2;	
}; 

class PWM_1
{
	public:
	PWM_1(uint8_t pin1, uint8_t pin2, bool pin1Mode, bool pin2Mode);
	void setFreq(uint16_t freq);
	void setDuty(uint8_t duty);
	void tick();
	void setOuts(uint8_t st);
	void enable();
	void disable();
	
	private:
	uint32_t _timer, _period, _on_p, _off_p;
	bool _state, _allow_p1, _allow_p2, _is_working;
	uint8_t _pin1, _pin2;
	bool _pin1_mode, _pin2_mode;
};

class PWM_2
{
	public:
	PWM_2(uint8_t pin1, uint8_t pin2);
	void setFreq(uint16_t freq);
	void setDuty(uint8_t duty);
	void tick();
	void setOuts(uint8_t st);
	void enable();
	void disable();
	
	private:
	uint32_t _timer, _period, _on_p, _off_p;
	bool _state, _allow_p1, _allow_p2, _is_working;
	uint8_t _pin1, _pin2;
};

#endif