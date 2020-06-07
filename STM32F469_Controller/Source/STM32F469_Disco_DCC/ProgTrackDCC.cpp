#include "dcc.h"
#include "config.h"
#include "ProgTrackDCC.h"

static ProgTrackDCC dcc ( PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin,
						  PrgTrk_Disable_GPIO_Port, PrgTrk_Disable_Pin,
						  PrgTrk_Fault_GPIO_Port, PrgTrk_Fault_Pin,
						  PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin );

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
	
	void ProgTrkDCCTask_Entry(void *argument)
	{
		int tripCurrent, toff, slewRate;
		ReadSettings(tripCurrent, toff, slewRate);
		
		dcc.Initialise(tripCurrent, toff, slewRate);
		dcc.Run(false);
	}
}


extern"C"
{
	
	void ProgTrackDCC_Handle_DMA_IRQ()
	{
		// DMA transfer completed (but Timer still going)
		DMA1->LIFCR |= DMA_LISR_TCIF2;
		DMA1_Stream2->CR &= ~DMA_SxCR_EN;
		dcc.DCCSent();
	}
	
}

void ProgrammingTrack_DCC_EStop(bool stop)
{
	dcc.EStop(stop);
}

void ProgrammingTrack_DCC_Enable(bool enable)
{
	dcc.Enable(enable);
}

void ProgrammingTrack_DCC_ScanProgrammingTrack()
{
}