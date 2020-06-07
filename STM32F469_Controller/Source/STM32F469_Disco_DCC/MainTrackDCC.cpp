#include "dcc.h"
#include "config.h"
#include "MainTrackDCC.h"


static MainTrackDCC dcc( Boost_CS_GPIO_Port, Boost_CS_Pin,
				         Boost_Disable_GPIO_Port, Boost_Disable_Pin,
				         Boost_Fault_GPIO_Port, Boost_Fault_Pin,
				         DCC_Signal_GPIO_Port, DCC_Signal_Pin );

extern "C"
{
	static void ReadSettings(int &tripCurrent, int &toff, int &slewRate)
	{
		DCCConfig config;
		config.parse();
		
		tripCurrent = config.getTripCurrent();
		toff = config.getToff();
		slewRate = config.getSlewRate();
	}
	
	void MainTrkDCCTask_Entry(void *argument)
	{
		int tripCurrent, toff, slewRate;
		ReadSettings(tripCurrent, toff, slewRate);
		
		dcc.Initialise(tripCurrent, toff, slewRate);
		dcc.Run(true);
	}
}


extern"C"
{
	void MainTrackDCC_Handle_DMA_IRQ()
	{
		// DMA transfer completed (but Timer still going)
		DMA2->LIFCR |= DMA_LISR_TCIF1;
		DMA2_Stream1->CR &= ~DMA_SxCR_EN;
		dcc.DCCSent();
	}
}

void MainTrack_DCC_EStop(bool stop)
{
	dcc.EStop(stop);
}
