#include "dcc.h"


#define MAINTRK_ARR_OFFSET 0
#define MAINTRK_CCn_OFFSET 3
#define MAINTRK_BURST_SIZE 4

#define PRGTRK_ARR_OFFSET 0
#define PRGTRK_CCn_OFFSET 2
#define PRGTRK_BURST_SIZE 3

#define QUEUE_LEN	10

DCC<DCCType::MainTrack, MAINTRK_ARR_OFFSET, MAINTRK_CCn_OFFSET, MAINTRK_BURST_SIZE, QUEUE_LEN> mainTrack(  
		Boost_CS_GPIO_Port, Boost_CS_Pin,
		Boost_Disable_GPIO_Port, Boost_Disable_Pin,
		Boost_Fault_GPIO_Port, Boost_Fault_Pin,
		&hadc1, 1,
		DCC_Signal_GPIO_Port, DCC_Signal_Pin );					// tim8_2
DCC<DCCType::ProgrammingTrack, PRGTRK_ARR_OFFSET, PRGTRK_CCn_OFFSET, PRGTRK_BURST_SIZE, QUEUE_LEN> programmingTrack(   
		PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin,
		PrgTrk_Disable_GPIO_Port, PrgTrk_Disable_Pin,
		PrgTrk_Fault_GPIO_Port, PrgTrk_Fault_Pin,
		PrgTrk_ACK_GPIO_Port, PrgTrk_ACK_Pin,
		PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin );	// tim3_1 

extern "C"
{
	void DCCTask_Entry(void *argument)
	{
		DCCType type = static_cast<DCCType>(reinterpret_cast<int>(argument));
for(;;) vTaskDelay(pdMS_TO_TICKS(1000));
		switch (type)
		{
			case DCCType::ProgrammingTrack:
				programmingTrack.Initialise();
				programmingTrack.Run();
				break;
			case DCCType::MainTrack:
				mainTrack.Initialise();
				mainTrack.Run();
				break;
		}
		
		
		for (;;)
		{
			
			osDelay(1);
		}
	}
}



extern"C"
{
	
void MainTrack_DCC_Handle_DMA_IRQ()
{
	// DMA transfer completed (but Timer still going)
	DMA2->LIFCR |= DMA_LISR_TCIF1;
	DMA2_Stream1->CR &= ~DMA_SxCR_EN;
	mainTrack.DCCSent();
}
	
void ProgrammingTrack_DCC_Handle_DMA_IRQ()
{
	// DMA transfer completed (but Timer still going)
	DMA1->LIFCR |= DMA_LISR_TCIF2;
	DMA1_Stream2->CR &= ~DMA_SxCR_EN;
	programmingTrack.DCCSent();
}
	
}

