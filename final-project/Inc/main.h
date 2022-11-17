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
typedef char bool;
typedef int MODE;

// Model for a rigid body.
typedef struct __rigid_body
{
  // x-position.
  int32_t x;
  // y-coordinate.
  int32_t y;
  // x-velocity.
  int32_t vel_x;
  // y-velocity.
  int32_t vel_y;
} rigid_body;

// Model for the user's character.
typedef struct __user
{
  // Health.
  int32_t health;
  // Rigid body for the user.
  rigid_body rb;
} user;

// Model for an enemy.
typedef struct __enemy
{
  // Enabled.
  bool enabled;
  // Health.
  int32_t health;
  // Rigid body for the enemy.
  rigid_body rb;
} enemy;

// Model for a projectile.
typedef struct __projectile
{
  // Enabled.
  bool enabled;
  // Rigid body for the projectile.
  rigid_body rb;
} projectile;
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

// Debugging port
#define ITM_Port32(n) (*((volatile unsigned long *) (0xE0000000+4*n)))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void initEngine();
void initInput();

void delay(uint32_t delay);
void hal_exec(uint8_t HAL_Status);
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
#define false (bool)0
#define true  !false

// ================
// MODE VARIABLES
// ================
#define MODE_RTOS         0b0001 // Release mode
#define MODE_TEST_INPUT   0b0010 // Mode testing input sensors
#define MODE_TEST_ENGINE  0b0100 // Mode testing engine
#define MODE_TEST_OUTPUT  0b1000 // Mode testing output generation

// ================
// RTOS DEFINES
// ================
#define OS_TIMEOUT 30000
#define IRQ_TIMEOUT 5

// ================
// INPUT DEFINES
// ================

// ================
// ENGINE DEFINES
// ================
// dimensions of the playing field
#define MAX_X 20
#define MAX_Y 15
#define NUM_ENEMIES 5
#define NUM_PROJECTILES 10

// ================
// OUTPUT DEFINES
// ================
#define UART_TIMEOUT 30000
// dimensions of the buffer
#define MAX_BUF_SIZE ((MAX_X + 4) * (MAX_Y + 2))
#define SCR_WIDTH (MAX_X + 4)
#define SCR_HEIGHT (MAX_Y + 2)
// refresh rate of the screen (in Hz)
/*
 * Automatically calculates a safe refresh
 * rate based on the resolution of the field.
 *
 * The baudrate of the UART terminal is set to
 * 115200 bits/second -> 14400 bytes/second.
 *
 * Divide the baudrate by twice the size of the
 * field buffer to get the number of times per
 * second the field can be rendered.
 *
 * Multiply by 2 because we need to transmit '\b'
 * characters to clear the field.
 */
#define REFRESH_RATE (14400 / (2 * MAX_BUF_SIZE))
// game characters
#define USER '^'
#define ENEMY '@'
#define PROJECTILE '|'
// playing field characters
#define BACKGROUND ' '
#define BORDER '*'
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
