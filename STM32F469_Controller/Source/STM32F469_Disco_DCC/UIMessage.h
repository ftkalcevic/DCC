#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "semphr.h"
#include <assert.h>

#include "DCCSettings.hpp"
#include "InputEvent.h"
#include "KeyEvent.h"


namespace EUIMessageType
{
	enum EUIMessageType
	{
		MainTrackStatusUpdate,
		ProgrammingTrackStatusUpdate,
		InputEvent,
		KeyEvent,
	};
}


struct UIMsg
{
	EUIMessageType::EUIMessageType type;
	
	union
	{
		DCCSettings::HBStatus hbStatus;
		InputEvent input;
		KeyEvent keys;
	};
};

#define UIMESSAGE_QUEUE_LEN		10

class UIMessage
{
	uint8_t queueStorageBuffer[sizeof(UIMsg)*UIMESSAGE_QUEUE_LEN];
	StaticQueue_t queueBuffer;
	QueueHandle_t queueHandle;
public:
	UIMessage()
	{
		queueHandle = xQueueCreateStatic(UIMESSAGE_QUEUE_LEN, sizeof(UIMsg), queueStorageBuffer, &queueBuffer);
	}
	
	void Send(UIMsg &msg)
	{
		BaseType_t res = xQueueSend(queueHandle, &msg, 0);
		if (res != pdPASS)
		{
			assert(false && "UI Queue Send failed");
		}
	}
	
	bool Receive(UIMsg &msg)
	{
		BaseType_t res = xQueueReceive(queueHandle, &msg, 0);
		if (res != pdPASS)
		{
			assert(false && "UI Queue Receive failed");
			return false;
		}
		return true;
	}
	
	int QueueLength()
	{
		return uxQueueMessagesWaiting(queueHandle);
	}
};


extern UIMessage uimsg;