#include "AppMain.h"
#include "main.h"
#include "DRV8873S.h"
#include "AudioTask.h"
#include "UIMessage.h"
#include <stdio.h>


UIMessage uimsg;
DRV8873S drv8873S(&hspi2);


extern "C" void AppMainTask_Entry(void *argument)
{
	AppMain app;
	app.Run();
}


enum EKey
{
	F1 = 1,
	F2 = 2,
	F3 = 3,
	F4 = 4,
	F5 = 5,
	FWD = 6,
	REV = 7,
	EStop = 8
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

static void ReadInputs()
{
	for (int i = 0; i < countof(Keys); i++)
	{
		KeyScan &k = Keys[i];
		
		bool keyDown = HAL_GPIO_ReadPin(k.port, k.pin) == GPIO_PIN_RESET;	// all keys are pull up
		if(k.debounce)
		{
			if (keyDown)
			{
				k.debounce--;
				if (k.debounce == 0)
				{
					printf("Key %d down\n", k.key);
					// Key down event.
					k.release = DEBOUNCE_TIMER;
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
					printf("Key %d up\n", k.key);
					// key release event
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
	
	static int count = 0;
	if (count % 1000 == 0)
	{
		printf("%d,%d\n", avg1, avg2);
	}
	count++;
}


//LED1_GPIO_Port LED1_Pin
//LED2_GPIO_Port LED2_Pin
//LED3_GPIO_Port LED3_Pin
//LED4_GPIO_Port LED4_Pin
  

void AppMain::Run()
{
	// Play startup sound
	audioTask.PlaySound(EAudioSounds::Bell2);
	
	InitInputs();
	
	for(;  ;)
	{
		ReadInputs();
		
		osDelay(pdMS_TO_TICKS(1));
	}
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
					
					
		- DCC Prog
		- LCC Config
		- About
		- Log/Diagnostics
		- More
			- app settings
			- configure dcc/lcc devices - that is define the objects in the list
			- dcc track programming
			- lcc config
			- switch to usb_msc mode
			- usb on the go
				- usb hid device input
		
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
	
*/