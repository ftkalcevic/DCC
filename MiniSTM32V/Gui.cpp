#include "Gui.h"
#include "Src/GLCD.h"
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core

Gui gui;

extern "C" void guiTask_Start(void *argument)
{
	gui.Run();
}



void Gui::Run()
{
	
	LCD_Clear(Black);
	
	uint16_t tick=0;
	for(;;)
	{
		if (tick == 500)
		{
			static const char *spin = ".oOo";
			static uint8_t count = 0;
			
			uint8_t buf[2] = { spin[count % 4], 0 };
			GUI_TextXY(0,0, buf, White, Black);
			
			tick = 0;
			count++;
		}
		tick++;
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

