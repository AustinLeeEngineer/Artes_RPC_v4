/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "iwdg.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "artes_rpc_scpi.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void led_init(void);
void safe_printf(const char *format, ...);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  led_init();
  //Turn LED running red then later deep in the code green to show its running.
  set_led_state(LED_RUNNING, LED_STATE_ON);
  set_led_color(LED_RUNNING, LED_COLOR_RED);
//XXX:  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  // Pulse XPORT Reset line low for .2 second on powerup or NRST
  // HAL_Delay is safe before osKernelStart()
  // Datasheet says nRST pulse triggers internal 200ms reset. Nothing
  // on minimum external pulse width, so ...
  /*
  HAL_GPIO_WritePin(XPORT_RST_GRP, XP_nRST_Pin, GPIO_PIN_RESET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(XPORT_RST_GRP, XP_nRST_Pin, GPIO_PIN_SET);
  led_init();

  HAL_Delay(1000);
  HAL_GPIO_WritePin (LED_GPIO_GRP, LED2_G_Pin, GPIO_PIN_SET);

  HAL_GPIO_WritePin(Relay1_GPIO_Port, Relay1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(Relay2_GPIO_Port, Relay2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(Relay3_GPIO_Port, Relay3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(Relay4_GPIO_Port, Relay4_Pin, GPIO_PIN_SET);

  HAL_Delay(1000);
  HAL_GPIO_WritePin (LED_GPIO_GRP, LED2_G_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(Relay1_GPIO_Port, Relay1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Relay2_GPIO_Port, Relay2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Relay3_GPIO_Port, Relay3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Relay4_GPIO_Port, Relay4_Pin, GPIO_PIN_RESET);

  //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
  /* USER CODE END 2 */
/*
  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

void safe_printf(const char *format, ...);
led_config_t led_sys, led_test;

void led_init (void)
{
    led_sys.color = LED_COLOR_GREEN;
    led_sys.state = LED_STATE_OFF;
    led_sys.blink_half_period_ms = 1000;
    led_sys.counter = 1;

    led_test.color = LED_COLOR_GREEN;
    led_test.state = LED_STATE_OFF;
    led_test.blink_half_period_ms = 1000;
    led_test.counter = 1;
}

void set_led_freq(led_types_t led_type, uint32_t val)
{
    if (LED_RUNNING == led_type)
    {
        led_test.blink_half_period_ms = val/2;
        led_test.counter = 1;
    }
    if (LED_PASS_FAIL == led_type)
    {
        led_sys.blink_half_period_ms = val/2;
        led_sys.counter = 1;
    }
}

void set_led_state(led_types_t led_type, led_state_t state)
{
    if (LED_RUNNING == led_type)
    {
        // SAFETY: Clear both pins first to prevent orange bleeding
        HAL_GPIO_WritePin(LED_GPIO_GRP, LED2_G_Pin | LED2_R_Pin, GPIO_PIN_RESET);

        led_test.state = state;
        if (LED_STATE_ON == led_test.state)
        {
            if (LED_COLOR_GREEN == led_test.color || LED_COLOR_YELLOW == led_test.color)
                HAL_GPIO_WritePin (LED_GPIO_GRP, LED2_G_Pin, GPIO_PIN_SET);
            if (LED_COLOR_RED == led_test.color || LED_COLOR_YELLOW == led_test.color)
                HAL_GPIO_WritePin (LED_GPIO_GRP, LED2_R_Pin, GPIO_PIN_SET);
        }
    }

    if (LED_PASS_FAIL == led_type)
    {
        // SAFETY: Clear both pins first
        HAL_GPIO_WritePin(LED_GPIO_GRP, LED1_G_Pin | LED1_R_Pin, GPIO_PIN_RESET);

        led_sys.state = state;
        if (LED_STATE_ON == led_sys.state)
        {
            // FIXED: Now correctly checks led_sys.color instead of led_test.color
            if (LED_COLOR_GREEN == led_sys.color || LED_COLOR_YELLOW == led_sys.color)
                HAL_GPIO_WritePin (LED_GPIO_GRP, LED1_G_Pin, GPIO_PIN_SET);
            if (LED_COLOR_RED == led_sys.color || LED_COLOR_YELLOW == led_sys.color)
                HAL_GPIO_WritePin (LED_GPIO_GRP, LED1_R_Pin, GPIO_PIN_SET);
        }
    }
}

void set_led_color(led_types_t led_type, led_color_t color)
{
    if (LED_RUNNING == led_type)
    {
        // SAFETY: Reset pins to prevent color mixing
        HAL_GPIO_WritePin(LED_GPIO_GRP, LED2_G_Pin | LED2_R_Pin, GPIO_PIN_RESET);
        led_test.color = color;
        if (LED_STATE_ON == led_test.state)
        {
            if (LED_COLOR_GREEN == led_test.color || LED_COLOR_YELLOW == led_test.color)
                HAL_GPIO_WritePin (LED_GPIO_GRP, LED2_G_Pin, GPIO_PIN_SET);
            if (LED_COLOR_RED == led_test.color || LED_COLOR_YELLOW == led_test.color)
                HAL_GPIO_WritePin (LED_GPIO_GRP, LED2_R_Pin, GPIO_PIN_SET);
        }
    }
    if (LED_PASS_FAIL == led_type)
    {
        // SAFETY: Reset pins to prevent color mixing
        HAL_GPIO_WritePin(LED_GPIO_GRP, LED1_G_Pin | LED1_R_Pin, GPIO_PIN_RESET);
        led_sys.color = color;
        if (LED_STATE_ON == led_sys.state)
        {
            if (LED_COLOR_GREEN == led_sys.color || LED_COLOR_YELLOW == led_sys.color)
                HAL_GPIO_WritePin (LED_GPIO_GRP, LED1_G_Pin, GPIO_PIN_SET);
            if (LED_COLOR_RED == led_sys.color || LED_COLOR_YELLOW == led_sys.color)
                HAL_GPIO_WritePin (LED_GPIO_GRP, LED1_R_Pin, GPIO_PIN_SET);
        }
    }
}

void set_relay_state(relay_types_t relay, relay_state_t state)
{
    // Convert your software state to a hardware STM32 state
    GPIO_PinState pin_state = (state == RELAY_STATE_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET;

    switch(relay)
    {
        // --- Main Power Relays ---
        case RELAY_AC1:
            HAL_GPIO_WritePin(Relay1_GPIO_Port, Relay1_Pin, pin_state);
            break;

        case RELAY_AC2:
            HAL_GPIO_WritePin(Relay2_GPIO_Port, Relay2_Pin, pin_state);
            break;

        case RELAY_ITA:
            HAL_GPIO_WritePin(Relay3_GPIO_Port, Relay3_Pin, pin_state);
            break;

        case RELAY_3PHASE:
            HAL_GPIO_WritePin(Relay4_GPIO_Port, Relay4_Pin, pin_state);
            break;

        // --- Safety Inhibit Relays ---
        case RELAY_INH1:
            HAL_GPIO_WritePin(Inh_Relay1_GPIO_Port, Inh_Relay1_Pin, pin_state);
            break;

        case RELAY_INH2:
            HAL_GPIO_WritePin(Inh_Relay2_GPIO_Port, Inh_Relay2_Pin, pin_state);
            break;

        case RELAY_INH3:
            HAL_GPIO_WritePin(Inh_Relay3_GPIO_Port, Inh_Relay3_Pin, pin_state);
            break;

        case RELAY_INH4:
            HAL_GPIO_WritePin(Inh_Relay4_GPIO_Port, Inh_Relay4_Pin, pin_state);
            break;

        // --- The "ALL" Command ---
        case RELAY_ALL:
            // Apply the requested state to every single relay simultaneously
            HAL_GPIO_WritePin(Relay1_GPIO_Port, Relay1_Pin, pin_state);
            HAL_GPIO_WritePin(Relay2_GPIO_Port, Relay2_Pin, pin_state);
            HAL_GPIO_WritePin(Relay3_GPIO_Port, Relay3_Pin, pin_state);
            HAL_GPIO_WritePin(Relay4_GPIO_Port, Relay4_Pin, pin_state);
            HAL_GPIO_WritePin(Inh_Relay1_GPIO_Port, Inh_Relay1_Pin, pin_state);
            HAL_GPIO_WritePin(Inh_Relay2_GPIO_Port, Inh_Relay2_Pin, pin_state);
            HAL_GPIO_WritePin(Inh_Relay3_GPIO_Port, Inh_Relay3_Pin, pin_state);
            HAL_GPIO_WritePin(Inh_Relay4_GPIO_Port, Inh_Relay4_Pin, pin_state);
            break;

        default:
            // Fail safely if an unknown relay tag is passed
            break;
    }
}

relay_state_t get_relay_state(relay_types_t relay)
{
    GPIO_PinState pin_state = GPIO_PIN_RESET;

    switch(relay)
    {
        case RELAY_AC1: pin_state = HAL_GPIO_ReadPin(Relay1_GPIO_Port, Relay1_Pin); break;
        case RELAY_AC2: pin_state = HAL_GPIO_ReadPin(Relay2_GPIO_Port, Relay2_Pin); break;
        case RELAY_ITA: pin_state = HAL_GPIO_ReadPin(Relay3_GPIO_Port, Relay3_Pin); break;
        case RELAY_3PHASE: pin_state = HAL_GPIO_ReadPin(Relay4_GPIO_Port, Relay4_Pin); break;
        case RELAY_INH1: pin_state = HAL_GPIO_ReadPin(Inh_Relay1_GPIO_Port, Inh_Relay1_Pin); break;
        case RELAY_INH2: pin_state = HAL_GPIO_ReadPin(Inh_Relay2_GPIO_Port, Inh_Relay2_Pin); break;
        case RELAY_INH3: pin_state = HAL_GPIO_ReadPin(Inh_Relay3_GPIO_Port, Inh_Relay3_Pin); break;
        case RELAY_INH4: pin_state = HAL_GPIO_ReadPin(Inh_Relay4_GPIO_Port, Inh_Relay4_Pin); break;
        default: return RELAY_STATE_OFF;
    }

    return (pin_state == GPIO_PIN_SET) ? RELAY_STATE_ON : RELAY_STATE_OFF;
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
    if (htim->Instance == TIM6)
    {
        //SYSTEM LED
        if (LED_STATE_BLINK == led_sys.state)     // Current state (off/on/blink)
        {
            if (--led_sys.counter == 0)
            {
                if (led_sys.color == LED_COLOR_GREEN || led_sys.color == LED_COLOR_YELLOW)
                {
                    HAL_GPIO_TogglePin(LED_GPIO_GRP, LED1_G_Pin);
                }
                if (led_sys.color == LED_COLOR_RED || led_sys.color == LED_COLOR_YELLOW)
                {
                    HAL_GPIO_TogglePin(LED_GPIO_GRP, LED1_R_Pin);
                }

                led_sys.counter = led_sys.blink_half_period_ms ;
            }
        }
    }

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
