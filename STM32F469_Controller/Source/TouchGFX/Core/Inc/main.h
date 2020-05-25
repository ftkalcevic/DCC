/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define countof(x)      (sizeof(x)/sizeof((x)[0]))
/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern ADC_HandleTypeDef hadc1;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;
extern CAN_HandleTypeDef hcan2;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SAI1_FSA_Pin GPIO_PIN_4
#define SAI1_FSA_GPIO_Port GPIOE
#define SPKR_HP_Pin GPIO_PIN_3
#define SPKR_HP_GPIO_Port GPIOE
#define AUDIO_RST_Pin GPIO_PIN_2
#define AUDIO_RST_GPIO_Port GPIOE
#define Boost_CS_Pin GPIO_PIN_14
#define Boost_CS_GPIO_Port GPIOG
#define SAI1_SCKA_Pin GPIO_PIN_5
#define SAI1_SCKA_GPIO_Port GPIOE
#define Boost_Disable_Pin GPIO_PIN_13
#define Boost_Disable_GPIO_Port GPIOG
#define OTG_FS1_OverCurrent_Pin GPIO_PIN_7
#define OTG_FS1_OverCurrent_GPIO_Port GPIOB
#define Fn4_Pin GPIO_PIN_11
#define Fn4_GPIO_Port GPIOG
#define MIC_DATA_Pin GPIO_PIN_6
#define MIC_DATA_GPIO_Port GPIOD
#define Fn1_Pin GPIO_PIN_12
#define Fn1_GPIO_Port GPIOG
#define PrgTrk_Fault_Pin GPIO_PIN_10
#define PrgTrk_Fault_GPIO_Port GPIOG
#define LED3_Pin GPIO_PIN_5
#define LED3_GPIO_Port GPIOD
#define BOOST_SCK_Pin GPIO_PIN_3
#define BOOST_SCK_GPIO_Port GPIOD
#define SPI1_IRQ_Pin GPIO_PIN_13
#define SPI1_IRQ_GPIO_Port GPIOC
#define LED4_Pin GPIO_PIN_3
#define LED4_GPIO_Port GPIOK
#define Boost_Fault_Pin GPIO_PIN_9
#define Boost_Fault_GPIO_Port GPIOG
#define LED2_Pin GPIO_PIN_4
#define LED2_GPIO_Port GPIOD
#define SPI1_CE_Pin GPIO_PIN_8
#define SPI1_CE_GPIO_Port GPIOA
#define DCC_Signal_Pin GPIO_PIN_7
#define DCC_Signal_GPIO_Port GPIOC
#define I2C2_SCL_Pin GPIO_PIN_4
#define I2C2_SCL_GPIO_Port GPIOH
#define PrgTrk_DCC_Signal_Pin GPIO_PIN_6
#define PrgTrk_DCC_Signal_GPIO_Port GPIOC
#define I2C2_SDA_Pin GPIO_PIN_5
#define I2C2_SDA_GPIO_Port GPIOH
#define SAI1_MCLKA_Pin GPIO_PIN_7
#define SAI1_MCLKA_GPIO_Port GPIOG
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOG
#define FWD_Pin GPIO_PIN_3
#define FWD_GPIO_Port GPIOC
#define CANRX_Pin GPIO_PIN_12
#define CANRX_GPIO_Port GPIOB
#define Fn5_Pin GPIO_PIN_1
#define Fn5_GPIO_Port GPIOC
#define EncoderB_Pin GPIO_PIN_2
#define EncoderB_GPIO_Port GPIOC
#define OTG_FS1_PowerSwitchOn_Pin GPIO_PIN_2
#define OTG_FS1_PowerSwitchOn_GPIO_Port GPIOB
#define uSD_Detect_Pin GPIO_PIN_2
#define uSD_Detect_GPIO_Port GPIOG
#define LCD_INT_Pin GPIO_PIN_5
#define LCD_INT_GPIO_Port GPIOJ
#define Boost_Sense_Pin GPIO_PIN_1
#define Boost_Sense_GPIO_Port GPIOA
#define PrgTrk_ACK_Pin GPIO_PIN_4
#define PrgTrk_ACK_GPIO_Port GPIOA
#define REV_Pin GPIO_PIN_4
#define REV_GPIO_Port GPIOC
#define Fn2_Pin GPIO_PIN_2
#define Fn2_GPIO_Port GPIOA
#define Fn3_Pin GPIO_PIN_6
#define Fn3_GPIO_Port GPIOA
#define EStop_Pin GPIO_PIN_5
#define EStop_GPIO_Port GPIOC
#define STLK_RX_Pin GPIO_PIN_10
#define STLK_RX_GPIO_Port GPIOB
#define PrgTrk_Disable_Pin GPIO_PIN_6
#define PrgTrk_Disable_GPIO_Port GPIOH
#define LCD_BL_CTRL_Pin GPIO_PIN_3
#define LCD_BL_CTRL_GPIO_Port GPIOA
#define PrgTrk_CS_Pin GPIO_PIN_7
#define PrgTrk_CS_GPIO_Port GPIOA
#define EncoderA_Pin GPIO_PIN_1
#define EncoderA_GPIO_Port GPIOB
#define EXT_RESET_Pin GPIO_PIN_0
#define EXT_RESET_GPIO_Port GPIOB
#define STLK_TX_Pin GPIO_PIN_11
#define STLK_TX_GPIO_Port GPIOB
#define BOOST_MISO_Pin GPIO_PIN_14
#define BOOST_MISO_GPIO_Port GPIOB
#define BOOST_MOSI_Pin GPIO_PIN_15
#define BOOST_MOSI_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define APB1_TIMER_CLOCK   90000000
#define APB2_TIMER_CLOCK  180000000
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
