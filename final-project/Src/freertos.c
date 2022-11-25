/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
bool semaphoresEnabled = false;
uint32_t activeSemaphores = 0;
shared_variable semaphores[NUM_SEMAPHORES] = {{{0, NULL}, NULL, NULL}};
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
 * Create a shared variable.
 * @param count the number of allowed concurrent accesses
 * @param var the pointer to the variable to share.
 * @returns the ID of the created variable, -1 if not created.
 */
uint32_t createSharedVariable(int32_t count, void *var)
{
  if (activeSemaphores >= NUM_SEMAPHORES)
  {
    return -1;
  }

  shared_variable *sv = semaphores + activeSemaphores;
  sv->semaphoreHandle = osSemaphoreCreate(&sv->semaphoreDef, count);
  sv->var = var;

  return activeSemaphores++;
}

/**
 * Get the ID of a shared variable.
 * @param var the pointer to the shared variable.
 * @returns the ID of the variable, -1 if it does not exist.
 */
uint32_t getSharedVariable(void *var)
{
  uint32_t i = 0;
  shared_variable *sv;
  while (i < NUM_SEMAPHORES)
  {
    sv = semaphores + i;
    if (sv->var && sv->var == var)
    {
      return i;
    }
    ++i;
  }

  return -1;
}

/**
 * Enable access control to shared variables.
 */
void enableSharedVariables()
{
  semaphoresEnabled = true;
}

/**
 * Disable access control to shared variables.
 */
void disableSharedVariables()
{
  semaphoresEnabled = false;
}

/**
 * Lock a shared variable.
 * @param idx the ID of the shared variable.
 * @param timeout the maximum time to wait to take the variable.
 * @returns the pointer to the shared variable, NULL if not successful.
 */
void *lockSharedVariable(uint32_t idx, uint32_t timeout)
{
  return idx < NUM_SEMAPHORES && (!semaphoresEnabled || osSemaphoreWait(semaphores[idx].semaphoreHandle, timeout) == osOK)
      ? semaphores[idx].var
      : NULL;
}

/**
 * Release a shared variable.
 * @param idx the ID of the shared variable.
 */
void releaseSharedVariable(uint32_t idx)
{
  if (semaphoresEnabled)
  {
    osSemaphoreRelease(semaphores[idx].semaphoreHandle);
  }
}

/**
 * Wrap a function that uses a shared variable.
 * @param idx the ID of the shared variable to pass into the function.
 * @param timeout the maximum time to wait for access to the variable.
 * @param func the function to execute after gaining access.
 */
void lockSharedVariableAndExecute(uint32_t idx, uint32_t timeout, locked_func func)
{
  void *var = lockSharedVariable(idx, timeout);
  if (var)
  {
    func(var);
    releaseSharedVariable(idx);
  }
}
/* USER CODE END Application */
