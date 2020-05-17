/*
 * Arduino.h
 *
 * Created: 16/05/2020 6:39:50 PM
 *  Author: Frank Tkalcevic
 */ 


#ifndef ARDUINO_H_
#define ARDUINO_H_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "EEPROM.h"


extern uint32_t millis();

class SerialClass
{
public:
	static void print(...){}
};

extern SerialClass Serial;

class Stream
{
};


#define F(string_literal) PSTR(string_literal)

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )



extern void arduino_init();

#endif /* ARDUINO_H_ */