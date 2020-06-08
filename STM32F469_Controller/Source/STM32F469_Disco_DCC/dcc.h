#pragma once

#include "main.h"
#include "Common.h"

extern "C" void DCCTask_Entry(void *argument);

const int PREAMBLE_BITS = 14;
const int LONG_PREAMBLE_BITS = 20;
const int DCC_ONE_TIME = 58;	// usec
const int DCC_ZERO_TIME = 100;	// usec
const int MAX_DCC_MESSAGE_LEN = 6;
const int MAX_DCC_MESSAGES = 100;
const int BIT_BUFFER_SIZE = (LONG_PREAMBLE_BITS + MAX_DCC_MESSAGE_LEN*(8 + 1) + 1);	// max bits

const uint8_t ADDR_BROADCAST = 0;
const uint8_t ADDR_IDLE = 0xFF;

const uint8_t INS_CV_ACCESS = 0b01110000;
const uint8_t INS_CV_ACCESS_LONG = 0b11100000;
const uint8_t INS_CV_VERIFY_BYTE = INS_CV_ACCESS_LONG | 0b0100;
const uint8_t INS_CV_WRITE_BYTE = INS_CV_ACCESS_LONG | 0b1100;
const uint8_t INS_CV_BIT = INS_CV_ACCESS_LONG | 0b1000;

const uint8_t DATA_CV_WRITE_BIT = 0b11110000;
const uint8_t DATA_CV_VERIFY_BIT = 0b11100000;
const uint8_t DATA_BIT = 0b1000;

struct DCCMessage
{
	uint16_t address;
	uint8_t msgLen;
	uint8_t msg[MAX_DCC_MESSAGE_LEN];
	uint32_t lastSendTime;
};


enum EProgTrackMessage
{
	ScanTrack,
};

struct ProgTrackMessage
{
	EProgTrackMessage type;
};

