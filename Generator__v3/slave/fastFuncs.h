#ifndef FASTFUNCS
#define FASTFUNCS

void writeHigh(uint8_t pin) {
	if (pin < 8) PORTD |= (1 << pin);
	else PORTB |= (1 << (pin - 8));
	//digitalWrite(pin, 1);
}

void writeLow(uint8_t pin) {
	//digitalWrite(pin, 0);
	
	if (pin < 8) PORTD &= (~(1 << pin));
	else PORTB &= ~(1 << (pin - 8)); 
}
#endif