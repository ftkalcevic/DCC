#include "AppMain.h"
#include "main.h"
#include "DRV8873S.h"
#include "AudioTask.h"
#include "UIMessage.h"
#include <stdio.h>
#include "ProgTrackDCC.h"
#include "MainTrackDCC.h"


UIMessage uimsg;
DRV8873S drv8873S(&hspi2);
AppMain app;


extern "C" void AppMainTask_Entry(void *argument)
{
	app.Run();
}


enum EKey
{
	None = 0,
	F1 = 1,
	F2 = 2,
	F3 = 3,
	F4 = 4,
	F5 = 5,
	FWD = 6,
	REV = 7,
	EStop = 8,
	KeyRelease = 0x1000
};
static struct KeyScan
{
	GPIO_TypeDef *port;
	uint16_t pin;
	EKey key;
	int debounce;
	int release;
	
} Keys[] = { 
	{ Fn1_GPIO_Port,   Fn1_Pin,   F1 },	
	{ Fn2_GPIO_Port,   Fn2_Pin,   F2 },	
	{ Fn3_GPIO_Port,   Fn3_Pin,   F3 },	
	{ Fn4_GPIO_Port,   Fn4_Pin,   F4 },
	{ Fn5_GPIO_Port,   Fn5_Pin,   F5 },
	{ FWD_GPIO_Port,   FWD_Pin,   FWD },
	{ REV_GPIO_Port,   REV_Pin,   REV },
	{ EStop_GPIO_Port, EStop_Pin, EStop }
};

const int DEBOUNCE_TIMER = 25;	//ms
static void InitInputs()
{
	for (int i = 0; i < countof(Keys); i++)
		Keys[i].debounce = Keys[i].release = 0;
}

static EKey ReadInputs(int &i)
{
	while (i < countof(Keys))
	{
		KeyScan &k = Keys[i];
		i++;
		
		bool keyDown = HAL_GPIO_ReadPin(k.port, k.pin) == GPIO_PIN_RESET;	// all keys are pull up
		if(k.debounce)
		{
			if (keyDown)
			{
				k.debounce--;
				if (k.debounce == 0)
				{
					// Key down event.
					k.release = DEBOUNCE_TIMER;
					return k.key;
				}
			}
			else
				k.debounce = 0;
		}
		else if (k.release)
		{
			if (!keyDown)
			{
				k.release--;
				if (k.release == 0)
				{
					// key release event
					return (EKey)(k.key + KeyRelease);
				}
			}
			else
				k.release = DEBOUNCE_TIMER;
				
		}
		else
		{
			if (keyDown)
				k.debounce = DEBOUNCE_TIMER;
		}
	}

	uint16_t sum1 = 0, sum2 = 0;
	for (int i = 0; i < countof(ADC_Joysticks); i += 2)
	{
		sum1 += ADC_Joysticks[i + 0];
		sum2 += ADC_Joysticks[i + 1];
	}
	uint16_t avg1 = sum1 / (countof(ADC_Joysticks) / 2);
	uint16_t avg2 = sum2 / (countof(ADC_Joysticks) / 2);
	
	return None;
}

static struct LEDDef
{
	GPIO_TypeDef *port;
	uint16_t pin;
	int counter;
	
} LEDs[] = { 
	{ LED1_GPIO_Port, LED1_Pin,   0 },	// Green
	{ LED2_GPIO_Port, LED2_Pin,   0 },	// Orange
	{ LED3_GPIO_Port, LED3_Pin,   0 },	// Red
	{ LED4_GPIO_Port, LED4_Pin,   0 }	// Blue
};

static void UpdateLEDs()
{
	for (int i = 0; i < countof(LEDs); i++)
	{
		LEDDef &led = LEDs[i];
		if (led.counter > 0)
		{
			led.counter--;
			if ( led.counter == 0 )
				HAL_GPIO_WritePin(led.port, led.pin, GPIO_PIN_SET);	
		}
	}
}
  
void AppMain::ShowLED(int id, bool enable, int duration)
{
	assert(id >= 0 && id < countof(LEDs) && "LED id is out of range");
	LEDDef &led = LEDs[id];
	if (enable)
	{
		HAL_GPIO_WritePin(led.port, led.pin, GPIO_PIN_RESET);	
		led.counter = duration;
	}
	else
	{
		HAL_GPIO_WritePin(led.port, led.pin, GPIO_PIN_SET);	
		led.counter = 0;
	}
}

	extern "C" void CheckTaskStacks(void);

void AppMain::ToggleEStop()
{
	eStop = !eStop;

	MainTrack_DCC_EStop(eStop);
	ProgrammingTrack_DCC_EStop(eStop);
	
	CheckTaskStacks();
	

}

void AppMain::Run()
{
	CheckTaskStacks();
	// Play startup sound
	audioTask.PlaySound(EAudioSounds::Bell2);
	
	InitInputs();
	
	for(;  ;)
	{
		UpdateLEDs();
		
		int i = 0;
		EKey key;
		while ((key = ReadInputs(i)) != None)
		{
			if (key == EKey::EStop)
				ToggleEStop();
		}
		
		osDelay(pdMS_TO_TICKS(1));
	}
}


extern "C" {
	void ShowDiskLED(int enable, int duration) {app.ShowDiskLED(enable, duration);}
	void ShowTrackPowerLED(int enable, int duration) {app.ShowTrackPowerLED(enable, duration);}
	void ShowErrorPowerLED(int enable, int duration) {app.ShowErrorLED(enable, duration);}
	void ShowProgrammingTrackPowerLED(int enable, int duration) {app.ShowProgrammingTrackPowerLED(enable, duration);}
}


/*
	- App will always start in the "drive" screen.
		- swipe to next loco/accessory
	- Go back to the main screen
		- 4x2 buttons per page
			- can swipe for more
			- one button will be the config menu (default to last button)
			- the others will be to select a trains, or accessories
			- buttons will be image + text.  user configurable image/text for train/acc
			- buttons can be ordered.
		- buttons will zoom to full screen config (flip over and zoom)
	- config
		- Settings
			- Audio volume
			- key ticks
			- startup tune (presence of file)
			- startup screen (presence of file) or default
		- DCC Settings
			- Trip current
			- TOFF 
			- Slew Rate
			- Display statuses
				- Both H-Bridges
				- Fault Reg
					- Fault
					- over temp warning
					- under voltage fault
					- over current
					- over temp fault
					- open load detected
				- Diag Reg
					- open load 1
					- OL2
					- trip 1
					- itrip2
					- over current fault high 1
					- ocp_l1
					- ocp_h2
					- ocp_l2
					
					
		- DCC Prog/Config
			- for a new device
				- Select programming track.
				- Scan for device.
					- find address, manufacturer, model, type, CV29
						- this can be either a new decoder, or an existing one.
					- How do we do this?
						- A separate ProgrammingTrack task?  That uses the dcc_programming_track task?  Seems like an uneccessary task.  The DCCProg task doesn't have a loop like the main DCC task.
						- So do we separate the DCC HAL from the task?  DCC task will have the loop and decoder table.  DCC Prog will have a state machine to send/ack, multiple requests.
						- When deciding prog or main track, same CV program request, just no ack from main track.  Do we block or async (queue) results - queue obviously.
					- do we access dcc class directly or go through App?  Already go through model - view -> presenter -> model -> app -> dcc?
						- DCCHal - timer + DMA + message building + send + send complete
						- DCC loops through the decoder table
							- sends status every 1sec.
						- PrgTrk, only needs to process requests (plus status)
							- requests via queue.  block, but timeout when status time (queueReceive(msg, timeTillNextStatusUpdate).
				- Set the address plus CV29 (2 or 4 byte address).
			- otherwise, 
				- standard programming
				- can set anything but the address (can, but should we allow it)
				- on the programming track we get ACK
					- so we can send requests via broadcast
					- we get acks when we write.
					- we can also read CVs, bit bye bit.
			- also customisation
				- change the icon
				- change the name
				- change the tune?  For accessories, the change noise - switch, turntable, 
				- button assignments (Fn1-5, also, HID devices - this may need a separate screen).
					- do the fn keys have to be assigned to the active decoder?  We could control a train, F1 lights on/of.  F2-5 could be to toggle switches.
					
		- LCC Config
		- About
		- Log/Diagnostics
		- More
			- calibrate touch screen
			- app settings
			- configure dcc/lcc devices - that is define the objects in the list
			- dcc track programming
			- lcc config
			- switch to usb_msc mode
			- usb on the go
				- usb hid device input
				- configure pot range
		
	- dcc programming 
		- track 
			- search to find existing 
			- read CVs
			- define each loco/accessory in a config file. (like lcc)
		- global (terminology)
			- send commands without ACK (can't query CVs, values, etc)
	- lcc
		- search
		- map events.
	- Do we want to show a map?
		- with all accessories, so we can select, or zoom then select?
		
TODO
	- Add selection changed action to scrollwheel.
	- Create uncover transition.
	
*/