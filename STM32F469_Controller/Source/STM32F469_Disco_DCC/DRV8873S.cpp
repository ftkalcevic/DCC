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

const uint8_t MSG_READ = 0x40;
const uint8_t MSG_WRITE = 0;

bool DRV8873S::DoRXTX(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, uint16_t tx, uint16_t &rx)
{
	MutexLock m(busySemaphoreHandle);
	if (m.Aquired())
	{
		HAL_StatusTypeDef res;
		
		HAL_GPIO_WritePin(CS_GPIO, CS_Pin, GPIO_PIN_RESET);
		res = HAL_SPI_TransmitReceive(hspi, (uint8_t *)&tx, (uint8_t *)&rx, 1,5);
		HAL_GPIO_WritePin(CS_GPIO, CS_Pin, GPIO_PIN_SET);
		
		if ( res == HAL_OK )
			return true;
	}
	return false;
}

bool DRV8873S::Read(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, MemoryMap reg, uint16_t &rx)
{
	return DoRXTX(CS_GPIO, CS_Pin, (MSG_READ | (reg << 1)) << 8, rx);
}

bool DRV8873S::Write(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, MemoryMap reg, uint8_t data )
{
	uint16_t tx;
	return DoRXTX(CS_GPIO, CS_Pin, ((MSG_WRITE | (reg << 1)) << 8) | data, tx);
}

bool DRV8873S::ReadRegister(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, MemoryMap reg, uint8_t &status, uint8_t &data)
{
	uint16_t rx;
	if (Read(CS_GPIO, CS_Pin, reg, rx))
	{
		status = rx >> 8;
		data = rx & 0xFF;
		return true;
	}
	return false;
}

bool DRV8873S::WriteRegister(GPIO_TypeDef* CS_GPIO, uint16_t CS_Pin, MemoryMap reg, uint8_t data)
{
	uint16_t rx;
	return Write(CS_GPIO, CS_Pin, reg, data);
}
