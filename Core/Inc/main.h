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

#include "stm32f4xx_ll_crc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"

#include "stm32f4xx_ll_exti.h"

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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TP_SPI_CS_Pin GPIO_PIN_2
#define TP_SPI_CS_GPIO_Port GPIOE
#define LCD_TPRST_Pin GPIO_PIN_3
#define LCD_TPRST_GPIO_Port GPIOE
#define TP_BSY_Pin GPIO_PIN_8
#define TP_BSY_GPIO_Port GPIOI
#define TP_SIRQ_Pin GPIO_PIN_13
#define TP_SIRQ_GPIO_Port GPIOC
#define USR_LED_1_Pin GPIO_PIN_14
#define USR_LED_1_GPIO_Port GPIOC
#define USR_LED_2_Pin GPIO_PIN_15
#define USR_LED_2_GPIO_Port GPIOC
#define USR_BTN_1_Pin GPIO_PIN_0
#define USR_BTN_1_GPIO_Port GPIOA
#define USR_BTN_2_Pin GPIO_PIN_2
#define USR_BTN_2_GPIO_Port GPIOA
#define TX_Pin GPIO_PIN_10
#define TX_GPIO_Port GPIOB
#define RX_Pin GPIO_PIN_11
#define RX_GPIO_Port GPIOB
#define R_RST_Pin GPIO_PIN_6
#define R_RST_GPIO_Port GPIOH
#define R_INT_Pin GPIO_PIN_7
#define R_INT_GPIO_Port GPIOH
#define R_CS_Pin GPIO_PIN_12
#define R_CS_GPIO_Port GPIOB
#define SDIO_CDET_Pin GPIO_PIN_8
#define SDIO_CDET_GPIO_Port GPIOA
#define LCD_GPIO6_Pin GPIO_PIN_1
#define LCD_GPIO6_GPIO_Port GPIOI
#define LCD_GPIO5_Pin GPIO_PIN_15
#define LCD_GPIO5_GPIO_Port GPIOA
#define LCD_GPIO4_Pin GPIO_PIN_4
#define LCD_GPIO4_GPIO_Port GPIOD
#define LCD_GPIO3_Pin GPIO_PIN_5
#define LCD_GPIO3_GPIO_Port GPIOD
#define LCD_GPIO2_Pin GPIO_PIN_7
#define LCD_GPIO2_GPIO_Port GPIOD
#define STL_RX_Pin GPIO_PIN_9
#define STL_RX_GPIO_Port GPIOG
#define STL_TX_Pin GPIO_PIN_14
#define STL_TX_GPIO_Port GPIOG
#define LCD_GPIO1_Pin GPIO_PIN_7
#define LCD_GPIO1_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_8
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_9
#define SDA_GPIO_Port GPIOB
#define LCD_TPINT_Pin GPIO_PIN_5
#define LCD_TPINT_GPIO_Port GPIOI
/* USER CODE BEGIN Private defines */

#define RIB_MOSI_GPIO_Port GPIOB
#define RIB_MOSI_Pin GPIO_PIN_15
#define RIB_MISO_GPIO_Port GPIOB
#define RIB_MISO_Pin GPIO_PIN_14
#define RIB_SCK_GPIO_Port GPIOB
#define RIB_SCK_Pin GPIO_PIN_13

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
