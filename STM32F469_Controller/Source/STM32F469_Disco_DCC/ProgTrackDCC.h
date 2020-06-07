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

#define PRGTRK_ARR_OFFSET 0
#define PRGTRK_CCn_OFFSET 2
#define PRGTRK_BURST_SIZE 3


class ProgTrackDCC: public DCCHal<DCCType::ProgrammingTrack, PRGTRK_ARR_OFFSET, PRGTRK_CCn_OFFSET, PRGTRK_BURST_SIZE>	
{
	SemaphoreHandle_t sentSemaphoreHandle;
	StaticSemaphore_t sentSemaphoreBuffer;

public:
	ProgTrackDCC( GPIO_TypeDef *CS_Port, uint16_t CS_Pin, 
				  GPIO_TypeDef *Disable_Port, uint16_t Disable_Pin, 
				  GPIO_TypeDef *Fault_Port, uint16_t Fault_Pin, 
				  GPIO_TypeDef *Signal_Port, uint16_t Signal_Pin ) 
		: DCCHal(CS_Port, CS_Pin, Disable_Port, Disable_Pin, Fault_Port, Fault_Pin, Signal_Port, Signal_Pin)
	{
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
				msg.type = ProgrammingTrackStatusUpdate;
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
				ShowProgrammingTrackPowerLED(enabled, 0);
			}
		
			// If new message
//			if ( uxQueueMessagesWaiting( queueHandle ) )
//			{
//				// Read message
//				DCCMessage msg;
//				if (xQueueReceive(queueHandle, &msg, 0) == pdPASS)
//				{
//					
//					// if trackEnabled, send message
//					if(enabled)
//					{
//					// Update local table
//					//      loco commands need to be stored for playback later
//					}
//				}
//			}
//			// Else process message table
//			else if ( enabled )
//			{
//				// We can't find a message to send - either empty table, or 5ms gap issue.
//				static uint8_t idleMessage[3] = { 0xFF, 0, 0xFF };
//				SendDCCMessage(idleMessage, sizeof(idleMessage));
//				xSemaphoreTake(sentSemaphoreHandle, portMAX_DELAY);	// Wait forever for the message to be sent. (or max 5ms?  Why wouldn't it go?)
//			}
		
			// need to handle a 5ms gap before sending a message to the same decoder.
			// shortest message - 14 preamble + 3 bytes + 3 start bits + 1 end bit
			//                    14 * 58 + 3 * 8 * 58 + 3*100 + 1 * 58 = 2562us

			// todo - better timeout - every 1 second to update status.  only need to wait for msg (or enable/estop - should these be messages - high priority) when disabled.
			vTaskDelay(pdMS_TO_TICKS(250));
		}
	}
	
	virtual void DCCSent() 
	{
		DCCHal::DCCSent();
		xSemaphoreGiveFromISR(sentSemaphoreHandle, NULL);
	}
};


extern void ProgrammingTrack_DCC_EStop(bool stop);
extern void ProgrammingTrack_DCC_Enable(bool enable); 
extern void ProgrammingTrack_DCC_ScanProgrammingTrack();
