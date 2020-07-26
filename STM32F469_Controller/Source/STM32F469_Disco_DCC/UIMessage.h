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
		ScanTrackReply,
		ScanAllCVsReply,
		CVWriteReply
	};
}

namespace EErrorCode
{
	enum EErrorCode
	{
		Success=0,
		Complete,
		NoACK,
		ValueMismatch,
		NoTrackPower,
		MAX_ERROR_CODE
	};
}


inline const char16_t *ErrorCodeText(EErrorCode::EErrorCode code)
{
	if (code < EErrorCode::MAX_ERROR_CODE)
	{
		static const char16_t *errors[] = { u"Success",
											u"Complete",
											u"No Ack from Decoder",
											u"Value Mismatch",
											u"No Track Power"
										   };
		static_assert(EErrorCode::MAX_ERROR_CODE == countof(errors), "Number of error strings doesn't match the number of error codes");
		return errors[code];
	}
	return u"Unknown Error";
}
	
	
struct ScanTrackReply
{
	EErrorCode::EErrorCode result;
	int16_t address;
	int16_t manufacturer;
	int16_t version;
	int16_t config;
	int16_t extendedAddress;
};

struct ScanAllCVsReply
{
	EErrorCode::EErrorCode result;
	int16_t CV;
	int8_t value;
};


struct UIMsg
{
	EUIMessageType::EUIMessageType type;
	
	union
	{
		DCCSettings::HBStatus hbStatus;
		InputEvent input;
		KeyEvent keys;
		ScanTrackReply scan;
		EErrorCode::EErrorCode result;
		ScanAllCVsReply scanAllCVs;
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
			//assert(false && "UI Queue Send failed");
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