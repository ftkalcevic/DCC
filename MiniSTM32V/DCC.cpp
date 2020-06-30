#include "main.h"
#include "cmsis_os.h"
#include "task.h"
#include "DCC.h"
#include "stm32f1xx_hal_tim.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
extern TIM_HandleTypeDef htim4;


const int HALF_BIT_1_MIN = 52;
const int HALF_BIT_1_MAX = 64;
const int HALF_BIT_1_MAX_DELTA = 6;

const int HALF_BIT_0_MIN = 90;
const int HALF_BIT_0_MAX = 10000;
const int HALF_BIT_0_MAX_DURATION = 12000;

const int MIN_PREAMBLE_COUNT = 10;
const int MAX_DCC_MESSAGE = 6;

const int BIT_BUFFER_SIZE = 1024;
DCC dcc;
volatile uint16_t dccBitBuffer[BIT_BUFFER_SIZE];

extern "C" void dccTask_Start(void *argument)
{
	dcc.Run();
}


void DCC::Run()
{
	printf("Starting\n");
#ifdef TWO_DMA_CHANNELS	
	// Start the timer interrupt capture DMA - capture via 2 dma channels
	HAL_StatusTypeDef status;
	memset(dccBitBuffer, 1, sizeof(dccBitBuffer));
	//status = HAL_TIM_IC_Start_DMA(&htim4, TIM_CHANNEL_1, (uint32_t*)dccBitBuffer, countof(dccBitBuffer)/2);
	memset(dccBitBuffer2, 1, sizeof(dccBitBuffer2));
	//status = HAL_TIM_IC_Start_DMA(&htim4, TIM_CHANNEL_2, (uint32_t*)dccBitBuffer2, countof(dccBitBuffer2)/2);
	
    HAL_DMA_Start(htim4.hdma[TIM_DMA_ID_CC2], (uint32_t)&htim4.Instance->CCR1, (uint32_t)dccBitBuffer, countof(dccBitBuffer) / 2);
    HAL_DMA_Start(htim4.hdma[TIM_DMA_ID_CC1], (uint32_t)&htim4.Instance->CCR2, (uint32_t)dccBitBuffer2, countof(dccBitBuffer2) / 2);
    __HAL_TIM_ENABLE_DMA(&htim4, TIM_DMA_CC2);
    __HAL_TIM_ENABLE_DMA(&htim4, TIM_DMA_CC1);
	TIM_CCxChannelCmd(htim4.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
	TIM_CCxChannelCmd(htim4.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

	/* Enable the Peripheral, except in trigger mode where enable is automatically done with trigger */
	htim4.Instance->SMCR &= ~TIM_SMCR_TS;
	htim4.Instance->SMCR |= TIM_TS_TI1FP1;
    //htim4.Instance->SMCR &= ~TIM_SMCR_SMS;
    //htim4.Instance->SMCR |= TIM_SLAVEMODE_RESET;
	htim4.Instance->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC2P;
    __HAL_TIM_ENABLE(&htim4);
#endif
	
#define SINGLE_DMA_CHANNEL_BURST	
#ifdef SINGLE_DMA_CHANNEL_BURST	
	// Start the timer interrupt capture DMA - capture via 2 dma channels
	HAL_StatusTypeDef status;
    status = HAL_DMA_Start(htim4.hdma[TIM_DMA_ID_CC2], (uint32_t)&htim4.Instance->DMAR, (uint32_t)dccBitBuffer, countof(dccBitBuffer));
	
	#define  BURST_SIZE 2
	htim4.Instance->DCR = TIM_DMABASE_CCR1 | ((BURST_SIZE - 1) << TIM_DCR_DBL_Pos);
	
	TIM_CCxChannelCmd(htim4.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
	TIM_CCxChannelCmd(htim4.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

	/* Enable the Peripheral, except in trigger mode where enable is automatically done with trigger */
	htim4.Instance->SMCR &= ~TIM_SMCR_TS;
	htim4.Instance->SMCR |= TIM_TS_TI1FP1;
    //htim4.Instance->SMCR &= ~TIM_SMCR_SMS;
    //htim4.Instance->SMCR |= TIM_SLAVEMODE_RESET;	// reset counter after cc1 and cc2
	htim4.Instance->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC2P;
    __HAL_TIM_ENABLE(&htim4);
    __HAL_TIM_ENABLE_DMA(&htim4, TIM_DMA_CC2);
#endif	

#ifdef USE_STM_CODE
	// HAL_TIM_DMABurst_ReadStart doesn't allow us to specified a received buffer that isn't the same length as the burst length
	// HAL_StatusTypeDef status;
	// memset(dccBitBuffer, 1, sizeof(dccBitBuffer));
	// status = HAL_TIM_DMABurst_ReadStart(&htim4, TIM_DMABASE_CCR1, TIM_DMA_CC2, dccBitBuffer, (2<<8));
#endif
		
	enum EState
	{
		Preambling,
		FirstHalf,
		SecondHalf1,
		SecondHalf0,
	} state = EState::Preambling;

	uint32_t errorInsufficentPreambleBits = 0;
	uint32_t errorMalformedFirstHalf = 0;
	uint32_t errorMalformedSecond0 = 0;
	uint32_t errorMalformedSecond1 = 0;
	uint32_t errorMalformedPreamble = 0;
	
	uint16_t read_ptr = 0;
	uint16_t last_time = 0, start_time;
	uint16_t preambleHalfBits = 0;
	for(;;)
	{
		while ( (BIT_BUFFER_SIZE - htim4.hdma[TIM_DMA_ID_CC2]->Instance->CNDTR) != read_ptr)
		{
			uint16_t time = dccBitBuffer[read_ptr];
			
			if ( read_ptr == BIT_BUFFER_SIZE-1 )
				read_ptr = 0;
			else
				read_ptr++;

			uint16_t period = time - last_time;
			last_time = time;
//			static uint32_t dwtlast;
//			uint32_t dwtcnt = DWT->CYCCNT;
//			uint32_t diff = dwtcnt - dwtlast;
//			dwtlast = dwtcnt;

			static TickType_t tick = 0;
			if ( xTaskGetTickCount() > tick )
			{
				printf( "E %d %d %d %d %d\n", errorInsufficentPreambleBits, errorMalformedFirstHalf, errorMalformedSecond0, errorMalformedSecond1, errorMalformedPreamble );
				tick = xTaskGetTickCount() + pdMS_TO_TICKS(1000);
			}
			
			switch(state)
			{
				case EState::Preambling:
					if (period >= HALF_BIT_1_MIN && period <= HALF_BIT_1_MAX)
					{
						preambleHalfBits++;
					}
					else  if (period >= HALF_BIT_0_MIN && period <= HALF_BIT_0_MAX)
					{
						if (preambleHalfBits > MIN_PREAMBLE_COUNT * 2)
						{
							ProcessBit(1, true);
							start_time = last_time;
							state = EState::SecondHalf0;
						}
						else
						{
							errorInsufficentPreambleBits++;
							preambleHalfBits = 0;
						}
					}
					else
					{
						preambleHalfBits = 0;
						errorMalformedPreamble++;
					}
					break;
				case EState::FirstHalf:
					if (period >= HALF_BIT_1_MIN && period <= HALF_BIT_1_MAX)
					{
						start_time = last_time;
						state = EState::SecondHalf1;
					}
					else  if (period >= HALF_BIT_0_MIN && period <= HALF_BIT_0_MAX)
					{
						start_time = last_time;
						state = EState::SecondHalf0;
					}
					else
					{
						// Error malformed bit
						state = EState::Preambling;
						preambleHalfBits = 0;
						errorMalformedFirstHalf++;
					}
					break;
				case EState::SecondHalf1:
					if (period >= HALF_BIT_1_MIN && period <= HALF_BIT_1_MAX && abs(period - (uint16_t)(last_time - start_time)) <= HALF_BIT_1_MAX_DELTA)
					{
						if (ProcessBit(1))
							state = EState::FirstHalf;
						else
							state = EState::Preambling;
					}
					else 
					{
						// if (period >= HALF_BIT_1_MIN && period <= HALF_BIT_1_MAX)
						//		Delta Error 1 bit
						// else
						//		Error malformed 1 bit
						state = EState::Preambling;
						preambleHalfBits = 0;
						errorMalformedSecond1++;
					}
					break;
					
				case EState::SecondHalf0:
					if (period >= HALF_BIT_0_MIN && period <= HALF_BIT_0_MAX && (uint16_t)(last_time - start_time) <= HALF_BIT_0_MAX_DURATION)
					{
						if (ProcessBit(0))
							state = EState::FirstHalf;
						else
							state = EState::Preambling;
					}
					else 
					{
						// if (period >= HALF_BIT_0_MIN && period <= HALF_BIT_0_MAX )
						//		Too long total 0
						// else
						//		Error malformed 0 bit
						state = EState::Preambling;
						preambleHalfBits = 0;
						errorMalformedSecond0++;
					}
					break;
			}
		}
		taskYIELD();
	}
	
}

// return true if continue processing
bool DCC::ProcessBit(uint8_t bit, bool reset)
{
	static enum EState
	{
		Init,
		WaitForByteStart,
		ReadByte
	} state = Init;
	static uint8_t bitPtr;
	static uint8_t bytePtr;
	static uint8_t activeByte;
	static uint8_t buffer[MAX_DCC_MESSAGE];
	
	if (reset || state == EState::Init)
	{
		bitPtr = 0;
		bytePtr = 0;
		activeByte = 0;
		state = EState::WaitForByteStart;
	}
	
	if (reset)
		return true;
	
	switch (state)
	{
		case EState::WaitForByteStart:
			if (bit == 0)
			{
				bitPtr = 0;
				activeByte = 0;
				state = EState::ReadByte;
			}
			else if ( bit == 1 && bytePtr > 0 )
			{
				ProcessPacket(buffer, bytePtr);
				state = EState::Init;
			}
			break;

		case EState::ReadByte:
			activeByte = (activeByte << 1) | bit;
			if (bitPtr == 7)
			{
				if (bytePtr == MAX_DCC_MESSAGE - 1)
				{
					// Error message too long
					state = EState::Init;
				}
				else
				{
					buffer[bytePtr] = activeByte;
					bytePtr++;
					state = EState::WaitForByteStart;
				}
			}
			else
			{
				bitPtr++;
			}
			break;
	}
	
	if (state == EState::Init)
		return false;
	else
		return true;
}


void DCC::ProcessPacket(uint8_t const * buffer, uint8_t len)
{
	// Check Error byte
	uint8_t err = buffer[0];
	for (int i = 1; i < len - 1; i++)
		err ^= buffer[i];
	if (err != buffer[len - 1])
	{
		// error CRC check failed
		return;
	}
	
	for (int i = 0; i < len; i++)
		printf("%02X ", buffer[i]);
	if (len == 3)
	{
		printf("ADDR:%d ", buffer[0]);
		if ((buffer[1] & 0b11000000) == 0b01000000)
		{
			printf("D%d C%d S%d", buffer[1] & 0b00100000 ? 1 : 0, buffer[1] & 0b00010000 ? 1 : 0, buffer[1] & 0b00001111);
		}
	}
	printf("\n");
}

