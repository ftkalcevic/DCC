#include "AppMain.h"
#include "main.h"
#include "DRV8873S.h"
#include "AudioTask.h"
#include "UIMessage.h"
#include <stdio.h>
#include "ProgTrackDCC.h"
#include "MainTrackDCC.h"
#include "Config.h"
#include "DecodersConfig.h"
#include "DecoderDefConfig.h"
#include "ImageFile.h"

UIMessage uimsg;
DRV8873S drv8873S(&hspi2);
AppMain app;
UIConfig uiConfig;
DecodersConfig uiDecodersConfig;;
DecoderDefConfig decoderDefinitions;
ImageFiles imageFiles;


extern "C" void AppMainTask_Entry(void *argument)
{
	app.Run();
}


static struct KeyScan
{
	GPIO_TypeDef *port;
	uint16_t pin;
	EKey::EKey key;
	int debounce;
	int release;
	bool down() const { return release != 0; }
} Keys[] = { 
	{ Fn1_GPIO_Port,   Fn1_Pin,   EKey::F1 },	
	{ Fn2_GPIO_Port,   Fn2_Pin,   EKey::F2 },	
	{ Fn3_GPIO_Port,   Fn3_Pin,   EKey::F3 },	
	{ Fn4_GPIO_Port,   Fn4_Pin,   EKey::F4 },
	{ Fn5_GPIO_Port,   Fn5_Pin,   EKey::F5 },
	{ FWD_GPIO_Port,   FWD_Pin,   EKey::FWD },
	{ REV_GPIO_Port,   REV_Pin,   EKey::REV },
	{ EStop_GPIO_Port, EStop_Pin, EKey::EStop }
};

uint16_t throttle, brake;

uint16_t throttleRaw, brakeRaw;
KeyScan *fwdKey, *revKey;

const int DEBOUNCE_TIMER = 25;	//ms

static void InitInputs()
{
	for (int i = 0; i < countof(Keys); i++)
		Keys[i].debounce = Keys[i].release = 0;
	uiConfig.parse();
	for (int i = 0; i < countof(Keys); i++)
		if (Keys[i].key == EKey::FWD)
			fwdKey = Keys + i;
		else if (Keys[i].key == EKey::REV)
			revKey = Keys + i;
}

static int MapAnalog(int raw, int min, int max, bool reverse, int outMin, int outMax)
{
	int out = (outMax - outMin) * (raw - min) / (max - min) + outMin;
	if (out < outMin)
		out = outMin;
	else if (out > outMax)
		out = outMax;

	if (reverse)
		out = outMax - out;
	return out;
}

static int MapAnalogToFixed(int raw, int min, int max, bool reverse, int outMin, int outMax)
{
	const int SCALE = 16;
	int n = MapAnalog(raw, min, max, reverse, outMin * SCALE, (outMax+1) * SCALE);
	int out = (n - SCALE / 2) / SCALE;
	return out;
}

static EKey::EKey ReadInputs(int &i)
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
					return (EKey::EKey)(k.key + EKey::KeyRelease);
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
	brakeRaw = sum1 / (countof(ADC_Joysticks) / 2);
	throttleRaw = sum2 / (countof(ADC_Joysticks) / 2);
	
	return EKey::None;
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
	printHeapStatistics();
}

void AppMain::YieldControl(Decoders &d)
{
	if (d.getLoco().getControlled())
	{
		d.getLoco().setControlled(false);
		// Send something to mainTrackDCC
		MainTrack_DCC_Stop(d.getAddress());
	}
}

void AppMain::TakeControl(int decoderIndex, bool control)
{
	if (decoderIndex >= 0 && decoderIndex < uiDecodersConfig.Count() )
	{
		Decoders &d = uiDecodersConfig[decoderIndex];
		if (d.getType() == EDecoderType::Multifunction )
		{
			if (!control)
			{
				YieldControl(d);
				uiDecodersConfig.setActiveDecoder(-1);
			}
			else
			{
				// If control only one, release others
				// TODO - if we support multiple active, then we need to know when a loco has focus on the UI so we can route speed/dir commands to it.
				//	      or do we have a button to grab it.
				for (int i = 0; i < uiDecodersConfig.Count(); i++)
					if (i!=decoderIndex && uiDecodersConfig[i].getType() == EDecoderType::Multifunction)
					{
						YieldControl(uiDecodersConfig[i]);
						uiDecodersConfig.setActiveDecoder(-1);
					}

				d.getLoco().setControlled(control);
				// Mark this as the active 
				uiDecodersConfig.setActiveDecoder(decoderIndex);
				// Send something to mainTrackDCC (happens every 50ms)
				// update the function keys
			}
		}
	}
	
}


void AppMain::Run()
{
	setupLocale();
	InitMallocChecking();
	CheckTaskStacks();
	printHeapStatistics();
	// Play startup sound
	//audioTask.Mute(true);
	
	InitInputs();
	uiDecodersConfig.parse();
	decoderDefinitions.init();
//	printHeapStatistics();
//	decoderDefinitions.loadDecoderDef("DZ143");
//	DecoderDef &def = decoderDefinitions.getDecoderDef(); 
//	printHeapStatistics();
//	decoderDefinitions.clear();
//	printHeapStatistics();

	imageFiles.Init();
	
	uint16_t ms_counter;
	for(;  ;)
	{
		ms_counter++;
		if (ms_counter >= 1000)
			ms_counter = 0;
		
		UpdateLEDs();
		
		int i = 0;
		EKey::EKey key;
		while ((key = ReadInputs(i)) != EKey::None)
		{
			if (key == EKey::EStop)	// Should we process this here, or should we listen for our own events?
				ToggleEStop();

			UIMsg msg;
			msg.type = EUIMessageType::KeyEvent;
			msg.keys.keyEvent = key;
			uimsg.Send(msg);
		}
		
		// Update joysticks periodically
		if(ms_counter % 50 == 0)
		{
			// compute value 0-1023
			UIMsg msg;
			msg.type = EUIMessageType::InputEvent;
			
			msg.input.throttleRaw = throttleRaw;
			msg.input.throttle = MapAnalog(throttleRaw, uiConfig.getThrottleMin(), uiConfig.getThrottleMax(), uiConfig.getThrottleReverse(), 0, 1024);
			msg.input.brakeRaw = brakeRaw;
			msg.input.brake = MapAnalog(brakeRaw, uiConfig.getBrakeMin(), uiConfig.getBrakeMax(), uiConfig.getBrakeReverse(), 0, 100 );
			if ( uiConfig.getDirectionReverse())
				msg.input.direction = revKey->down() ? EDirection::Forward : fwdKey->down() ? EDirection::Reverse : EDirection::Stopped;
			else
				msg.input.direction = fwdKey->down() ? EDirection::Forward : revKey->down() ? EDirection::Reverse : EDirection::Stopped;
			
			uimsg.Send(msg);
			if ( uiDecodersConfig.getActiveDecoder() >= 0 && 
				 uiDecodersConfig[uiDecodersConfig.getActiveDecoder()].getType() == EDecoderType::Multifunction && uiDecodersConfig[uiDecodersConfig.getActiveDecoder()].getLoco().getControlled() )
			{
				MainTrack_DCC_SetSpeedAndDirection(uiDecodersConfig[uiDecodersConfig.getActiveDecoder()].getAddress(), uiDecodersConfig[uiDecodersConfig.getActiveDecoder()].getLoco().getSpeedSteps(), msg.input.direction, msg.input.throttle, msg.input.brake);
			}
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
		- Inputs
			- usb on the go
				- usb hid device input
			- configure pot range 
		- More
			- calibrate touch screen
			- app settings
			- configure dcc/lcc devices - that is define the objects in the list
			- dcc track programming
			- lcc config
			- switch to usb_msc mode
		
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
	- Add disabled to checkbox

DCCConfigView
	- Make clickable sections bigger (Address, Name, etc)
	- Select a comerical decoder
		- Make this bigger
	- Read All Cvs
		- And do something with them
		- store them in the custom decoder bit
	- Edit cvs
		- from "decoder" definition (or "Unknown")
		- uint8_t, bits, specials (eg speed chart)
		- move to separate screens
			- 3 buttons 
				- CVS - numeric order eg "[01] Address"
					- swipe container - many pages.
				- grouped
					- swipe container - many pages.
				- raw - single panel - enter #, read, edit, write.
					  - maybe list all CVS [01]=255, [02]=27, etc
	- More config
		- don't change icon on DCCConfig screen, will either be loco or accessory
		- select small icon for MainView
		- select small user image for Mainview
		- select large icon for decoders view
		- select large user image for decoders view
			- decoders view - remove all widgets except take control
		- Accessory decoder...
			- large image for each states
				- mirror (flip) left/right
				- miroro top/bottom
		- other options
			- Address - 2/4 digit is automatic
			- Decoder type is read from CV29 bit 7
			- Speed steps are defined by CV29 bit ?
				- 0 - 14 speed steps
				- 1 - 28 speed steps (or 128 using the other command).
					- how do we pick 28/128?
			- front light position
			- release action - when yielding, stop, estop, or nothing.
			
			
			
	- How to handle 4 digit addresses?
		- Do we just let them enter a number and we work it out?
		- Or, do we make them change the configuration and define limits?
		
preferences
	- sound preferences
	- sound mute box
	- beep volume
	
decoders view 
	- remove all widgets except take control	
	- one button for Take control/yield (larger button)

General
	- re-read spec.  Find and reference key design points
	
	
Next
	- move CVs to separate screens
	- add graphics
*/