#include "dcc.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include <string.h>
#include "stm32f4xx_hal_tim_plus.h"


const int PREAMBLE_BITS = 14;
const int DCC_ONE_TIME = 58;	// usec
const int DCC_ZERO_TIME = 100;	// usec


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
				vTaskSuspend(xTaskGetCurrentTaskHandle());
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
		}
		
		// need to handle a 5ms gap before sending a message to the same decoder.
		// shortest message - 14 preamble + 3 bytes + 3 start bits + 1 end bit
		//                    14 * 58 + 3 * 8 * 58 + 3*100 + 1 * 58 = 2562us

	for(;  ;);
	}
}


void DCC::AddBit(int &index, bool n)
{
	bitBuffer[index + ARR_OFFSET] = 2*(n ? DCC_ONE_TIME : DCC_ZERO_TIME);
	bitBuffer[index + CCn_OFFSET] = n ? DCC_ONE_TIME : DCC_ZERO_TIME;
	index  += BURST_SIZE;
}

void DCC::SendDCCMessage(uint8_t *msg, uint8_t len)
{
	// Build a buffer of timings - a pair for bit off/bit on.
	// Use dma to stream the timings to the timer. (as used in opentx pxx)
	int bit = 0;
	for (int i = 0; i < PREAMBLE_BITS; i++)
		AddBit(bit, 1);
	AddBit(bit, 0);
	for (int i = 0; i < 8; i++)
		AddBit(bit, msg[i] & (1 << (7 - i)));
	AddBit(bit, 1);
	
	// Initialise timer, dma and interrupt.
	//HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
	HAL_TIM_DMABurst_WriteStart2(&htim8, TIM_DMABASE_DMAR, TIM_DMA_CC2, bitBuffer, bit, BURST_SIZE);
	__HAL_TIM_ENABLE(&htim8);
}