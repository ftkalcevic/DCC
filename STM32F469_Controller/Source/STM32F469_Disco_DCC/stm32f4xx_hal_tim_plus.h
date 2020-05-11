#pragma once


#ifdef __cplusplus
extern "C" {
#endif

extern HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart2(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress, uint32_t BurstRequestSrc,
													  uint32_t *BurstBuffer, uint32_t BufferLength, uint32_t  BurstLength);
#ifdef __cplusplus
}
#endif


	
	