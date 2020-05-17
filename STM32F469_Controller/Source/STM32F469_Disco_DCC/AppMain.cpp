#include "AppMain.h"
#include "main.h"
#include "DRV8873S.h"


extern "C" void AppMain_Task(void *argument)
{
	AppMain app;
	app.Run();
}


void AppMain::Run()
{
	HAL_GPIO_WritePin(Boost_Enable_GPIO_Port, Boost_Enable_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PrgTrk_Enable_GPIO_Port, PrgTrk_Enable_Pin, GPIO_PIN_SET); 

	for (;;)
	{
		HAL_GPIO_WritePin(DCC_Signal_GPIO_Port, DCC_Signal_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_SET);
		vTaskDelay(pdMS_TO_TICKS(500));
		HAL_GPIO_WritePin(DCC_Signal_GPIO_Port, DCC_Signal_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_RESET);
		vTaskDelay(pdMS_TO_TICKS(500));
	}



	vTaskDelay(pdMS_TO_TICKS(500));

	DRV8873S d(&hspi2);
//	d.ReadFaultStatus(Boost_CS_GPIO_Port, Boost_CS_Pin);
	d.ReadFaultStatus(PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin);

	
	for (;;)
		osDelay(pdMS_TO_TICKS(1000));
}