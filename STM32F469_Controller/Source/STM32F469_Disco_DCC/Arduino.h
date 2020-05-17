#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"


#define millis()		((1000*xTaskGetTickCount())/configTICK_RATE_HZ)
#define delay(x)		vTaskDelay(pdMS_TO_TICKS((x)))	

#define pgm_read_word(x)	(*(int16_t*)x)
#define pgm_read_byte(x)	(*(int8_t*)x)
#define PROGMEM
#define ESTRING(x)			x

typedef int8_t Stream;

class EEPROMClass
{
public:
	uint8_t read(int index) { return 0;}
	void write(int index, uint8_t value) {}
	void update(int index, uint8_t value) {}
	template<class T>T& get(int index, T &value) { return value; }
	template<class T>const T& put(int index, const T& value) { return value;}
};

extern EEPROMClass EEPROM;

class SerialClass
{
public:
	void print(...) {}
};

extern SerialClass Serial;
#define HEX 0
#define F(x)