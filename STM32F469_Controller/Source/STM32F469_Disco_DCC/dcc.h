#pragma once

#include "main.h"
#include "Common.h"

extern "C" void DCCTask_Entry(void *argument);

const int PREAMBLE_BITS = 14;
const int DCC_ONE_TIME = 58;	// usec
const int DCC_ZERO_TIME = 100;	// usec
const int MAX_DCC_MESSAGE_LEN = 6;
const int MAX_DCC_MESSAGES = 100;
const int BIT_BUFFER_SIZE = (PREAMBLE_BITS + MAX_DCC_MESSAGE_LEN*(8 + 1) + 1);	// max bits

struct DCCMessage
{
	uint16_t address;
	uint8_t msgLen;
	uint8_t msg[MAX_DCC_MESSAGE_LEN];
	uint32_t lastSendTime;
};

