// Файл конфигурации системы
#ifndef CONFIG
#define CONFIG

// СЛЕЙВЫ 1-4

#define MY_ADDRESS 2		// Адрес на шине I2C

// Выходы 
#define FREQ_OUT1 	9	// Генератор 1.1 (9 либо 10)
#define FREQ_OUT2 	10	// Генератор 1.2 (9 либо 10)

#define PWM1_OUT1  	7 	// ШИМ 1.1
#define PWM1_OUT2 	8 	// ШИМ 1.2

#define PWM2_OUT1  	6 	// ШИМ 2.1
#define PWM2_OUT2 	5 	// ШИМ 2.2

#define INTERRUPT_PIN 4 	// Пин обратной связи

// Входы
#define DIGITAL_1 	2	// цифровые счетчики, только D2 или D3
#define DIGITAL_2	3   // цифровые счетчики, только D2 или D3

#define ANALOG_1  	A0 	// Аналоговый вход
#define ANALOG_2 	A1	// Аналоговый вход
#define ANALOG_3  	A2 	// Аналоговый вход
#define ANALOG_4 	A3	// Аналоговый вход

#define PWM1_OUT1_MODE 0	// режим работы выхода №1 ШИМ 1.1 (0 - меандр, 1 - лог. 1)
#define PWM1_OUT2_MODE 0	// режим работы выхода №1 ШИМ 1.2 (0 - меандр, 1 - лог. 1)
#define PWM1_FREQ 	   1000	// частота ШИМ 1 (гц.)
#define PWM1_DUTY 	   50	// скважность ШИМ 1 (%)

#define FRONT_POL1	RISING	// RISING - переход сигнала с низкого ЛУ в высокий
							// FALLING - переход сигнала с высокого ЛУ в низкий
							// CHANGE  - при любом изменении сигнала

#define FRONT_POL2	RISING	// RISING - переход сигнала с низкого ЛУ в высокий
							// FALLING - переход сигнала с высокого ЛУ в низкий
							// CHANGE  - при любом изменении сигнала
#endif