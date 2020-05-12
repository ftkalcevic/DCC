#include "dcc.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include <string.h>
#include "stm32f4xx_hal_tim_plus.h"



DCC mainTrack(  
		Boost_CS_GPIO_Port, Boost_CS_Pin,
		Boost_Enable_GPIO_Port, Boost_Enable_Pin,
		Boost_Fault_GPIO_Port, Boost_Fault_Pin,
		&hadc1, 1,
		DCC_Signal_GPIO_Port, DCC_Signal_Pin );					// tim8_2
DCC programmingTrack(   
		PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin,
		PrgTrk_Enable_GPIO_Port, PrgTrk_Enable_Pin,
		PrgTrk_Fault_GPIO_Port, PrgTrk_Fault_Pin,
		PrgTrk_ACK_GPIO_Port, PrgTrk_ACK_Pin,
		PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin );	// tim3_1 

static uint8_t idleMessage[3] = { 0xFF, 0, 0xFF };

extern "C"
{
	void DCCTask_Entry(void *argument)
	{
		DCCType type = static_cast<DCCType>(reinterpret_cast<int>(argument));
		
		DCC *dcc;
		switch (type)
		{
			case DCCType::ProgrammingTrack:
				dcc = &programmingTrack;
				vTaskDelete(NULL);
				break;
			case DCCType::MainTrack:
				dcc = &mainTrack;
				break;
		}
		dcc->Initialise();
		dcc->Run();
		
		
		for (;;)
		{
			
			osDelay(1);
		}
	}
}




// main track TIM8_2
#define ARR_OFFSET	0
#define CCn_OFFSET	3
#define BURST_SIZE	4


void DCC::Initialise()
{
	messageCount = 0;
	trackEnabled = false;
	// clear timer dma buffer.  Only those registers used will be set, others remain 0
	memset(bitBuffer, 0, sizeof(bitBuffer));
	
	RCC->AHB1LPENR |= RCC_AHB1ENR_DMA2EN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
	__HAL_RCC_GPIOC_CLK_ENABLE();
	/**TIM8 GPIO Configuration    
	PC7     ------> TIM8_CH2 
	*/
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DCC_Signal_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
	HAL_GPIO_Init(DCC_Signal_GPIO_Port, &GPIO_InitStruct);
	
	DMA2_Stream1->CR |= DMA_CHANNEL_7;
	DMA2_Stream1->CR |= DMA_MEMORY_TO_PERIPH |
						DMA_PINC_DISABLE| DMA_MINC_ENABLE|
						DMA_PDATAALIGN_WORD| DMA_MDATAALIGN_WORD |
						DMA_NORMAL | DMA_PRIORITY_HIGH | DMA_IT_TC;	
	
	TIM8->CR1 = TIM_CLOCKDIVISION_DIV1 | TIM_COUNTERMODE_UP | TIM_AUTORELOAD_PRELOAD_ENABLE;
	TIM8->CCMR1 = (TIM_OCMODE_PWM1 << 8) | TIM_CCMR1_OC2PE;
	TIM8->ARR = 1;
	TIM8->CCR2 = 0;
	TIM8->PSC = 180;	// TIM8 on APB2 (180Mhz), TIM3 on APB1 (90MHz)
	TIM8->DIER = TIM_DMA_UPDATE;
//	TIM8->DCR = TIM_DMABase_ARR | TIM_DMABurstLength_4Transfers;
	TIM8->DCR = TIM_DMABase_ARR | ((BURST_SIZE-1) << TIM_DCR_DBL_Pos);
	TIM8->CCER |= TIM_CCER_CC2P;
	TIM8->BDTR |= TIM_BDTR_MOE;
	/* Enable UEV by setting UG bit to Load buffer data into preload registers */
	TIM8->EGR |= TIM_EGR_UG;
//	while ((TIM8->EGR & TIM_EGR_UG) == SET)
//		continue;
//	/* Enable UEV by setting UG bit to load data from preload to activeregisters */
//	TIM8->EGR |= TIM_EGR_UG;

}



void DCC::Run()
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


void DCC::AddBit(int &index, bool n)
{
	bitBuffer[index + ARR_OFFSET] = 2*(n ? DCC_ONE_TIME : DCC_ZERO_TIME);
	bitBuffer[index + CCn_OFFSET] = (n ? DCC_ONE_TIME : DCC_ZERO_TIME);
	index  += BURST_SIZE;
}

void DCC::SendDCCMessage(uint8_t *msg, uint8_t len)
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
	bitBuffer[index++] = 0;	// add terminator.  Easier to do this than also process the tim update interrupt after dma is finished.
	bitBuffer[index++] = 0;
	bitBuffer[index++] = 0;
	bitBuffer[index++] = 0;
	
	// Initialise timer, dma and interrupt.
	//HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
//	htim8.Instance->ARR = 0xFFFF;
//	htim8.Instance->CCR2 = 0xFFF;
	//TIM_CCxChannelCmd(htim8.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
	//HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
	//HAL_TIM_DMABurst_WriteStart2(&htim8, TIM_DMABASE_DMAR, TIM_DMA_UPDATE, bitBuffer, index, BURST_SIZE);
	
//bitBuffer[ARR_OFFSET] = 500;
//bitBuffer[CCn_OFFSET] = 25;
//bitBuffer[BURST_SIZE+ARR_OFFSET] = 500;
//bitBuffer[BURST_SIZE+CCn_OFFSET] = 25;	
//	TIM8->CCER = 0;
//	TIM8->CR1 = 0;
	

	//	htim8.hdma[TIM_DMA_ID_UPDATE]->Instance->CR =  DMA_MEMORY_TO_PERIPH |
	//												   DMA_PINC_DISABLE| DMA_MINC_ENABLE|
	//												   DMA_PDATAALIGN_WORD| DMA_MDATAALIGN_WORD |
	//												   DMA_PRIORITY_HIGH | DMA_IT_TC;
	//	htim8.hdma[TIM_DMA_ID_UPDATE]->Instance->NDTR = index;
	//	htim8.hdma[TIM_DMA_ID_UPDATE]->Instance->PAR =  (uint32_t)&(htim8.Instance->DMAR);
	//	htim8.hdma[TIM_DMA_ID_UPDATE]->Instance->M0AR = (uint32_t)bitBuffer;
	//	htim8.hdma[TIM_DMA_ID_UPDATE]->Instance->CR |=  DMA_SxCR_EN;
	DMA2_Stream1->PAR = (uint32_t)&(TIM8->DMAR);
	DMA2_Stream1->M0AR = (uint32_t)(bitBuffer);
	DMA2_Stream1->NDTR = index;

	
//	TIM8->CR1 = TIM_CLOCKDIVISION_DIV1 | TIM_COUNTERMODE_UP | TIM_AUTORELOAD_PRELOAD_ENABLE;
//	TIM8->CCMR1 = (TIM_OCMODE_PWM1 << 8) | TIM_CCMR1_OC2PE;
	TIM8->ARR = 1;
	TIM8->CCR2 = 0;
//	TIM8->PSC = 90;
//	TIM8->DIER = TIM_DMA_UPDATE;
//	TIM8->DCR = TIM_DMABase_ARR | TIM_DMABurstLength_4Transfers;
//	TIM8->CCER |= TIM_CCER_CC2P;
//	TIM8->BDTR |= TIM_BDTR_MOE;
	/* Enable UEV by setting UG bit to Load buffer data into preload registers */
	TIM8->EGR |= TIM_EGR_UG;
//	while ((TIM8->EGR & TIM_EGR_UG) == SET)
//		continue;
	/* Enable UEV by setting UG bit to load data from preload to activeregisters */
//	TIM8->EGR |= TIM_EGR_UG;
	

	DMA2_Stream1->CR |= DMA_SxCR_EN;
	TIM8->CCER |= TIM_CCER_CC2E;
	TIM8->CR1 |= TIM_CR1_CEN;
	
}

extern"C"
{
void DCC_Handle_DMA_IRQ()
{
	// DMA transfer completed (but Timer still going)
	DMA2->LIFCR |= DMA_LISR_TCIF1;
	DMA2_Stream1->CR &= ~DMA_SxCR_EN;
	mainTrack.DCCSent();
}
}