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

static void SetMultiFunctionAddress(uint8_t * &ptr, uint16_t address)
{
    if ( address <= 127 )
    {
        *(ptr++) = address;
    }
    else
    {
	    address &= 0x3FFF;	// 14bit
        *(ptr++) = ((address >> 8) & 0xFF) | ADDR_EXTENDED;
        *(ptr++) = address & 0xFF;
    }
}

static uint8_t MakeSpeed(uint16_t speed)	// speed is always 0-128
{
//	if (speed_type == 14)
//	{
//		return speed;
//	}
//	else if (speed_type == 28)
	{
		if (speed == 0)
			return (uint8_t)speed;
		else
		{
			speed = speed * 28 / 128;
			if (speed > 27)
				speed = 27;
			speed += 4;
			// LSB of bit 4(C) - C S3 S2 S1 S0)
			if (speed & 1)
				speed |= (1 << 5);
			speed >>= 1;
			return (uint8_t)(speed & 0b00011111);
		}
	}
//	else if (speed_type == 128)
//	{
//	}
		
}


void MainTrack_DCC_Stop(uint16_t address, bool estop)
{
	DCCMessage msg;
	msg.address = address;
	uint8_t *ptr = msg.msg;
	
	SetMultiFunctionAddress(ptr, address);

	// todo - bit 4 can be used to control the headlight (F1?)
	*ptr++ = INS_SPEED_AND_DIRECTION | (estop ? DATA_SD_ESTOP : DATA_SD_STOP);
	msg.msgLen = ptr - msg.msg;
	
	dcc.SendMessage(msg);
}


void MainTrack_DCC_SetSpeedAndDirection(uint16_t address, EDirection::EDirection direction, uint8_t throttle, uint8_t brake)
{
	DCCMessage msg;
	msg.address = address;
	uint8_t *ptr = msg.msg;
	
	SetMultiFunctionAddress(ptr, address);

	// todo - bit 4 can be used to control the headlight (F1?)
	// Speed can be 14 (+headlight), 28(-headlight) or 128 step
	uint8_t speedByte = direction == EDirection::Stopped ? 0 : MakeSpeed(throttle);
	*ptr++ = INS_SPEED_AND_DIRECTION | (direction == EDirection::Forward ? DATA_SD_FORWARD : DATA_SD_REVERSE) | speedByte;
	msg.msgLen = ptr - msg.msg;
	
	dcc.SendMessage(msg);
}
