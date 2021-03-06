#pragma once

#include "main.h"
#include "Common.h"

extern "C" void DCCTask_Entry(void *argument);

const int PREAMBLE_BITS = 14;
const int LONG_PREAMBLE_BITS = 22;
const int DCC_ONE_TIME = 58;	// usec
const int DCC_ZERO_TIME = 100;	// usec
const int MAX_DCC_MESSAGE_LEN = 6;
const int MAX_DCC_MESSAGES = 100;
const int BIT_BUFFER_SIZE = (LONG_PREAMBLE_BITS + MAX_DCC_MESSAGE_LEN*(8 + 1) + 5);	// max bits

const uint8_t ADDR_BROADCAST = 0;
const uint16_t ADDR_ACCESSORY_BROADCAST = 0x1FF;
const uint8_t ADDR_EXTENDED = 0b11000000;
const uint8_t ADDR_ACCESSORY = 0b10000000;
const uint8_t ADDR_ACCESSORY_MASK = 0b11000000;
const uint8_t ADDR2_ACCESSORY = 0b10000000;
const uint8_t ADDR2_ACCESSORY_EXTENDED = 0b00000001;
const uint8_t ADDR2_ACCESSORY_EXTENDED_MASK = 0b10001001;
const uint8_t ADDR_IDLE = 0xFF;
const uint16_t NO_ADDRESS = 0xFFFF;

const uint16_t ADDR_ACCESSORY_9BIT_MAX	= 0b1'11111111;

const uint8_t INS_SPEED_AND_DIRECTION = 0b01000000;
const uint8_t INS_ADVANCED_OPERATION  = 0b00100000;
const uint8_t INS_CV_ACCESS = 0b01110000;
//const uint8_t INS_CV_ACCESS_LONG = 0b11100000;
const uint8_t INS_CV_VERIFY_BYTE = INS_CV_ACCESS | 0b0100;
const uint8_t INS_CV_WRITE_BYTE = INS_CV_ACCESS | 0b1100;
const uint8_t INS_CV_BIT = INS_CV_ACCESS | 0b1000;

const uint8_t INS_ACC_CV_ACCESS = 0b11100000;
const uint8_t INS_ACC_CV_VERIFY_BYTE = 0b0100;
const uint8_t INS_ACC_CV_WRITE_BYTE = 0b1100;
const uint8_t INS_ACC_CV_BIT = 0b1000;

const uint8_t ADVOP_128_SPEED_STEP	   = 0b00011111;

const uint8_t ADVOP_128_SPEED_STEP_FWD		= 0b10000000;
const uint8_t ADVOP_128_SPEED_STEP_REV		= 0b00000000;
const uint8_t ADVOP_128_SPEED_STEP_ESTOP	= 0b00000001;
	
const uint8_t DATA_CV_WRITE_BIT = 0b11110000;
const uint8_t DATA_CV_VERIFY_BIT = 0b11100000;
const uint8_t DATA_BIT = 0b1000;

const uint8_t DATA_SD_STOP = 0;
const uint8_t DATA_SD_ESTOP = 1;
const uint8_t DATA_SD_FORWARD = (1 << 5);
const uint8_t DATA_SD_REVERSE = (0 << 5);
const uint8_t DATA_SD_FL	  = (1 << 4);


const uint16_t CV_PRIMARY_ADDRESS		= 1;
const uint16_t CV_EXTENDED_ADDRESS_MSB	= 17;
const uint16_t CV_EXTENDED_ADDRESS_LSB	= 18;

const uint8_t CV29_DIRECTION			= 0x01;
const uint8_t CV29_FL_LOCATION			= 0x02;
const uint8_t CV29_POWER_SOURCE_CONV	= 0x04;
const uint8_t CV29_BIDIRECT_COMMS		= 0x08;
const uint8_t CV29_FULL_SPEED_TABLE		= 0x10;
const uint8_t CV29_TWO_BYTE_ADDRESS		= 0x20;
const uint8_t CV29_ACCESSORY_DECODER	= 0x80;

const uint8_t CV29_ACC_BIDIRECT_COMMS   = 0x08;
const uint8_t CV29_ACC_EXTENDED_DECODER = 0x20;
const uint8_t CV29_ACC_EXTENDED_ADDR     = 0x40;

struct DCCMessage
{
	uint16_t address;
	uint8_t msgLen;
	uint8_t msg[MAX_DCC_MESSAGE_LEN];
	uint8_t repeatCount;
};


enum EProgTrackMessage
{
	ScanTrack,
	ScanAllCVs,
	StopScanAllCVs,
	WriteCV,
};

struct CV
{
	uint16_t cv;
	uint8_t value;
};

static const uint8_t MAX_SCANALL_RETRIES = 2;
struct ScanAll
{
	uint16_t cv;
	uint8_t retries;
	bool cancel;
};

struct ProgTrackMessage
{
	EProgTrackMessage type;
	union
	{
		CV cv;
		ScanAll scanAll;
	};
};

