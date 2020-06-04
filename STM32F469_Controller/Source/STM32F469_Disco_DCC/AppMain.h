#pragma once

#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "cmsis_os.h"
#include "semphr.h"

#ifdef __cplusplus
class AppMain
{
public:
	AppMain() :
		eStop(true)
	{
	}
	
	void Run();
	
	void ShowDiskLED(bool enable, int duration) { ShowLED(0, enable, duration);}
	void ShowTrackPowerLED(bool enable, int duration) { ShowLED(1, enable, duration);}
	void ShowErrorLED(bool enable, int duration) { ShowLED(2, enable, duration);}
	void ShowProgrammingTrackPowerLED(bool enable, int duration) { ShowLED(3, enable, duration);}
	
protected:
	void ShowLED(int id, bool enable, int duration);
	void ToggleEStop();
	
	bool eStop;
};
#endif


#ifdef __cplusplus
extern "C" {
#endif
void AppMainTask_Entry(void *argument);
void ShowDiskLED(int enable, int duration);
void ShowTrackPowerLED(int enable, int duration);
void ShowErrorLED(int enable, int duration);
void ShowProgrammingTrackPowerLED(int enable, int duration);
	
#ifdef __cplusplus
};
#endif
