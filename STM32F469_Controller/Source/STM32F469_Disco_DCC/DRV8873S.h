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

#define IC2_ITRIP_REP_NFAULT_DISABLE	(0<<7)
#define IC2_ITRIP_REP_NFAULT_ENABLE		(1<<7)

#define IC2_OVERTEMP_LATCHED			(0<<6)
#define IC2_OVERTEMP_AUTO				(1<<6)

#define IC2_OTW_REP_NFAULT_DISABLE		(0<<5)
#define IC2_OTW_REP_NFAULT_ENABLE		(1<<5)

#define IC2_CPUV_ENABLED				(0<<4)
#define IC2_CPUV_DISABLED				(1<<4)

#define IC2_OCP_TRETRY_0_5MS			(0<<2)
#define IC2_OCP_TRETRY_1_0MS			(1<<2)
#define IC2_OCP_TRETRY_2_0MS			(2<<2)
#define IC2_OCP_TRETRY_4_0MS			(3<<2)

#define IC2_OCPMODE_LATCHED_FAULT		(0<<0)
#define IC2_OCPMODE_AUTO_RETRY_FAULT	(1<<0)
#define IC2_OCPMODE_REPORT_ONLY			(2<<0)
#define IC2_OCPMODE_IGNORED				(3<<0)

#define IC3_CLEAR_FAULT					(1<<7)

#define IC3_LOCK						(0b011 << 4)
#define IC3_UNLOCK						(0b100 << 4)

#define IC3_OUT1_DISABLE				(1<<3)
#define IC3_OUT2_DISABLE				(1<<2)

#define IC3_EN_IN1						(1<<1)
#define IC3_PH_IN2						(1<<0)

#define IC4_ENABLE_OLP					(1<<6)

#define IC4_OLP_DLY_0_3MS				(0<<5)
#define IC4_OLP_DLY_1_2S				(1<<5)

#define IC4_OLA_ENABLE					(1<<4)
#define IC4_OLA_DISABLE					(0<<4)

#define IC4_ITRIP_LVL_4_0A				(0<<2)
#define IC4_ITRIP_LVL_5_4A				(1<<2)
#define IC4_ITRIP_LVL_6_5A				(2<<2)
#define IC4_ITRIP_LVL_7_0A				(3<<2)

#define IC4_ITRIP_OUT1_DISABLED			(1<<0)
#define IC4_ITRIP_OUT2_DISABLED			(2<<0)
#define IC4_ITRIP_DISABLED				(IC4_ITRIP_OUT1_DISABLED|IC4_ITRIP_OUT2_DISABLED)


#define FSR_FAULT				(1<<6)
#define FSR_OTW					(1<<5)
#define FSR_UVLO				(1<<4)
#define FSR_CPUV				(1<<3)
#define FSR_OCP					(1<<2)
#define FSR_TSD					(1<<1)
#define FSR_OLD					(1<<0)

#define DIAG_OL1				(1<<7)
#define DIAG_OL2				(1<<6)
#define DIAG_ITRIP1				(1<<5)
#define DIAG_ITRIP2				(1<<4)
#define DIAG_OCP_H1				(1<<3)
#define DIAG_OCP_L1				(1<<2)
#define DIAG_OCP_H2				(1<<1)
#define DIAG_OCP_L2				(1<<0)



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

extern DRV8873S drv8873S;
