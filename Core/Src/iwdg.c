/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    iwdg.c
  * @brief   This file provides code for the configuration
  *          of the IWDG instances.
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
/* Includes ------------------------------------------------------------------*/
#include "iwdg.h"

/* USER CODE BEGIN 0 */

#include "cmsis_os.h"
#include <stdbool.h>

#define NUM_TASKS 5   //Number of Tasks that are monitored by IWDG
volatile uint8_t task_health[NUM_TASKS] = {0}; // Each task is responsible to update
extern osMutexId_t health_array_mutexHandle;

// Returns number of tasks which are monitored by IWDG
int  get_number_of_tasks(void){ return NUM_TASKS; }

void update_health_array (int task_number)
{

    if (0 < task_number && task_number < NUM_TASKS)
    {
        osMutexAcquire(health_array_mutexHandle, osWaitForever);
        task_health[task_number] = 1;
        osMutexRelease(health_array_mutexHandle);
    }
}

bool test_health_array (void)
{
    osMutexAcquire(health_array_mutexHandle, osWaitForever);
    for (int i = 0; i < NUM_TASKS; i++)
    {
        if ( 0 == task_health[i] )
        {
            osMutexRelease(health_array_mutexHandle);
            return false;
        }
    }
    osMutexRelease(health_array_mutexHandle);
    return true;
}

void clear_health_array (void)
{
    osMutexAcquire(health_array_mutexHandle, osWaitForever);
    for (int i = 0; i < NUM_TASKS; i++)
    {
        task_health[i] = 0;
    }
    osMutexRelease(health_array_mutexHandle);
}

void refresh_watchdog(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

// Provide means for Tasks to determine IWDG timeout
uint32_t get_iwdg_timeout(void) {
    return ((uint32_t)(((4 << hiwdg.Init.Prescaler) * hiwdg.Init.Reload) / 32000));
}

/* USER CODE END 0 */

IWDG_HandleTypeDef hiwdg;

/* IWDG init function */
void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

