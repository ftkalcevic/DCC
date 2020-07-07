#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "semphr.h"
#include <string.h>
#include "DCCSettings.hpp"
#include "UIMessage.h"
#include "AppMain.h"
#include "DCCHal.h"


#define MAINTRK_ARR_OFFSET 0
#define MAINTRK_CCn_OFFSET 3
#define MAINTRK_BURST_SIZE 4

#define MESSAGE_QUEUE_LEN	10

struct DecoderMessage
{
	uint16_t address;
	uint8_t msgLen;
	uint8_t msg[MAX_DCC_MESSAGE_LEN];
	uint8_t repeatCount;
	uint32_t lastSendTime;
};



class MainTrackDCC: public DCCHal<DCCType::MainTrack, MAINTRK_ARR_OFFSET, MAINTRK_CCn_OFFSET, MAINTRK_BURST_SIZE>
{
	DecoderMessage decoderTable[MAX_DCC_MESSAGES];
	int decoderCount;
	int nextRecord;
	SemaphoreHandle_t sentSemaphoreHandle;
	StaticSemaphore_t sentSemaphoreBuffer;
	uint8_t queueStorageBuffer[sizeof(DCCMessage)*MESSAGE_QUEUE_LEN];
	StaticQueue_t queueBuffer;
	QueueHandle_t queueHandle;

public:
	MainTrackDCC( GPIO_TypeDef *CS_Port, uint16_t CS_Pin, 
				  GPIO_TypeDef *Disable_Port, uint16_t Disable_Pin, 
				  GPIO_TypeDef *Fault_Port, uint16_t Fault_Pin, 
				  GPIO_TypeDef *Signal_Port, uint16_t Signal_Pin ) 
		: DCCHal(CS_Port, CS_Pin, Disable_Port, Disable_Pin, Fault_Port, Fault_Pin, Signal_Port, Signal_Pin)
	{
		for (int i = 0; i < countof(decoderTable); i++)
			decoderTable[i].address = NO_ADDRESS;
		decoderCount = 0;
		nextRecord = -1;
	}
	
	void Initialise(int tripCurrent, int toff, int slewRate)
	{
		sentSemaphoreHandle = xSemaphoreCreateBinaryStatic(&sentSemaphoreBuffer);
		queueHandle = xQueueCreateStatic(MESSAGE_QUEUE_LEN, sizeof(DCCMessage), queueStorageBuffer, &queueBuffer);
		DCCHal::Initialise(tripCurrent, toff, slewRate);
	}
	
	// find the next message in the table.  Don't send the same message consecutively - put an idle (or another message) between
	int findNextRecord(int currentRecord, TickType_t now )
	{
		const TickType_t MIN_TIME_BETWEEN_PACKETS = pdMS_TO_TICKS(5);	// 5ms
		int start = currentRecord >= 0 ? currentRecord : 0;
		for (int i = 0; i < countof(decoderTable); i++)
		{
			int index = (start + i) % countof(decoderTable);
			if (decoderTable[index].address != NO_ADDRESS && index != currentRecord && 
				(decoderTable[index].lastSendTime == 0 ||  now - decoderTable[index].lastSendTime > MIN_TIME_BETWEEN_PACKETS) )
				return index;
		}
		return -1;
	}
	
	int findRecord(DCCMessage & msg)
	{
		for (int i = 0; i < countof(decoderTable); i++)
		{
			if (decoderTable[i].address == msg.address)
				return i;
		}
		for (int i = 0; i < countof(decoderTable); i++)
		{
			if (decoderTable[i].address == NO_ADDRESS)
			{
				decoderTable[i].address  = msg.address;
				return i;
			}
		}
		return -1;
	}

	void ProcessNextRecord()
	{
		xSemaphoreTake(sentSemaphoreHandle, portMAX_DELAY);	// Wait forever for the message to be sent. (or max 5ms?  Why wouldn't it go?)
		
		TickType_t now = xTaskGetTickCount();
		if (nextRecord >= 0)
		{
			
			decoderTable[nextRecord].lastSendTime = now;
		
			if (decoderTable[nextRecord].repeatCount > 0)
			{
				// Repeat count?  Remove record when done.
				decoderTable[nextRecord].repeatCount--;
				if (decoderTable[nextRecord].repeatCount == 0)
				{
					decoderTable[nextRecord].address = NO_ADDRESS;
				}
			}
		}

		nextRecord = findNextRecord(nextRecord, now);
		if (nextRecord < 0)
		{
			static uint8_t idleMessage[3] = { 0xFF, 0, 0xFF };
			SendDCCMessage(idleMessage, sizeof(idleMessage));
		}
		else
		{
			SendDCCMessage(decoderTable[nextRecord].msg, decoderTable[nextRecord].msgLen);
		}
	}
	
	void Run(bool enable)
	{
		Enable(enable);
		bool lastTrackEnabled = isEnabled() && !isEStop();
		TickType_t lastTime = xTaskGetTickCount();
		
		xSemaphoreGive(sentSemaphoreHandle);	// Pre trigger semaphore.

		for (;;)
		{
			// Check device status every second
			if(xTaskGetTickCount() - lastTime > pdMS_TO_TICKS(1000))
			{
				// 1 sec timer
				UIMsg msg;
				msg.type = EUIMessageType::MainTrackStatusUpdate;
				UpdateHBridgeStatus(msg.hbStatus);
				uimsg.Send(msg);
				lastTime = xTaskGetTickCount();
			}
			
			// if trackenabled changed
			bool enabled  = isEnabled() && !isEStop();
			if(enabled != lastTrackEnabled)
			{
				// if disabled, clear msg queue, reset msg repeat
				lastTrackEnabled = enabled;
				ShowTrackPowerLED(enabled, 0);
			}
		
			// If new message
			if ( uxQueueMessagesWaiting( queueHandle ) )
			{
				// Read message
				DCCMessage msg;
				if (xQueueReceive(queueHandle, &msg, 0) == pdPASS)
				{
					msg.msgLen++;
					SetErrorByte(msg.msg, msg.msgLen);

					// Update local table
					int index = findRecord(msg);
					if (index >= 0)
					{
						nextRecord = index;
						decoderTable[index].address = msg.address;
						memcpy( decoderTable[index].msg, msg.msg, msg.msgLen);
						decoderTable[index].msgLen = msg.msgLen;
						decoderTable[index].lastSendTime = 0;
						decoderTable[index].repeatCount = msg.repeatCount;
					}
				}
			}
			
			// process message table
			if ( enabled )
			{
				ProcessNextRecord();
			}
			else
			{
				vTaskDelay(pdMS_TO_TICKS(5));
			}
		
			// need to handle a 5ms gap before sending a message to the same decoder.
			// shortest message - 14 preamble + 3 bytes + 3 start bits + 1 end bit
			//                    14 * 58 + 3 * 8 * 58 + 3*100 + 1 * 58 = 2562us

			// todo - better timeout - every 1 second to update status.  only need to wait for msg (or enable/estop - should these be messages - high priority) when disabled.
			//vTaskDelay(pdMS_TO_TICKS(5));
			//printf("M%d\n", ReadCurrent());

		}
	}
	
	virtual void DCCSent() 
	{
		DCCHal::DCCSent();
		xSemaphoreGiveFromISR(sentSemaphoreHandle, NULL);
	}
	
	void SendMessage(DCCMessage msg)
	{
		BaseType_t res = xQueueSend(queueHandle, &msg, 0);
		if (res != pdPASS)
		{
			assert(false && "UI Queue Send failed");
		}
	}
	
};


extern void MainTrack_DCC_EStop(bool stop);
extern void MainTrack_DCC_Stop(uint16_t address, bool estop=false);
extern void MainTrack_DCC_SetSpeedAndDirection(uint16_t address, EDirection::EDirection direction, uint16_t throttle, uint16_t brake);
