
void playShort() {
	buzz_timer = millis();
	sig_type = 1;
	allow_play = true;
}

void playLong() {
	buzz_timer = millis();
	sig_type = 2;
	allow_play = true;
}

void playDoubleShort() {
	buzz_timer = millis();
	sig_type = 0;
	allow_play = true;
}

void buzzTick() {
	if (allow_play && enableBuzzer) {
		if (sig_type == 0) {
			if (buzz_steps == 0 || buzz_steps == 2) digitalWrite(BUZZ_PIN, HIGH);
			else digitalWrite(BUZZ_PIN, LOW);
			// переключение шагов
			if (millis() - buzz_timer >= SHORT_SIG_DUR) {
				buzz_steps++;
				if (buzz_steps > 2) {
					allow_play = 0;
					buzz_steps = 0;
					digitalWrite(BUZZ_PIN, LOW);
				}
				buzz_timer = millis();
			}
		} else if (sig_type == 1) {
			if (millis() - buzz_timer >= SHORT_SIG_DUR){
				digitalWrite(BUZZ_PIN, LOW);
				allow_play = 0;
				buzz_steps = 0;
			} 
			else digitalWrite(BUZZ_PIN, HIGH);
		} else {
			if (millis() - buzz_timer >= LONG_SIG_DUR){
				digitalWrite(BUZZ_PIN, LOW);
				allow_play = 0;
				buzz_steps = 0;
			} 
			else digitalWrite(BUZZ_PIN, HIGH);
		}
	} else 
		digitalWrite(BUZZ_PIN, LOW);
}