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

//#define CVDEBUG(...)	printf(__VA_ARGS__)
#define CVDEBUG(...)

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
		CVDEBUG("Addr%d ", addr);
		//SendReset();
		//while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
		//{
		//	CVDEBUG("R%d ", ReadCurrent());
		//}

		//SendIdle();
		//while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
		//{
		//	CVDEBUG("I%d ", ReadCurrent());
		//}

		uint8_t msg[5] = { (uint8_t)addr, (uint8_t)(INS_CV_BIT | (((cv - 1) >> 8) & 3)), (uint8_t)((cv - 1) & 0xFF), (uint8_t)(DATA_CV_VERIFY_BIT | (set ? DATA_BIT: 0) | (bit&0x07)), 0 };
		SetErrorByte(msg, countof(msg));

		for (int i = 0; i < 5; i++)
		{
			SendDCCMessage(msg, countof(msg));

			// Wait for the message to be sent.
			while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
			{
				CVDEBUG("P%d ", ReadCurrent());
			}
			for (int i = 0; i < 20; i++)
			{
				CVDEBUG("A%d ", ReadCurrent());
				vTaskDelay(pdMS_TO_TICKS(1));
			}
			CVDEBUG("\n");
		}
		return true;
	}

	bool Verify_SM(uint8_t *msg, uint8_t len)
	{
		bool ret = false;
		
		uint32_t baseCurrent = ReadCurrent();
		uint16_t count = 1;
		for (int n = 0; n < 3; n++)
		{
			SendReset(EPreamble::Long);
			CVDEBUG("R%d ", ReadCurrent());
			while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
			{
				uint16_t current = ReadCurrent();
				CVDEBUG("r%d ", current);
				baseCurrent += current;
				count++;
			}
		}
		baseCurrent /= count;
		
		uint8_t ackCount = 0;
		for (int n = 0; n < 5; n++)
		{
			CVDEBUG("S%d ", ReadCurrent());
			SendDCCMessage(msg, countof(msg), EPreamble::Long);
			while (xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
			{
				uint16_t current = ReadCurrent();
				CVDEBUG("s%d ", current);
				if (current > baseCurrent + DCC_ACK_CURRENT)
					ackCount++;
			}
		}
		
		// send resets for 20ms (maximum time to wait for ack)
		TickType_t endTime = xTaskGetTickCount() + pdMS_TO_TICKS(20);
		while (xTaskGetTickCount() < endTime)
		{
			CVDEBUG("P%d ", ReadCurrent());
			SendReset(EPreamble::Long);
			//xSemaphoreTake(sentSemaphoreHandle, portMAX_DELAY);
			while(xSemaphoreTake(sentSemaphoreHandle, pdMS_TO_TICKS(1)) == pdFAIL)
			{
				uint16_t current = ReadCurrent();
				CVDEBUG("p%d ", current);
				if (current > baseCurrent + DCC_ACK_CURRENT)
					ackCount++;
			}
		}
			
			
		if (ackCount > DCC_ACK_PERIOD)
			ret = true;

		return ret;
	}
	
	bool VerifyBit_SM(uint8_t cv, uint8_t bit, bool set)
	{
		CVDEBUG("B:%d ", bit);
		
		uint8_t msg[4] = { (uint8_t)(INS_CV_BIT | (((cv - 1) >> 8) & 3)), (uint8_t)((cv - 1) & 0xFF), (uint8_t)(DATA_CV_VERIFY_BIT | (set ? DATA_BIT : 0) | bit), 0 };
		SetErrorByte(msg, 4);
		bool ret = Verify_SM(msg, 4);

		CVDEBUG("\n");
		return ret;
	}
	
	bool VerifyByte_SM(uint8_t cv, uint8_t value)
	{
		CVDEBUG("U8:%d ", value);
		
		uint8_t msg[4] = { (uint8_t)(INS_CV_VERIFY_BYTE | (uint8_t)(((cv - 1) >> 8) & 3)), (uint8_t)((cv - 1) & 0xFF), value, 0 };
		SetErrorByte(msg, 4);
		bool ret = Verify_SM(msg, 4);

		CVDEBUG("\n");
		return ret;
	}

	// Direct mode
	int16_t ReadCV_SM(uint8_t cv)
	{
		uint8_t value = 0;
		for (int i = 0; i < 8; i++)
		{
			value |= VerifyBit_SM(cv, i, true) << i;
		}
		
		// verify byte
		if(VerifyByte_SM(cv, value))
		{
			CVDEBUG("Verified VC(%d)=%d\n", cv, value);
			return value;
		}
		else
		{
			CVDEBUG("Failed Verification! VC(%d)=%d\n", cv, value);
			return -1;
		}
	}

	void Run(bool enable)
	{
		Enable(enable);
		
		bool lastTrackEnabled = isEnabled() && !isEStop();
		TickType_t lastTime = xTaskGetTickCount();
		int initialisationPacketCount = 0;
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
				initialisationPacketCount = 20;
			}
		
			if (enabled && initialisationPacketCount != 0)
			{
				SendIdle();
				xSemaphoreTake(sentSemaphoreHandle, portMAX_DELAY);	// Wait for the message to be sent.
				initialisationPacketCount--;
			}
			else if (enabled)
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
								int16_t address = ReadCV_SM(1);
								int16_t config = ReadCV_SM(29);
								int16_t manufacturer = ReadCV_SM(8);
								int16_t version = ReadCV_SM(7);
								uint16_t extendedAddress = 0;
								if (config > 0 && (config & CV29_TWO_BYTE_ADDRESS) != 0)
								{
									extendedAddress = ReadCV_SM(17) << 8;
									extendedAddress |= ReadCV_SM(18);
								}
								
								// Send Reply
								UIMsg msg;
								msg.type = EUIMessageType::ScanTrackReply;
								msg.scan.address = address;
								msg.scan.config = config;
								msg.scan.manufacturer = manufacturer;
								msg.scan.version = version;
								msg.scan.extendedAddress = extendedAddress;
								uimsg.Send(msg);
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
				vTaskDelay(pdMS_TO_TICKS(5));
			}
			// need to handle a 5ms gap before sending a message to the same decoder.
			// shortest message - 14 preamble + 3 bytes + 3 start bits + 1 end bit
			//                    14 * 58 + 3 * 8 * 58 + 3*100 + 1 * 58 = 2562us
	
			// todo - better timeout - every 1 second to update status.  only need to wait for msg (or enable/estop - should these be messages - high priority) when disabled.
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
