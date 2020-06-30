#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "semphr.h"
#include "DRV8873S.h"
#include "dcc.h"

extern "C" void DCCTask_Entry(void *argument);

namespace DCCType
{
	enum DCCType 
	{
		MainTrack = 0,
		ProgrammingTrack = 1
	};
};


namespace EPreamble
{
	enum EPreamble
	{
		Regular,
		Long
	};
};

const int DCC_ACK_CURRENT = 60;		// mA
const int DCC_ACK_PERIOD = 5;		// mS


template<DCCType::DCCType type, const int ARR_OFFSET, const int CCn_OFFSET, const int BURST_SIZE >
class DCCHal
{
	bool trackEnabled;
	bool eStop;
	uint32_t bitBuffer[BIT_BUFFER_SIZE*BURST_SIZE];
	volatile bool sent;

	GPIO_TypeDef *CS_Port;
	uint16_t CS_Pin;
	GPIO_TypeDef *Disable_Port;
	uint16_t Disable_Pin;
	GPIO_TypeDef *nFault_Port;
	uint16_t nFault_Pin;

protected:
	void UpdateEnabledStatus()
	{
		bool enabled = trackEnabled && !eStop;
		if (type == DCCType::MainTrack)
			ShowTrackPowerLED(enabled, 0);
		else
			ShowProgrammingTrackPowerLED(enabled, 0);
					
		if (enabled)
		{
			uint8_t status, diag;
			ReadHBridgeStatus(status, diag);
			if (status & FSR_FAULT)
			{
				ResetHBridgeFault();
			}
		}
		EnableHBridge(enabled);
	}
	
public:
	DCCHal( GPIO_TypeDef *CS_Port, uint16_t CS_Pin, 
		 GPIO_TypeDef *Disable_Port, uint16_t Disable_Pin, 
		 GPIO_TypeDef *Fault_Port, uint16_t Fault_Pin, 
		 GPIO_TypeDef *Signal_Port, uint16_t Signal_Pin )
	{
		this->CS_Port = CS_Port;
		this->CS_Pin = CS_Pin;
		this->Disable_Port = Disable_Port;
		this->Disable_Pin = Disable_Pin;		
		this->nFault_Port = Fault_Port;
		this->nFault_Pin = Fault_Pin;
		eStop = true;
		trackEnabled = false;
	}
	
	void InitialiseHBridge(int tripCurrent, int toff, int slewRate)
	{
		EnableHBridge(false);		// register changes only take effect when HBridge is off.
		
		drv8873S.WriteRegister(CS_Port, CS_Pin, MemoryMap::IC1ControlRegister, ((toff << IC1_TOFF_OFFS) & IC1_TOFF_MASK) | 
																			   IC1_SPIIN_FOLLOW_INx | 
																			   ((slewRate << IC1_SR_OFFS) & IC1_SR_MASK) | 
																			   IC1_MODE_PHEN );
		drv8873S.WriteRegister(CS_Port, CS_Pin, MemoryMap::IC2ControlRegister, IC2_ITRIP_REP_NFAULT_ENABLE | 
																			   IC2_OVERTEMP_LATCHED | 
																			   IC2_OTW_REP_NFAULT_DISABLE | 
																			   IC2_CPUV_ENABLED | 
																			   IC2_OCP_TRETRY_4_0MS | 
																			   IC2_OCPMODE_LATCHED_FAULT );
		//drv.WriteRegister(CS_Port, CS_Pin, MemoryMap::IC3ControlRegister, IC3_UNLOCK);
		drv8873S.WriteRegister(CS_Port, CS_Pin, MemoryMap::IC4ControlRegister, IC4_OLA_ENABLE | 
																			   ((tripCurrent << IC4_ITRIP_LVL_OFFS) & IC4_ITRIP_LVL_MASK) );
	}
	
	void ReadHBridgeStatus(uint8_t &status, uint8_t &diag )
	{
		drv8873S.ReadRegister(CS_Port, CS_Pin, MemoryMap::DiagRegister, status, diag);
	}

	void EnableHBridge(bool enable)
	{
		HAL_GPIO_WritePin(Disable_Port, Disable_Pin, enable ? GPIO_PIN_RESET : GPIO_PIN_SET);
	}
	
	void ResetHBridgeFault()
	{
		uint8_t status, ic3;
		drv8873S.ReadRegister(CS_Port, CS_Pin, MemoryMap::DiagRegister, status, ic3);
		drv8873S.WriteRegister(CS_Port, CS_Pin, MemoryMap::IC3ControlRegister, ic3 | IC3_CLEAR_FAULT );
	}

	uint16_t ReadCurrent()
	{
		uint16_t sum = 0;
		for (int i = (type == DCCType::MainTrack ? 0 : 1); i < countof(ADC_Current); i += 2)
			sum += ADC_Current[i];
		// 3v (3723) = 10 A.  (3v max is 330 ohm sense resistor)
		// I = 10 / 3723 * n *1000 (mA)		(n = sum/count)
		uint32_t current = 10;
		current *= sum;
		current *= 1000;
		current /= 3723;
		current /= countof(ADC_Current)/2;
		
		//current = sum / (countof(ADC_Current) / 2);
		return current;
	}

	void UpdateHBridgeStatus(DCCSettings::HBStatus &hbStatus )
	{
		uint8_t status, diag;
		ReadHBridgeStatus(status, diag);
				
		bool nFault = HAL_GPIO_ReadPin(nFault_Port, nFault_Pin) == GPIO_PIN_RESET;
		hbStatus.fault = (status & FSR_FAULT) != 0;
		hbStatus.overTemp = (status & FSR_TSD) != 0 ? DCCSettings::Fault : (status & FSR_OTW) != 0 ? DCCSettings::Warning : DCCSettings::Good;
		hbStatus.overCurrent = (status & FSR_OCP) != 0 ? DCCSettings::Fault : DCCSettings::Good;
		hbStatus.openLoad = (status & FSR_OLD) != 0;
		hbStatus.current = ReadCurrent();
	}

	void Initialise(int tripCurrent, int toff, int slewRate)
	{
		trackEnabled = false;
		
		// clear timer dma buffer.  Only those elements used will be set each time, others remain 0
		memset(bitBuffer, 0, sizeof(bitBuffer));
	
		InitialiseHBridge(tripCurrent, toff, slewRate);
		
		// Initialise timer and dma to be used.
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
			TIM8->PSC = (APB2_TIMER_CLOCK/1000000)-1; 	// TIM8 on APB2 (1Mhz)
			TIM8->DIER = TIM_DMA_UPDATE;
			TIM8->DCR = TIM_DMABase_ARR | ((BURST_SIZE - 1) << TIM_DCR_DBL_Pos);
			TIM8->CCER |= TIM_CCER_CC2P;
			TIM8->BDTR |= TIM_BDTR_MOE;
			TIM8->EGR |= TIM_EGR_UG;
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
			TIM3->PSC = (APB1_TIMER_CLOCK/1000000)-1; 	// TIM3 on APB1 (1MHz)
			TIM3->DIER = TIM_DMA_UPDATE;
			TIM3->DCR = TIM_DMABase_ARR | ((BURST_SIZE - 1) << TIM_DCR_DBL_Pos);
			TIM3->CCER |= TIM_CCER_CC1P;
			TIM3->EGR |= TIM_EGR_UG;
		}

	}

	void AddBit(int &index, bool n)
	{
		bitBuffer[index + ARR_OFFSET] = 2*(n ? DCC_ONE_TIME : DCC_ZERO_TIME);
		bitBuffer[index + CCn_OFFSET] = (n ? DCC_ONE_TIME : DCC_ZERO_TIME);
		index  += BURST_SIZE;
	}

	void SendDCCMessage(uint8_t *msg, uint8_t len, EPreamble::EPreamble const preamble = EPreamble::Regular)
	{
		sent = false;
		// Build a buffer of timings - a pair for bit off/bit on.
		// Use dma to stream the timings to the timer. (as used in opentx pxx)
		int index = 0;
		for (int i = 0; i < (preamble == EPreamble::Regular ? PREAMBLE_BITS : LONG_PREAMBLE_BITS); i++)
			AddBit(index, 1);
		for (int m = 0; m < len; m++)
		{
			AddBit(index, 0);
			for (int i = 0; i < 8; i++)
				AddBit(index, msg[m] & (1 << (7 - i)));
		}
		AddBit(index, 1);
		AddBit(index, 1);	// without the send '1' the line stays low, not indicating the end of the previous bit
		AddBit(index, 1);	// without the send '1' the line stays low, not indicating the end of the previous bit
//		for ( int i = 0; i < BURST_SIZE;i++)
//			bitBuffer[index++] = 0;	// add terminator.  Easier to do this than also process the tim update interrupt after dma is finished.

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
	

	void SetErrorByte(uint8_t *msg, uint8_t len)
	{
		uint8_t e = *msg++;
		for (int i = 1; i < len - 1; i++)
			e ^= *msg++;
		*msg = e;
	}

	void SendIdle(EPreamble::EPreamble preamble = EPreamble::Regular)
	{
		// Send Idle
		static uint8_t idleMessage[3] = { 0xFF, 0, 0xFF };
		SendDCCMessage(idleMessage, countof(idleMessage), preamble);
	}

	
	void SendReset(EPreamble::EPreamble preamble = EPreamble::Regular)
	{
		// Send Idle
		static uint8_t resetMessage[3] = { 0, 0, 0 };
		SendDCCMessage(resetMessage, countof(resetMessage), preamble);
	}

	
	void EStop(bool stop)
	{
		eStop = stop;
		UpdateEnabledStatus();
	}
	
	bool isEStop() const { return eStop; }
	
	void Enable(bool enable)
	{
		trackEnabled = enable;
		UpdateEnabledStatus();
	}
	
	bool isEnabled() const { return trackEnabled; }
	
	// IRQ callback
	virtual void DCCSent() 
	{
		sent = true;
	}
};
