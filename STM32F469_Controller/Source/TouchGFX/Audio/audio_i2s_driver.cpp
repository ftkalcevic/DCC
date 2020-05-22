/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include "audio_i2s_driver.h"
#include "audio_arm.h"

const AudioBuffer * nextBuffer = 0;

//void setSampleRate(uint32_t frequency)
//{
////  uint32_t timer = (PERI2_FREQUENCY * TIMER_MULT_APB2) / frequency - 1 ;         // MCK/8 and 100 000 Hz
////
////  AUDIO_I2S_TIMER->CR1 &= ~TIM_CR1_CEN;
////  AUDIO_I2S_TIMER->CNT = 0;
////  AUDIO_I2S_TIMER->ARR = limit<uint32_t>(2, timer, 65535);
////  AUDIO_I2S_TIMER->CR1 |= TIM_CR1_CEN;
//}
//
//// Start TIMER6 at 100000Hz, used for i2s trigger
//void i2sTimerInit()
//{
////  AUDIO_I2S_TIMER->PSC = 0;                                                                                                  // Max speed
////  AUDIO_I2S_TIMER->ARR = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 100000 - 1;         // 10 uS, 100 kHz
////  AUDIO_I2S_TIMER->CR2 = 0;
////  AUDIO_I2S_TIMER->CR2 = 0x20;
////  AUDIO_I2S_TIMER->CR1 = TIM_CR1_CEN;
//}

static void i2sInit()
{
//  i2sTimerInit();
//
//  GPIO_InitTypeDef GPIO_InitStructure;
//
//  // SD Mode pin (enable/disable)
//  GPIO_InitStructure.GPIO_Pin = AUDIO_I2S_SDMODE_GPIO_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(AUDIO_I2S_SDMODE_GPIO, &GPIO_InitStructure);
//  GPIO_ResetBits(AUDIO_I2S_SDMODE_GPIO, AUDIO_I2S_SDMODE_GPIO_PIN);
//
//  // I2S/SPI pins
//	GPIO_InitStructure.GPIO_Pin = AUDIO_I2S_WS_GPIO_PIN | AUDIO_I2S_SD_GPIO_PIN | AUDIO_I2S_CK_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(AUDIO_I2S_SD_GPIO, &GPIO_InitStructure);
//  
//	GPIO_PinAFConfig(AUDIO_I2S_SD_GPIO, AUDIO_I2S_SD_GPIO_PinSource, AUDIO_I2S_GPIO_AF);
//	GPIO_PinAFConfig(AUDIO_I2S_CK_GPIO, AUDIO_I2S_CK_GPIO_PinSource, AUDIO_I2S_GPIO_AF);
//	GPIO_PinAFConfig(AUDIO_I2S_WS_GPIO, AUDIO_I2S_WS_GPIO_PinSource, AUDIO_I2S_GPIO_AF);
//
//	// I2S Clock - HS Source, I2SPLLM /8, I2SPLLN x384, I2SPLLQ /2, I2SPLLR /2
//	RCC->PLLI2SCFGR = __HAL_RCC_PLLI2S_CONFIG(8, 384, 2, 2);
//	RCC->CR |= RCC_CR_PLLI2SON;
//	while (!(RCC->CR & RCC_CR_PLLI2SRDY)) 
//		continue;
//	
//	I2S_InitTypeDef I2S_InitStruct;
//	I2S_InitStruct.I2S_Mode = I2S_Mode_MasterTx;
//	I2S_InitStruct.I2S_Standard = I2S_Standard_Phillips;
//	I2S_InitStruct.I2S_DataFormat = I2S_DataFormat_16b;
//	I2S_InitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
//	I2S_InitStruct.I2S_AudioFreq = I2S_AudioFreq_16k;			// Note - 16k, not 32k - MAX98357A takes stereo data, so we just give every 2nd word to the other channel.
//	I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
//	I2S_Init(AUDIO_I2S, &I2S_InitStruct);
//
//	GPIO_SetBits(AUDIO_I2S_SDMODE_GPIO, AUDIO_I2S_SDMODE_GPIO_PIN);
	
    //NVIC_EnableIRQ(AUDIO_I2S_TIM_IRQn);  // TODO needed? Need TC interrupt to stop spi after last word send
    //NVIC_SetPriority(AUDIO_I2S_TIM_IRQn, 7);
  	
    NVIC_EnableIRQ(AUDIO_I2S_DMA_Stream_IRQn);
    NVIC_SetPriority(AUDIO_I2S_DMA_Stream_IRQn, 7);
}

void audioConsumeCurrentBuffer()
{

  if (nextBuffer == 0) {

    nextBuffer = audioQueue.buffersFifo.getNextFilledBuffer();
    if (nextBuffer) {
		DMA_InitTypeDef DMA_InitStructure;
	    DMA_DeInit(AUDIO_I2S_DMA_Stream);
	    DMA_InitStructure.DMA_Channel = AUDIO_I2S_DMA_CHANNEL;
	    DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&(AUDIO_I2S->DR));
	    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	    DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(&nextBuffer->data);
	    DMA_InitStructure.DMA_BufferSize = nextBuffer->size;
	    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	    DMA_Init(AUDIO_I2S_DMA_Stream, &DMA_InitStructure);

	    SPI_I2S_DMACmd(AUDIO_I2S, SPI_DMAReq_Tx, ENABLE);
	    I2S_Cmd(AUDIO_I2S, ENABLE);
	    
	    DMA_ITConfig(AUDIO_I2S_DMA_Stream, DMA_IT_TC, ENABLE);
	    DMA_Cmd(AUDIO_I2S_DMA_Stream, ENABLE);
	    }
    else {
      // mute
	    //GPIO_ResetBits(AUDIO_I2S_SDMODE_GPIO, AUDIO_I2S_SDMODE_GPIO_PIN);

    }
  }
}




// Sound routines
void audioInit()
{
  i2sInit();
}

void audioEnd()
{
//  DAC->CR = 0 ;
	//AUDIO_I2S_TIMER->CR1 = 0;
  // Also need to turn off any possible interrupts
  //NVIC_DisableIRQ(AUDIO_I2S_TIM_IRQn);
	NVIC_DisableIRQ(AUDIO_I2S_DMA_Stream_IRQn);
}

//extern "C" void AUDIO_I2S_TIM_IRQHandler()
//{
//  DEBUG_INTERRUPT(INT_AUDIO);
//  DAC->CR &= ~DAC_CR_DMAEN1 ;     // Stop DMA requests
//#if defined(STM32F2)
//  DAC->CR &= ~DAC_CR_DMAUDRIE1 ;  // Stop underrun interrupt
//#endif
//  DAC->SR = DAC_SR_DMAUDR1 ;      // Write 1 to clear flag
//}

extern "C" void AUDIO_I2S_DMA_Stream_IRQHandler()
{
	// Wait for data to be sent
	while(!(AUDIO_I2S->SR & SPI_I2S_FLAG_TXE));
	
	AUDIO_I2S_DMA_Stream->CR &= ~DMA_SxCR_TCIE;             // Stop interrupt
	AUDIO_I2S_DMA->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;  // Write ones to clear flags
	AUDIO_I2S_DMA_Stream->CR &= ~DMA_SxCR_EN;                               // Disable DMA channel
	I2S_Cmd(AUDIO_I2S, DISABLE);

	if (nextBuffer) 
		audioQueue.buffersFifo.freeNextFilledBuffer();

	nextBuffer = audioQueue.buffersFifo.getNextFilledBuffer();
	if (nextBuffer)
	{
		I2S_Cmd(AUDIO_I2S, ENABLE);
		AUDIO_I2S_DMA_Stream->M0AR = CONVERT_PTR_UINT(&nextBuffer->data);
		AUDIO_I2S_DMA_Stream->NDTR = nextBuffer->size;
		AUDIO_I2S_DMA->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;    // Write ones to clear bits
		AUDIO_I2S_DMA_Stream->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE;          // Enable DMA channel
		DAC->SR = DAC_SR_DMAUDR1;                        // Write 1 to clear flag
	}
}
