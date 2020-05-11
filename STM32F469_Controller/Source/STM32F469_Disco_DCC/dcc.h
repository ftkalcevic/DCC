#pragma once

#include "main.h"

extern "C" void DCCTask_Entry(void *argument);

enum DCCType 
{
	MainTrack = 0,
	ProgrammingTrack = 1
};

const int MAX_DCC_MESSAGE = 6;
const int MAX_DCC_MESSAGES = 100;
const int BIT_BUFFER_SIZE = (14 + MAX_DCC_MESSAGE*(8 + 1) + 1);	// max bit changes

struct DCCMessage
{
	uint16_t address;
	uint8_t msgLen;
	uint8_t msg[MAX_DCC_MESSAGE];
	uint32_t lastSendTime;
};

class DCC
{
	DCCMessage messageTable[MAX_DCC_MESSAGES];
	int messageCount;
	bool trackEnabled;
	uint32_t bitBuffer[BIT_BUFFER_SIZE*4];


	void SendDCCMessage(uint8_t *msg, uint8_t len);
	void AddBit(int &index, bool bit);

public:
	DCC( GPIO_TypeDef *CS_Port, uint16_t CS_Pin, 
		 GPIO_TypeDef *Enable_Port, uint16_t Enable_Pin, 
		 GPIO_TypeDef *Fault_Port, uint16_t Fault_Pin, 
		 ADC_HandleTypeDef *Sense_ADC, uint16_t ADC1_IN1, 
		 GPIO_TypeDef *Signal_Port, uint16_t Signal_Pin )
	{
	}
	
	DCC( GPIO_TypeDef *CS_Port, uint16_t CS_Pin, 
		 GPIO_TypeDef *Enable_Port, uint16_t Enable_Pin, 
		 GPIO_TypeDef *Fault_Port, uint16_t Fault_Pin, 
		 GPIO_TypeDef *Sense_Port, uint16_t Sense_Pin, 
		 GPIO_TypeDef *Signal_Port, uint16_t Signal_Pin )
	{
	}

	void Initialise();
	void Run();
};

extern DCC mainTrack;
extern DCC programmingTrack;