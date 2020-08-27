uint8_t buzzStep;
uint32_t buzz_timer;
bool buzzLong, buzzShort, buzzDoubleShort; 

void buzzerTick(){
	if (buzzShort) buzzShortTick();
	if (buzzDoubleShort) buzzDoubleShortTick();
	if (buzzLong) buzzLongTick();
}

void playShort() {
	buzz_timer = millis();
	buzzShort = true;
}

void playLong() {	
	buzz_timer = millis();
	buzzLong = true;
}

void playDoubleShort() {
	buzz_timer = millis();
	buzzDoubleShort = true;
}

void buzzShortTick() {
	if (enableBuzzer && buzzShort) {
		if (millis() - buzz_timer >= SHORT_SIG_DUR){
			digitalWrite(BUZZ_PIN, LOW);
			buzzShort = false;
		} 
		else
			digitalWrite(BUZZ_PIN, HIGH);
	} else 
		digitalWrite(BUZZ_PIN, LOW);
}

void buzzDoubleShortTick() {
	if (enableBuzzer && buzzDoubleShort) {
		if (buzz_steps == 0 || buzz_steps == 2)
			digitalWrite(BUZZ_PIN, HIGH);
		else
			digitalWrite(BUZZ_PIN, LOW);
		if (millis() - buzz_timer >= SHORT_SIG_DUR) {
			buzz_steps++;
			if (buzz_steps > 2) {
				buzz_steps = 0;
				buzzDoubleShort = false;
				digitalWrite(BUZZ_PIN, LOW);
			}
			buzz_timer = millis();
		}
	}
}

void buzzLongTick() {
	if (enableBuzzer && buzzLong) {
		if (millis() - buzz_timer >= LONG_SIG_DUR){
			digitalWrite(BUZZ_PIN, LOW);
			buzzLong = false;
		} 
		else 
			digitalWrite(BUZZ_PIN, HIGH);
	} else 
		digitalWrite(BUZZ_PIN, LOW);
}