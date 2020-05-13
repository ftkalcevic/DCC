#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include <string.h>

extern "C" void DCCTask_Entry(void *argument);

enum DCCType 
{
	MainTrack = 0,
	ProgrammingTrack = 1
};

const int PREAMBLE_BITS = 14;
const int DCC_ONE_TIME = 58-1;	// usec
const int DCC_ZERO_TIME = 100-1;	// usec
const int MAX_DCC_MESSAGE = 6;
const int MAX_DCC_MESSAGES = 100;
const int BIT_BUFFER_SIZE = (PREAMBLE_BITS + MAX_DCC_MESSAGE*(8 + 1) + 1);	// max bits

struct DCCMessage
{
	uint16_t address;
	uint8_t msgLen;
	uint8_t msg[MAX_DCC_MESSAGE];
	uint32_t lastSendTime;
};


	
template<DCCType type, int ARR_OFFSET, int CCn_OFFSET, int BURST_SIZE >
class DCC
{
	DCCMessage messageTable[MAX_DCC_MESSAGES];
	int messageCount;
	bool trackEnabled;
	uint32_t bitBuffer[BIT_BUFFER_SIZE*BURST_SIZE];
	volatile bool sent;


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



	void Initialise()
	{
		messageCount = 0;
		trackEnabled = false;
		// clear timer dma buffer.  Only those registers used will be set, others remain 0
		memset(bitBuffer, 0, sizeof(bitBuffer));
	
		if (type == DCCType::MainTrack)
		{
			// TIM8_CH2, DMA2_Stream1_Channel7 APB2
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
			RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
			__HAL_RCC_GPIOC_CLK_ENABLE();
			/**TIM8 GPIO Configuration    
			PC7     ------> TIM8_CH2 
			*/
			GPIO_InitTypeDef GPIO_InitStruct = { 0 };
			GPIO_InitStruct.Pin = DCC_Signal_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
			HAL_GPIO_Init(DCC_Signal_GPIO_Port, &GPIO_InitStruct);
	
			DMA2_Stream1->CR |= DMA_CHANNEL_7;
			DMA2_Stream1->CR |= DMA_MEMORY_TO_PERIPH |
								DMA_PINC_DISABLE | DMA_MINC_ENABLE |
								DMA_PDATAALIGN_WORD | DMA_MDATAALIGN_WORD |
								DMA_NORMAL | DMA_PRIORITY_HIGH | DMA_IT_TC;	
	
			TIM8->CR1 = TIM_CLOCKDIVISION_DIV1 | TIM_COUNTERMODE_UP | TIM_AUTORELOAD_PRELOAD_ENABLE;
			TIM8->CCMR1 = (TIM_OCMODE_PWM1 << 8) | TIM_CCMR1_OC2PE;
			TIM8->ARR = 1;
			TIM8->CCR2 = 0;
			TIM8->PSC = 180; 	// TIM8 on APB2 (180Mhz)
			TIM8->DIER = TIM_DMA_UPDATE;
			//	TIM8->DCR = TIM_DMABase_ARR | TIM_DMABurstLength_4Transfers;
				TIM8->DCR = TIM_DMABase_ARR | ((BURST_SIZE - 1) << TIM_DCR_DBL_Pos);
			TIM8->CCER |= TIM_CCER_CC2P;
			TIM8->BDTR |= TIM_BDTR_MOE;
			/* Enable UEV by setting UG bit to Load buffer data into preload registers */
			TIM8->EGR |= TIM_EGR_UG;
			//	while ((TIM8->EGR & TIM_EGR_UG) == SET)
			//		continue;
			//	/* Enable UEV by setting UG bit to load data from preload to activeregisters */
			//	TIM8->EGR |= TIM_EGR_UG;
		}
		else
		{
			// TIM3_CH1, DMA1_Stream2_Channel5 APB1
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			__HAL_RCC_GPIOC_CLK_ENABLE();

			/**TIM3 GPIO Configuration    
			PC6     ------> TIM3_CH1 
			*/
			GPIO_InitTypeDef GPIO_InitStruct = {0};
			GPIO_InitStruct.Pin = PrgTrk_DCC_Signal_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
			HAL_GPIO_Init(PrgTrk_DCC_Signal_GPIO_Port, &GPIO_InitStruct);
			
			DMA1_Stream2->CR |= DMA_CHANNEL_5;
			DMA1_Stream2->CR |= DMA_MEMORY_TO_PERIPH |
								DMA_PINC_DISABLE | DMA_MINC_ENABLE |
								DMA_PDATAALIGN_WORD | DMA_MDATAALIGN_WORD |
								DMA_NORMAL | DMA_PRIORITY_HIGH | DMA_IT_TC;	
	
			TIM3->CR1 = TIM_CLOCKDIVISION_DIV1 | TIM_COUNTERMODE_UP | TIM_AUTORELOAD_PRELOAD_ENABLE;
			TIM3->CCMR1 = TIM_OCMODE_PWM1 | TIM_CCMR1_OC1PE;
			TIM3->ARR = 1;
			TIM3->CCR1 = 0;
			TIM3->PSC = 90; 	// TIM3 on APB1 (90MHz)
			TIM3->DIER = TIM_DMA_UPDATE;
			//	TIM3->DCR = TIM_DMABase_ARR | TIM_DMABurstLength_4Transfers;
			TIM3->DCR = TIM_DMABase_ARR | ((BURST_SIZE - 1) << TIM_DCR_DBL_Pos);
			TIM3->CCER |= TIM_CCER_CC1P;
			//TIM3->BDTR |= TIM_BDTR_MOE;
			/* Enable UEV by setting UG bit to Load buffer data into preload registers */
			TIM3->EGR |= TIM_EGR_UG;
			//	while ((TIM8->EGR & TIM_EGR_UG) == SET)
			//		continue;
			//	/* Enable UEV by setting UG bit to load data from preload to activeregisters */
			//	TIM8->EGR |= TIM_EGR_UG;			
		}

	}



	void Run()
	{
	#ifdef DEBUG
		trackEnabled = true;
	#endif

		for (;;)
		{
			// if trackenabled changed
				// if disabled, clear msg queue, reset msg repeat
		
			// If new message
			if(false)
			{
				// Read message
				// if trackEnabled, send message
				// Update local table
				//      loco commands need to be stored for playback later
			}
			// Else process message table
			else 
			{
				// We can't find a message to send - either empty table, or 5ms gap issue.
				static uint8_t idleMessage[3] = { 0xFF, 0, 0xFF };
				SendDCCMessage(idleMessage, sizeof(idleMessage));
				while (!sent)
					continue;
			}
		
			// need to handle a 5ms gap before sending a message to the same decoder.
			// shortest message - 14 preamble + 3 bytes + 3 start bits + 1 end bit
			//                    14 * 58 + 3 * 8 * 58 + 3*100 + 1 * 58 = 2562us

		vTaskDelay(5);
		}
	}


	void AddBit(int &index, bool n)
	{
		bitBuffer[index + ARR_OFFSET] = 2*(n ? DCC_ONE_TIME : DCC_ZERO_TIME);
		bitBuffer[index + CCn_OFFSET] = (n ? DCC_ONE_TIME : DCC_ZERO_TIME);
		index  += BURST_SIZE;
	}

	void SendDCCMessage(uint8_t *msg, uint8_t len)
	{
		sent = false;
		// Build a buffer of timings - a pair for bit off/bit on.
		// Use dma to stream the timings to the timer. (as used in opentx pxx)
		int index = 0;
		for (int i = 0; i < PREAMBLE_BITS; i++)
			AddBit(index, 1);
		for (int m = 0; m < len; m++)
		{
			AddBit(index, 0);
			for (int i = 0; i < 8; i++)
				AddBit(index, msg[m] & (1 << (7 - i)));
		}
		AddBit(index, 1);
		for ( int i = 0; i < BURST_SIZE;i++)
			bitBuffer[index++] = 0;	// add terminator.  Easier to do this than also process the tim update interrupt after dma is finished.

		if(type == DCCType::MainTrack)
		{
			DMA2_Stream1->PAR = (uint32_t)&(TIM8->DMAR);
			DMA2_Stream1->M0AR = (uint32_t)(bitBuffer);
			DMA2_Stream1->NDTR = index;

			TIM8->ARR = 1;
			TIM8->CCR2 = 0;
			TIM8->EGR |= TIM_EGR_UG;

			DMA2_Stream1->CR |= DMA_SxCR_EN;
			TIM8->CCER |= TIM_CCER_CC2E;
			TIM8->CR1 |= TIM_CR1_CEN;
		}
		else
		{
			DMA1_Stream2->PAR = (uint32_t)&(TIM3->DMAR);
			DMA1_Stream2->M0AR = (uint32_t)(bitBuffer);
			DMA1_Stream2->NDTR = index;

			TIM3->ARR = 1;
			TIM3->CCR1 = 0;
			TIM3->EGR |= TIM_EGR_UG;

			DMA1_Stream2->CR |= DMA_SxCR_EN;
			TIM3->CCER |= TIM_CCER_CC1E;
			TIM3->CR1 |= TIM_CR1_CEN;		
	
		}
	
	}

	void DCCSent()
	{ 
		sent = true;
	}
};
