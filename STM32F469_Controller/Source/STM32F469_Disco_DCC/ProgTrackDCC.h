#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "semphr.h"
#include <string.h>
#include <stdio.h>
#include "DCCSettings.hpp"
#include "UIMessage.h"
#include "AppMain.h"
#include "DCCHal.h"

#define PRGTRK_ARR_OFFSET 0
#define PRGTRK_CCn_OFFSET 2
#define PRGTRK_BURST_SIZE 3


#define PRGTRK_MESSAGE_QUEUE_LEN	5

class ProgTrackDCC: public DCCHal<DCCType::ProgrammingTrack, PRGTRK_ARR_OFFSET, PRGTRK_CCn_OFFSET, PRGTRK_BURST_SIZE>	
{
	SemaphoreHandle_t sentSemaphoreHandle;
	StaticSemaphore_t sentSemaphoreBuffer;
	uint8_t queueStorageBuffer[sizeof(ProgTrackMessage) * PRGTRK_MESSAGE_QUEUE_LEN];
	StaticQueue_t queueBuffer;
	QueueHandle_t queueHandle;

public:
	ProgTrackDCC( GPIO_TypeDef *CS_Port, uint16_t CS_Pin, 
				  GPIO_TypeDef *Disable_Port, uint16_t Disable_Pin, 
				  GPIO_TypeDef *Fault_Port, uint16_t Fault_Pin, 
				  GPIO_TypeDef *Signal_Port, uint16_t Signal_Pin ) 
		: DCCHal(CS_Port, CS_Pin, Disable_Port, Disable_Pin, Fault_Port, Fault_Pin, Signal_Port, Signal_Pin)
	{
	}

	void Initialise(int tripCurrent, int toff, int slewRate)
	{
		sentSemaphoreHandle = xSemaphoreCreateBinaryStatic(&sentSemaphoreBuffer);
		queueHandle = xQueueCreateStatic(PRGTRK_MESSAGE_QUEUE_LEN, sizeof(ProgTrackMessage), queueStorageBuffer, &queueBuffer);
		DCCHal::Initialise(tripCurrent, toff, slewRate);
	}

	bool VerifyBit(uint16_t addr, uint8_t cv, uint8_t bit, bool set)
	{
		printf("Addr%d ", addr);
		//SendReset();
		//while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
		//{
		//	printf("R%d ", ReadCurrent());
		//}

		//SendIdle();
		//while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
		//{
		//	printf("I%d ", ReadCurrent());
		//}

		uint8_t msg[5] = { (uint8_t)addr, INS_CV_BIT | (((cv - 1) >> 8) & 3), ((cv - 1) & 0xFF), DATA_CV_VERIFY_BIT | (set ? DATA_BIT: 0) | (bit&0x07), 0 };
		SetErrorByte(msg, countof(msg));

		for (int i = 0; i < 5; i++)
		{
			SendDCCMessage(msg, countof(msg));

			// Wait for the message to be sent.
			while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
			{
				printf("P%d ", ReadCurrent());
			}
			for (int i = 0; i < 20; i++)
			{
				printf("A%d ", ReadCurrent());
				vTaskDelay(pdMS_TO_TICKS(1));
			}
			printf("\n");
		}
		return true;
	}

	uint8_t ReadCV(uint8_t cv)
	{
		//for (int addr = 990; addr < 9999; addr++)
		////uint8_t addr = 5;
		//	{
		//	printf("ADDr=%d\n", addr);
		//	for (int n = 0; n < 25; n++)
		//	{
		//		SendIdle();
		//		while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL);
		//	}
		//	uint8_t drv[6];
		//	uint8_t* ptr = drv;
		//	if (addr <= 127)
		//	{
		//		*(ptr++) = addr;
		//	}
		//	else
		//	{
		//		*(ptr++) = ((addr >> 8) & 0xFF) | 0xC0;
		//		*(ptr++) = addr & 0xFF;
		//	}
		//	*(ptr++) = 0b01110111;
		//	*(ptr++) = 0;
		//	SetErrorByte(drv, ptr - drv);
		//	
		//	//uint8_t drv[3] = { addr, 0b01110111, 0 };
		//	//SetErrorByte(drv, countof(drv));

		//	for (int n = 0; n < 500; n++)
		//	{
		//		SendDCCMessage(drv, ptr - drv);
		//		while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL);
		//	}	

		//	ptr = drv;
		//	if (addr <= 127)
		//	{
		//		*(ptr++) = addr;
		//	}
		//	else
		//	{
		//		*(ptr++) = ((addr >> 8) & 0xFF) | 0xC0;
		//		*(ptr++) = addr & 0xFF;
		//	}
		//	*(ptr++) = 0b01100001;
		//	*(ptr++) = 0;
		//	SetErrorByte(drv, ptr - drv);
		//	for (int n = 0; n < 100; n++)
		//	{
		//		SendDCCMessage(drv, ptr - drv);
		//		while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL);
		//	}

		//}
		//
		//	return 0;
		for (uint8_t addr = 0; addr < 2; addr++)
		{
			VerifyBit(addr, 8, 0, true);
			VerifyBit(addr, 8, 0, false);
		}
		return 0;
		uint8_t ret = 0;
		for (uint8_t addr = 0; addr < 10; addr++)
		for (uint8_t i = 0; i < 8; i++)
		{
			//for (int n = 0; n < 5; n++)
			//{
			//	uint8_t drv[3] = { addr, 0b01100011, 0 };
			//	SetErrorByte(drv, countof(drv));
			//	SendDCCMessage(drv, countof(drv));
			//	while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL);
			//	SendIdle();
			//	while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL);
			//}

			SendIdle();
			while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL);

			printf("%d ", i);

			// Send reset packet
			//for (int r = 0; r < 3; r++)
			//{
			//	SendReset();
			//	while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
			//	{
			//		printf("R%d ", ReadCurrent());
			//	}
			//}


			//for (int i = 0; i < 10; i++)
			//{
			//	vTaskDelay(pdMS_TO_TICKS(1));
			//	printf("P%d ", ReadCurrent());
			//}

			uint8_t msg[5] = { addr, INS_CV_BIT | (((cv-1)>>8)&3), ((cv - 1) & 0xFF), DATA_CV_VERIFY_BIT | DATA_BIT | i, 0 };
			SetErrorByte(msg, countof(msg));
			SendDCCMessage(msg, countof(msg));

			// Wait for the message to be sent.
			uint16_t idleCurrent;
			while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
			{
				idleCurrent = ReadCurrent();
				printf("I%d ",idleCurrent);
			}
			// Read the current 
			uint16_t ackCurrent;
			for (int i = 0; i < 20; i++)
			{
				vTaskDelay(pdMS_TO_TICKS(1));
				ackCurrent = ReadCurrent();
				printf("A%d ", ackCurrent);
			}
			printf("\n");

		}
		return ret;
	}

	void Run(bool enable)
	{
		Enable(enable);
		bool lastTrackEnabled = isEnabled() && !isEStop();
		TickType_t lastTime = xTaskGetTickCount();
		for (;;)
		{
			// Check device status ever second
			if(xTaskGetTickCount() - lastTime > pdMS_TO_TICKS(1000))
			{
				// 1 sec timer
				UIMsg msg;
				msg.type = EUIMessageType::ProgrammingTrackStatusUpdate;
				UpdateHBridgeStatus(msg.hbStatus);
				uimsg.Send(msg);
				lastTime = xTaskGetTickCount();
			}
			
			// if trackenabled changed
			bool enabled  = isEnabled() && !isEStop();
			if(enabled != lastTrackEnabled)
			{
				lastTrackEnabled = enabled;
				ShowProgrammingTrackPowerLED(enabled, 0);
			}
		
			if (enabled)
			{
				if (uxQueueMessagesWaiting(queueHandle))
				{
					ProgTrackMessage msg;
					if (xQueueReceive(queueHandle, &msg, 0) == pdPASS)
					{
						switch (msg.type)
						{
							case EProgTrackMessage::ScanTrack:
							{
								// Read CV1,  CV7,CV8, CV29,  (CV17,CV18 extended address)
								uint8_t address = ReadCV(1);
								if (address != 0)
								{
									uint8_t version = ReadCV(7);
									uint8_t manufacturer = ReadCV(8);
									uint8_t config = ReadCV(29);
									uint16_t extendedAddress = 0;
									if (config & (1 << 5))
									{
										extendedAddress = ReadCV(17) << 8;
										extendedAddress |= ReadCV(18);
									}
									// do something.
								}
								// for each bit, send CV bit manipulation cmd
								// if message, 
									// send
									// wait
									// wait for ack. (+60mA for 6ms+/-1ms)
								break;
							}
							default:
								break;
						}
					}
				}
				else
				{
					SendIdle();
					xSemaphoreTake(sentSemaphoreHandle, portMAX_DELAY);	// Wait for the message to be sent.
				}
			}
			else
			{
				if (uxQueueMessagesWaiting(queueHandle))
					xQueueReset(queueHandle);
				SendIdle();
				xSemaphoreTake(sentSemaphoreHandle, portMAX_DELAY);	// Wait for the message to be sent.
			}
			// need to handle a 5ms gap before sending a message to the same decoder.
			// shortest message - 14 preamble + 3 bytes + 3 start bits + 1 end bit
			//                    14 * 58 + 3 * 8 * 58 + 3*100 + 1 * 58 = 2562us
	
			// todo - better timeout - every 1 second to update status.  only need to wait for msg (or enable/estop - should these be messages - high priority) when disabled.
			vTaskDelay(pdMS_TO_TICKS(5));
		}
	}
	
	virtual void DCCSent() 
	{
		DCCHal::DCCSent();
		xSemaphoreGiveFromISR(sentSemaphoreHandle, NULL);
	}

	void SendMsg(ProgTrackMessage& msg)
	{
		xQueueSend(queueHandle, &msg, 0);
	}
};


extern void ProgrammingTrack_DCC_EStop(bool stop);
extern void ProgrammingTrack_DCC_Enable(bool enable); 
extern void ProgrammingTrack_DCC_ScanProgrammingTrack();
