
void parsing() {
	if (Serial.available() > 0) {
		char incomingByte = Serial.read();
		if (startParsing) {
			if (incomingByte == 'E') {
				enableBuzzer = true;
			} else if (incomingByte == 'D') {
				enableBuzzer = false;
			} else if (incomingByte == 'S') {
				isPowerButtonSingle = true;
			} else if (incomingByte == 'T') {
				isPowerButtonSingle = false;
			} else {
				if (incomingByte != ' ' && incomingByte != ';') {
					new_number = (new_number * 10) + (incomingByte - '0');
				} else {
					data[index] = new_number;
					new_number = 0;
					index++;
				}
			}
		}
		if (incomingByte == '$') {
			startParsing = true;
			index = 0;
			new_number = 0;
		}
		if (incomingByte == ';') {
			startParsing = false;
			successful = true;
		}
		if (incomingByte == '!')
			power();
	}
	if (successful) {
		configure_slave();
		successful = 0;
	}
}


void sendUART() {
	if (millis() - send_timer >= POOL_PERIOD){
		Serial.print("$ "); 
		Serial.println(ID);
		uint8_t activeSlavesCount = getWorkingSlavesCount();
		for (uint8_t i = 0; i < SLAVE_AMOUNT; i++){
			uint16_t c1 = (I2C2.readByte(addresses[i], COUNTER_1_HIGHBYTE) << 8) | I2C2.readByte(addresses[i], COUNTER_1_LOWBYTE);
			uint16_t c2 = (I2C2.readByte(addresses[i], COUNTER_2_HIGHBYTE) << 8) | I2C2.readByte(addresses[i], COUNTER_2_LOWBYTE);
			uint16_t a1 = (I2C2.readByte(addresses[i], ANALOG_1_HIGHBYTE) << 8) | I2C2.readByte(addresses[i], ANALOG_1_LOWBYTE);
			uint16_t a2 = (I2C2.readByte(addresses[i], ANALOG_2_HIGHBYTE) << 8) | I2C2.readByte(addresses[i], ANALOG_2_LOWBYTE);
			uint16_t a3 = (I2C2.readByte(addresses[i], ANALOG_3_HIGHBYTE) << 8) | I2C2.readByte(addresses[i], ANALOG_3_LOWBYTE);
			uint16_t a4 = (I2C2.readByte(addresses[i], ANALOG_4_HIGHBYTE) << 8) | I2C2.readByte(addresses[i], ANALOG_4_LOWBYTE);
			printSlaveString(isSettingUp, activeSlavesCount, addresses[i], c1, c2, a1, a2, a3, a4);
		}
		Serial.println("#");
		send_timer = millis();
	}
}

String printSlaveString(bool isu, uint8_t activeSlavesCount, uint8_t slaveNumber, uint16_t c1, uint16_t c2, uint16_t a1, uint16_t a2, uint16_t a3, uint16_t a4) {
	if (!isu)	
		Serial.print("E");
	else if (isu && activeSlavesCount == 0)
		Serial.print("S");
	else 
		Serial.print("W");
	Serial.print(slaveNumber);
	Serial.print(" A");
	Serial.print(c1);
	Serial.print(" B");
	Serial.print(c2);
	Serial.print(" C");
	Serial.print(a1);
	Serial.print(" D");
	Serial.print(a2);
	Serial.print(" E");
	Serial.print(a3);
	Serial.print(" F");
	Serial.println(a4);
}
