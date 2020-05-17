#pragma once

#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "cmsis_os.h"
#include "semphr.h"

class AppMain
{
public:
	AppMain()
	{
	}
	
	void Run();
};


extern "C" void AppMain_Task(void *argument);
