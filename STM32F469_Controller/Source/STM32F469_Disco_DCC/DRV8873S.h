#pragma once

#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "cmsis_os.h"
#include "semphr.h"

enum MemoryMap: uint8_t
{
	FaultRegister = 0,
	DiagRegister = 1,
	IC1ControlRegister = 2,
	IC2ControlRegister = 3,
	IC3ControlRegister = 4,
	IC4ControlRegister = 5
};

#define IC1_TOFF_20us			(0<<6)
#define IC1_TOFF_40us			(1<<6)
#define IC1_TOFF_60us			(2<<6)
#define IC1_TOFF_80us			(3<<6)

#define IC1_SPIIN_FOLLOW_INx	(0<<5)
#define IC1_SPIIN_FOLLOW_REG	(1<<5)

#define IC1_SR_53_2				(0<<2)
#define IC1_SR_34_0				(1<<2)
#define IC1_SR_18_3				(2<<2)
#define IC1_SR_13_0				(3<<2)
#define IC1_SR_10_8				(4<<2)
#define IC1_SR_7_9				(5<<2)
#define IC1_SR_5_3				(6<<2)
#define IC1_SR_2_6 				(7<<2)

#define IC1_MODE_PHEN 			(0)
#define IC1_MODE_PWM 			(1)
#define IC1_MODE_INDEP 			(2)
#define IC1_MODE_DISABLED		(3)


class DRV8873S
{
	SPI_HandleTypeDef *hspi;
	bool Lock();
	void Unlock();
	SemaphoreHandle_t busySemaphoreHandle;
	StaticSemaphore_t busySemaphoreBuffer;

	bool DoRXTX(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, uint16_t tx, uint16_t &rx);
	bool Read(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, MemoryMap reg, uint16_t &rx);
	bool Write(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, MemoryMap reg, uint8_t data);
	
public:
	DRV8873S(SPI_HandleTypeDef *hspi) : hspi(hspi)
	{
		busySemaphoreHandle = xSemaphoreCreateMutexStatic(&busySemaphoreBuffer);
	}
	
	bool ReadRegister(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, MemoryMap reg, uint8_t &status, uint8_t &data);
	bool WriteRegister(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, MemoryMap reg, uint8_t data);
};