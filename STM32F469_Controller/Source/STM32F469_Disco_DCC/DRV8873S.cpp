#include "DRV8873S.h"


class MutexLock
{
	SemaphoreHandle_t handle;
	bool aquired;
	
public:
	MutexLock(SemaphoreHandle_t h, TickType_t xTicksToWait = 0)
	{
		handle = h;
		BaseType_t res = xSemaphoreTake(handle, xTicksToWait);
		if (res == pdPASS)
			aquired = true;
		else
			aquired = false;
	}
	~MutexLock()
	{
		if (aquired)
		{
			xSemaphoreGive(handle);
			aquired = false;
		}
	}
	bool Aquired() const
	{
		return aquired;
	}
};

enum MemoryMap: uint8_t
{
	FaultRegister = 0,
	DiagRegister = 1,
	IC1ControlRegister = 2,
	IC2ControlRegister = 3,
	IC3ControlRegister = 4,
	IC4ControlRegister = 5
};

const uint8_t MSG_READ = 0x40;
const uint8_t MSG_WRITE = 0;

bool DRV8873S::ReadFaultStatus(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin)
{
	MutexLock m(busySemaphoreHandle);
	if (m.Aquired())
	{
		HAL_StatusTypeDef res;
		
		uint16_t tx, rx;
		tx = (MSG_READ | (IC1ControlRegister << 1)) << 8;
		HAL_GPIO_WritePin(CS_GPIO, CS_Pin, GPIO_PIN_RESET);
		res = HAL_SPI_TransmitReceive(hspi, (uint8_t *)&tx, (uint8_t *)&rx, 1,5);
		HAL_GPIO_WritePin(CS_GPIO, CS_Pin, GPIO_PIN_SET);
		
		if ( res == HAL_OK )
			return true;
	}
	return false;
}

bool DRV8873S::ReadDiagStatus(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin)
{
	return false;
}

bool DRV8873S::WriteIC1(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin)
{
	return false;
}

bool DRV8873S::WriteIC2(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin)
{
	return false;
}

bool DRV8873S::WriteIC3(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin)
{
	return false;
}

bool DRV8873S::WriteIC4(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin)
{
	return false;
}
