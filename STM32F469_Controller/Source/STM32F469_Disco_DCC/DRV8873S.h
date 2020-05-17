#pragma once

#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "cmsis_os.h"
#include "semphr.h"

class DRV8873S
{
	SPI_HandleTypeDef *hspi;
	bool Lock();
	void Unlock();
	SemaphoreHandle_t busySemaphoreHandle;
	StaticSemaphore_t busySemaphoreBuffer;

public:
	DRV8873S(SPI_HandleTypeDef *hspi) : hspi(hspi)
	{
		busySemaphoreHandle = xSemaphoreCreateMutexStatic(&busySemaphoreBuffer);
	}
	

	bool ReadFaultStatus(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin);
	bool ReadDiagStatus(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin);
	bool WriteIC1(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin);
	bool WriteIC2(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin);
	bool WriteIC3(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin);
	bool WriteIC4(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin);
};