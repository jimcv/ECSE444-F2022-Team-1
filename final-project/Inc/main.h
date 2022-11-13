/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32l4s5i_iot01_accelero.h"
#include "stm32l4s5i_iot01_gyro.h"
#include "stm32l4s5i_iot01_magneto.h"
#include "stm32l4s5i_iot01_qspi.h"

#define ARM_MATH_CM4
#include "arm_math.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef int MODE;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern const MODE mode;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define ARRAY_LEN(a) sizeof(a) / sizeof(a[0])

#define IS_MODE_RTOS()    mode & MODE_RTOS
#define IS_MODE_INPUT()   mode & MODE_TEST_INPUT
#define IS_MODE_ENGINE()  mode & MODE_TEST_ENGINE
#define IS_MODE_OUTPUT()  mode & MODE_TEST_OUTPUT
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void flash_exec(uint8_t QSPI_Memory_Status);
void gen_sine(uint32_t *buf, uint32_t buf_size, float step, uint32_t dac_max);
void led_green_on();
void led_green_off();
void led_red_on();
void led_red_off();

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_RED_Pin GPIO_PIN_3
#define LED_RED_GPIO_Port GPIOE
#define USER_BUTTON_Pin GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define USER_BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define LED_GREEN_Pin GPIO_PIN_14
#define LED_GREEN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
// MODE VARIABLES
#define MODE_RTOS         0b0001 // Release mode
#define MODE_TEST_INPUT   0b0010 // Mode testing input sensors
#define MODE_TEST_ENGINE  0b0100 // Mode testing engine
#define MODE_TEST_OUTPUT  0b1000 // Mode testing output generation
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
