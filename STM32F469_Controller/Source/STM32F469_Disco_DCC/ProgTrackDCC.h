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

	bool VerifyMsg(uint8_t *msg, uint8_t len)
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
			SendDCCMessage(msg, len, EPreamble::Long);
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
	
	
	void SetAccessoryAddress(uint8_t *msg, uint16_t addr)
	{
		if (addr <=  ADDR_ACCESSORY_9BIT_MAX)
		{
			// 9.2.1:431 The most significant bits of the 9-bit address are bits 4-6 of the second data byte. By convention these bits (bits 4-6 of
			// the second data byte) are in ones complement.
			msg[0] = ADDR_ACCESSORY | (addr & 0b00111111);
			msg[1] = ADDR2_ACCESSORY | ((~(addr >> 2) ) & 0b01110000);
		}
		else
		{
			// 9.2.1:431  seems to apply to 11 bit addresses too.  I'm guessing bits 10-11 of the address are ones complement, and are stored in bits 1-2 of byte two.
			msg[0] = ADDR_ACCESSORY | (addr & 0b00111111);
			msg[1] = ADDR2_ACCESSORY_EXTENDED | ((~(addr >> 2) ) & 0b01110000) | ((addr >> 9) & 0b110);
		}
	}
	
	bool VerifyAccessoryCVBit(uint16_t addr, uint16_t cv, uint8_t bit, bool set)
	{
		CVDEBUG("B:%d ", bit);
		
		uint8_t msg[6] = { 0, 0, (uint8_t)(INS_ACC_CV_ACCESS | INS_ACC_CV_BIT | (((cv - 1) >> 8) & 3)), (uint8_t)((cv - 1) & 0xFF), (uint8_t)(DATA_CV_VERIFY_BIT | (set ? DATA_BIT : 0) | bit), 0 };
		SetAccessoryAddress(msg, addr);
			
		SetErrorByte(msg, countof(msg));
		bool ret = VerifyMsg(msg, countof(msg));

		CVDEBUG("\n");
		return ret;
	}
	
	bool VerifyAccessoryByte(uint16_t addr, uint16_t cv, uint8_t value)
	{
		CVDEBUG("U8:%d ", value);
		
		uint8_t msg[6] = { 0, 0, (uint8_t)(INS_ACC_CV_ACCESS | INS_ACC_CV_VERIFY_BYTE | (((cv - 1) >> 8) & 3)), (uint8_t)((cv - 1) & 0xFF), value, 0 };
		SetAccessoryAddress(msg, addr);

		SetErrorByte(msg, countof(msg));
		bool ret = VerifyMsg(msg, countof(msg));

		CVDEBUG("\n");
		return ret;
	}	
	
	EErrorCode::EErrorCode ReadAccessoryCV(uint16_t addr, uint16_t cv, uint8_t &value)
	{
		value = 0;
		for (int i = 0; i < 8; i++)
		{
			bool bitValue = VerifyAccessoryCVBit(addr, cv, i, true);
			if ( bitValue )
				value |= 1 << i;
			if (i == 0 && !bitValue)
			{
				// double check the first bit acks when we send 0 (ie check we are actually talking to someone)
				if (!VerifyAccessoryCVBit(addr, cv, 0, false))
					return EErrorCode::NoACK;
			}
		}
		
		if(VerifyAccessoryByte(addr, cv, value))					// verify byte
		{
			CVDEBUG("Verified CV(%d)=%d\n", cv, value);
			return EErrorCode::Success;
		}
		else
		{
			CVDEBUG("Failed Verification! CV(%d)=%d\n", cv, value);
			return EErrorCode::ValueMismatch;
		}
	}
	
	bool VerifyMsg_SM(uint8_t *msg, uint8_t len)
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
			SendDCCMessage(msg, len, EPreamble::Long);
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
	
	bool VerifyBit_SM(uint16_t cv, uint8_t bit, bool set)
	{
		CVDEBUG("B:%d ", bit);
		
		uint8_t msg[4] = { (uint8_t)(INS_CV_BIT | (((cv - 1) >> 8) & 3)), (uint8_t)((cv - 1) & 0xFF), (uint8_t)(DATA_CV_VERIFY_BIT | (set ? DATA_BIT : 0) | bit), 0 };
		SetErrorByte(msg, countof(msg));
		bool ret = VerifyMsg_SM(msg, countof(msg));

		CVDEBUG("\n");
		return ret;
	}
	
	bool VerifyByte_SM(uint16_t cv, uint8_t value)
	{
		CVDEBUG("U8:%d ", value);
		
		uint8_t msg[4] = { (uint8_t)(INS_CV_VERIFY_BYTE | (uint8_t)(((cv - 1) >> 8) & 3)), (uint8_t)((cv - 1) & 0xFF), value, 0 };
		SetErrorByte(msg, countof(msg));
		bool ret = VerifyMsg_SM(msg, countof(msg));

		CVDEBUG("\n");
		return ret;
	}

	// Direct mode
	EErrorCode::EErrorCode ReadCV_SM(uint16_t cv, uint8_t &value)
	{
		value = 0;
		for (int i = 0; i < 8; i++)
		{
			bool bitValue = VerifyBit_SM(cv, i, true);
			if ( bitValue )
				value |= 1 << i;
			if (i == 0 && !bitValue)
			{
				// double check the first bit acks when we send 0 (ie check we are actually talking to someone)
				if (!VerifyBit_SM(cv, 0, false))
					return EErrorCode::NoACK;
			}
		}
		
		if(VerifyByte_SM(cv, value))						// verify byte
		{
			CVDEBUG("Verified VC(%d)=%d\n", cv, value);
			return EErrorCode::Success;
		}
		else
		{
			CVDEBUG("Failed Verification! VC(%d)=%d\n", cv, value);
			return EErrorCode::ValueMismatch;
		}
	}
	
	EErrorCode::EErrorCode WriteCV_SM(uint16_t cv, uint8_t value)
	{
		uint8_t msg[4] = { (uint8_t)(INS_CV_WRITE_BYTE | (uint8_t)(((cv - 1) >> 8) & 3)), (uint8_t)((cv - 1) & 0xFF), value, 0 };
		SetErrorByte(msg, 4);
		if (!VerifyMsg_SM(msg, sizeof(msg)))
			return EErrorCode::NoACK;
		if (!VerifyByte_SM(cv, value))
			return EErrorCode::ValueMismatch;
		return EErrorCode::Success;
	}
	
	uint16_t selectNextCV(uint16_t cv)
	{
		cv++;
		if (cv == 1025)
			cv = 0;
		return cv;
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
			// TODO - if not enabled, we need to reply to the message.
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
								uint8_t address=0, config=0, manufacturer=0, version=0;
								uint16_t extendedAddress = 0;
								EErrorCode::EErrorCode result;
								
								result = ReadCV_SM(1, address);
								if(result == EErrorCode::Success) 
								{
									if ( result == EErrorCode::Success ) ReadCV_SM(29, config);
									if ( result == EErrorCode::Success ) ReadCV_SM(8, manufacturer);
									if ( result == EErrorCode::Success ) ReadCV_SM(7, version);
									extendedAddress = 0;
									if (config > 0 && (config & CV29_TWO_BYTE_ADDRESS) != 0)
									{
										uint8_t addrMSB, addrLSB;
										if ( result == EErrorCode::Success ) ReadCV_SM(17, addrMSB);
										if ( result == EErrorCode::Success ) ReadCV_SM(18, addrLSB);
										extendedAddress = (addrMSB << 8) | addrLSB;
									}
								}
								else  // if this fails, try reading accessory decoders
								{
									result = ReadAccessoryCV(ADDR_ACCESSORY_BROADCAST, 1, address);
									if (result == EErrorCode::Success) 
									{
										uint8_t addrMSB, addrLSB;
										addrLSB = address;
										result = ReadAccessoryCV(ADDR_ACCESSORY_BROADCAST, 9, addrMSB);
										if (result == EErrorCode::Success)
										{
											extendedAddress = (addrMSB << 8) | addrLSB;
											address = extendedAddress;
										}
									}
									if ( result == EErrorCode::Success ) ReadAccessoryCV(ADDR_ACCESSORY_BROADCAST, 29, config);
									if ( result == EErrorCode::Success ) ReadAccessoryCV(ADDR_ACCESSORY_BROADCAST, 8, manufacturer);
									if ( result == EErrorCode::Success ) ReadAccessoryCV(ADDR_ACCESSORY_BROADCAST, 7, version);
								}
								
								
								// Send Reply
								UIMsg uiMsg;
								uiMsg.type = EUIMessageType::ScanTrackReply;
								uiMsg.scan.result = result;
								uiMsg.scan.address = address;
								uiMsg.scan.config = config;
								uiMsg.scan.manufacturer = manufacturer;
								uiMsg.scan.version = version;
								uiMsg.scan.extendedAddress = extendedAddress;
								uimsg.Send(uiMsg);
								break;
							}
							case EProgTrackMessage::StopScanAllCVs:
							{
								// If we are scanning, there will be a message in the queue.
								int count = uxQueueMessagesWaiting(queueHandle);
								for ( int i = 0; i < count; i++ )
								{
									ProgTrackMessage msg;
									if (xQueueReceive(queueHandle, &msg, 0) == pdPASS)
									{
										if (msg.type == EProgTrackMessage::ScanAllCVs)
											msg.scanAll.cancel = true;
										xQueueSend(queueHandle, &msg, 0);
										break;
									}
									else
										break;
								}
								break;
							}
							case EProgTrackMessage::ScanAllCVs:
							{
								UIMsg uiMsg;
								if (msg.scanAll.cancel)
								{
									uiMsg.type = EUIMessageType::ScanAllCVsReply;
									uiMsg.scanAllCVs.result = EErrorCode::Complete;
									uimsg.Send(uiMsg);
									break;
								}
								
								uint16_t nextCV = msg.scanAll.cv;
								if (nextCV == 0)	// First
									nextCV = 1;	

								uint8_t value;
								EErrorCode::EErrorCode result = ReadCV_SM(nextCV, value);
								
								// Send Reply
								uiMsg.type = EUIMessageType::ScanAllCVsReply;
								uiMsg.scanAllCVs.result = result;
								uiMsg.scanAllCVs.CV = nextCV;
								uiMsg.scanAllCVs.value = value;
								uimsg.Send(uiMsg);
								
								if (result != EErrorCode::Success && msg.scanAll.retries < MAX_SCANALL_RETRIES)
								{
									msg.scanAll.retries++;
									SendMsg(msg);
								}
								else
								{
									nextCV = selectNextCV(nextCV);
									if (nextCV > 0)
									{
										// Next
										msg.scanAll.cv = nextCV;
										msg.scanAll.retries = 0;
										SendMsg(msg);
									}
									else
									{
										// Done
										uiMsg.type = EUIMessageType::ScanAllCVsReply;
										uiMsg.scanAllCVs.result = EErrorCode::Complete;
										uimsg.Send(uiMsg);
									}
								}
								break;
							}
							case EProgTrackMessage::WriteCV:
							{
								EErrorCode::EErrorCode ret = WriteCV_SM(msg.cv.cv, msg.cv.value);
								
								// Send Reply
								// Send Reply
								UIMsg uiMsg;
								uiMsg.type = EUIMessageType::CVWriteReply;
								uiMsg.result = ret;
								uimsg.Send(uiMsg);
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
				{
					ProgTrackMessage msg;
					if (xQueueReceive(queueHandle, &msg, 0) == pdPASS)
					{
						switch (msg.type)
						{
							case EProgTrackMessage::ScanTrack:
								{
									// Send Reply
									UIMsg uiMsg;
									uiMsg.type = EUIMessageType::ScanTrackReply;
									uiMsg.scan.result = EErrorCode::NoTrackPower;
									uimsg.Send(uiMsg);
									break;
								}
							case EProgTrackMessage::WriteCV:
								{
									// Send Reply
									UIMsg uiMsg;
									uiMsg.type = EUIMessageType::CVWriteReply;
									uiMsg.result = EErrorCode::NoTrackPower;
									uimsg.Send(uiMsg);
									break;
								}
							case EProgTrackMessage::ScanAllCVs:
								{
									// Send Reply
									UIMsg uiMsg;
									uiMsg.type = EUIMessageType::ScanAllCVsReply;
									uiMsg.result = EErrorCode::Complete;
									uimsg.Send(uiMsg);
									break;
								}
							default:
								break;
						}
					}
				}
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
extern void ProgrammingTrack_DCC_WriteCV(uint16_t cv, uint8_t value);
extern void ProgrammingTrack_DCC_ScanAllCVs();
extern void ProgrammingTrack_DCC_StopScanAllCVs();