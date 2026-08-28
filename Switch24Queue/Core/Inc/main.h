/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

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
#define Col1_Pin GPIO_PIN_13
#define Col1_GPIO_Port GPIOC
#define Col2_Pin GPIO_PIN_14
#define Col2_GPIO_Port GPIOC
#define Col3_Pin GPIO_PIN_15
#define Col3_GPIO_Port GPIOC
#define Col4_Pin GPIO_PIN_0
#define Col4_GPIO_Port GPIOC
#define Col5_Pin GPIO_PIN_1
#define Col5_GPIO_Port GPIOC
#define Row1_Pin GPIO_PIN_2
#define Row1_GPIO_Port GPIOC
#define Row2_Pin GPIO_PIN_3
#define Row2_GPIO_Port GPIOC
#define Row3_Pin GPIO_PIN_0
#define Row3_GPIO_Port GPIOA
#define Row4_Pin GPIO_PIN_1
#define Row4_GPIO_Port GPIOA
#define Row5_Pin GPIO_PIN_2
#define Row5_GPIO_Port GPIOA
#define NET_LED_Pin GPIO_PIN_3
#define NET_LED_GPIO_Port GPIOA
#define Out_17_Pin GPIO_PIN_4
#define Out_17_GPIO_Port GPIOA
#define Out_16_Pin GPIO_PIN_5
#define Out_16_GPIO_Port GPIOA
#define Out_15_Pin GPIO_PIN_6
#define Out_15_GPIO_Port GPIOA
#define Out_14_Pin GPIO_PIN_7
#define Out_14_GPIO_Port GPIOA
#define Out_13_Pin GPIO_PIN_4
#define Out_13_GPIO_Port GPIOC
#define Out_5_Pin GPIO_PIN_5
#define Out_5_GPIO_Port GPIOC
#define Out_4_Pin GPIO_PIN_0
#define Out_4_GPIO_Port GPIOB
#define Out_3_Pin GPIO_PIN_1
#define Out_3_GPIO_Port GPIOB
#define Out_2_Pin GPIO_PIN_2
#define Out_2_GPIO_Port GPIOB
#define Out_1_Pin GPIO_PIN_10
#define Out_1_GPIO_Port GPIOB
#define STATE_LED_Pin GPIO_PIN_11
#define STATE_LED_GPIO_Port GPIOB
#define Out_24_Pin GPIO_PIN_12
#define Out_24_GPIO_Port GPIOB
#define Out_23_Pin GPIO_PIN_13
#define Out_23_GPIO_Port GPIOB
#define Out_22_Pin GPIO_PIN_14
#define Out_22_GPIO_Port GPIOB
#define Out_21_Pin GPIO_PIN_15
#define Out_21_GPIO_Port GPIOB
#define Out_20_Pin GPIO_PIN_6
#define Out_20_GPIO_Port GPIOC
#define Out_19_Pin GPIO_PIN_7
#define Out_19_GPIO_Port GPIOC
#define Out_18_Pin GPIO_PIN_8
#define Out_18_GPIO_Port GPIOC
#define Out_6_Pin GPIO_PIN_9
#define Out_6_GPIO_Port GPIOC
#define Out_7_Pin GPIO_PIN_8
#define Out_7_GPIO_Port GPIOA
#define Out_8_Pin GPIO_PIN_9
#define Out_8_GPIO_Port GPIOA
#define Out_9_Pin GPIO_PIN_10
#define Out_9_GPIO_Port GPIOA
#define Out_10_Pin GPIO_PIN_11
#define Out_10_GPIO_Port GPIOA
#define Out_11_Pin GPIO_PIN_12
#define Out_11_GPIO_Port GPIOA
#define Out_12_Pin GPIO_PIN_15
#define Out_12_GPIO_Port GPIOA
#define DIR_Pin GPIO_PIN_5
#define DIR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
