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

#include "FreeRTOS.h"
#include "cmsis_os.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef char bool;
typedef int MODE;

typedef void (*locked_func)(void*);

// Model to store an object to display on the screen.
typedef struct __rigid_body
{
  // Enabled.
  bool enabled;
  // x-position.
  int32_t x;
  // y-position.
  int32_t y;
} rigid_body;
typedef rigid_body user;
typedef rigid_body enemy;
typedef rigid_body projectile;

// Model to store text to display on screen
#define MAX_X 20
#define MAX_Y 15
typedef struct __game_text
{
  // whether to draw the text
  bool enabled;
  // x_offset
  int32_t indentation;
  // string
  char text[MAX_X];
} game_text;

// Model to store all game objects.
#define NUM_ENEMIES 5
#define NUM_PROJECTILES 10
typedef struct __game_objects
{
  // User.
  user user;
  // Enemies.
  enemy enemies[NUM_ENEMIES];
  // Projectiles.
  projectile projectiles[NUM_PROJECTILES];
  // text.
  game_text text[MAX_Y];
} game_objects;

// Model to interface with a shared variable.
typedef struct __sharedvariable
{
  osSemaphoreDef_t semaphoreDef;
  osSemaphoreId semaphoreHandle;
  void *var;
} shared_variable;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern const MODE mode;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define ARRAY_LEN(a) sizeof(a) / sizeof(a[0])
#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

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
uint32_t createSharedVariable(int32_t count, void *var);
uint32_t getSharedVariable(void *var);

void enableSharedVariables();
void disableSharedVariables();
void *lockSharedVariable(uint32_t idx, uint32_t timeout);
void releaseSharedVariable(uint32_t idx);
void lockSharedVariableAndExecute(uint32_t idx, uint32_t timeout, locked_func func);

void delay(uint32_t delay);
void hal_exec(uint8_t HAL_Status);
void gen_sine(uint32_t *buf, uint32_t buf_size, float step, uint32_t dac_max);
bool is_user_btn_down();
bool is_user_btn_up();
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
#define NUM_SEMAPHORES 32

// ================
// OUTPUT DEFINES
// ================
#define UART_TIMEOUT 30000
// dimensions of the buffer
#define MAX_BUF_SIZE ((MAX_X + 4) * (MAX_Y + 2))
#define SCR_WIDTH (MAX_X + 4)
#define SCR_HEIGHT (MAX_Y + 2)
// refresh rate of the screen
/*
 * Automatically calculates a safe refresh
 * rate based on the resolution of the field
 *
 * UART baudrate = 115200 bits/sec
 *
 * Make it wait 3x as long as necessary to let us see
 * the frame more clearly
 */
#define REFRESH_RATE (115200 / (MAX_BUF_SIZE * 6 * 10))
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
