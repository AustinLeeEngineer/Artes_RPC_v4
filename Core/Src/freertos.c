/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <queue.h>
#include "artes_rpc_scpi.h"
#include "cli.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define RX_BUF_SIZE          256


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osMutexId_t serial_tx_mutexHandle = NULL;
osMessageQueueId_t UartRxQueueHandle = NULL;

//extern osMutexId_t serial_tx_mutexHandle;
extern osThreadId_t SysHealthTaskHandle;
extern osThreadId_t LED1_RHandle;
extern osThreadId_t LED1_GHandle;
extern osThreadId_t CliTaskHandle;
extern UART_HandleTypeDef huart2;
extern IWDG_HandleTypeDef hiwdg;
void safe_printf(const char *format, ...);

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

// These functions found in iwdg.c
void     update_health_array (int task_number);
void     clear_health_array  (void);
void     refresh_watchdog    (void);
bool     test_health_array   (void);
int      get_number_of_tasks (void);
uint32_t get_iwdg_timeout    (void);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
    SCPI_Init(&scpi_context,
            scpi_commands,
            &scpi_interface,
            scpi_units_def,
            SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
            scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
            scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);


  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
    // CMSIS V2 Mutex Creation
    serial_tx_mutexHandle = osMutexNew(NULL);

    if (serial_tx_mutexHandle == NULL) {
        // Handle creation failure (optional but recommended)
    }
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
    // CMSIS V2 Queue Creation (10 items, 1 byte each)
    UartRxQueueHandle = osMessageQueueNew(10, sizeof(uint8_t), NULL);

    if (UartRxQueueHandle == NULL) {
        // Handle creation failure
    }
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	//AddedByAustin
		uint8_t rx_byte;
  /* Infinite loop */
  for(;;)
  {
  	//Set LED to green to indicate we got here and waiting for interrupt.
  	set_led_color(LED_RUNNING, LED_COLOR_GREEN);

	  if (osMessageQueueGet(UartRxQueueHandle, &rx_byte, NULL, 1000) == osOK)
	        {
		  	  //safe_printf("RTOS got byte: %c\r\n", rx_byte);

	            // We got a byte! Hand it to the CLI engine to process.

		  //Make LED red so that if steps after this fail, it stays red and indicates failure to return to set led green above.
		  	  set_led_color(LED_RUNNING, LED_COLOR_RED);
	        	cli_process_byte(rx_byte);


	            //set_led_state(LED_RUNNING, LED_STATE_ON);
	            //HAL_Delay(200);
	            //set_led_state(LED_RUNNING, LED_STATE_ON);
	        }
	        //Pet the watchdog timer
	        //safe_printf("About to pet dog \r\n");
	        HAL_IWDG_Refresh(&hiwdg);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/**
 * @brief Thread-safe printf-like function for serial output.
 *        Protects transmission with serial_tx_mutex to prevent interleaving
 *        from higher-priority tasks.
 *
 * @param format Format string (printf-style).
 * @param ... Variable arguments for formatting.
 */
void safe_printf(const char *format, ...) {
    // 1. GUARD: Don't try to print if the UART hardware hasn't booted up yet!
    if (huart2.Instance == NULL) return;

    char buffer[256];
    va_list args;
    int len;

    // 2. GUARD: Is it safe to use the Mutex?
    // __get_IPSR() == 0 means we are running normally (NOT inside an interrupt).
    // serial_tx_mutexHandle != NULL means the RTOS has successfully created the mutex.
    bool safe_to_block = (__get_IPSR() == 0) && (serial_tx_mutexHandle != NULL);

    // Only lock the Mutex if we are safe in a thread
    if (safe_to_block) {
        osMutexAcquire(serial_tx_mutexHandle, osWaitForever);
    }

    // Format the string
    va_start(args, format);
    len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // --- HARDWARE-LEVEL TRANSMIT ---
    for (int i = 0; i < len; i++) {
        // Wait until the Transmit Data Register is empty and ready
        while (!__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE)) {}

        // Push the character directly into the hardware register
        huart2.Instance->TDR = buffer[i];
    }
    // -----------------------------------

    // Unlock the Mutex
    if (safe_to_block) {
        osMutexRelease(serial_tx_mutexHandle);
    }
}


/**
 * @brief Print current stack high water mark for all tasks.
 *        High water mark = minimum free stack bytes ever seen.
 */
void report_stack_usage(void)
{
    safe_printf("=== Stack Usage (High Water Mark in bytes) ===\r\n");

    //safe_printf("SysHealthTask : %u\r\n", uxTaskGetStackHighWaterMark(SysHealthTaskHandle) * 4);
    //safe_printf("LED1_R        : %u\r\n", uxTaskGetStackHighWaterMark(LED1_RHandle) * 4);
    //safe_printf("LED1_G        : %u\r\n", uxTaskGetStackHighWaterMark(LED1_GHandle) * 4);
    //safe_printf("CliTask       : %u\r\n", uxTaskGetStackHighWaterMark(CliTaskHandle) * 4);
    safe_printf("============================================\r\n");
}
/* USER CODE END Application */

