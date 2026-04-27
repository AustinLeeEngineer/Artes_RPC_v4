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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "artes_rpc_scpi.h"

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

void set_led_freq(led_types_t led_type, uint32_t val);
void set_led_state(led_types_t led_type, led_state_t state);
void set_led_color(led_types_t led_type, led_color_t color);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define XP_nRST_Pin GPIO_PIN_13
#define XP_nRST_GPIO_Port GPIOC
#define LED1_R_Pin GPIO_PIN_0
#define LED1_R_GPIO_Port GPIOC
#define LED1_G_Pin GPIO_PIN_1
#define LED1_G_GPIO_Port GPIOC
#define LED2_R_Pin GPIO_PIN_2
#define LED2_R_GPIO_Port GPIOC
#define LED2_G_Pin GPIO_PIN_3
#define LED2_G_GPIO_Port GPIOC
#define UART_CTS_Pin GPIO_PIN_0
#define UART_CTS_GPIO_Port GPIOA
#define UART_RTS_Pin GPIO_PIN_1
#define UART_RTS_GPIO_Port GPIOA
#define UART_TX_Pin GPIO_PIN_2
#define UART_TX_GPIO_Port GPIOA
#define UART_RX_Pin GPIO_PIN_3
#define UART_RX_GPIO_Port GPIOA
#define B_Interlock1_Pin GPIO_PIN_4
#define B_Interlock1_GPIO_Port GPIOA
#define B_Interlock2_Pin GPIO_PIN_5
#define B_Interlock2_GPIO_Port GPIOA
#define B_Interlock3_Pin GPIO_PIN_6
#define B_Interlock3_GPIO_Port GPIOA
#define B_Interlock4_Pin GPIO_PIN_7
#define B_Interlock4_GPIO_Port GPIOA
#define Inh_Relay3_Pin GPIO_PIN_4
#define Inh_Relay3_GPIO_Port GPIOC
#define Inh_Relay4_Pin GPIO_PIN_5
#define Inh_Relay4_GPIO_Port GPIOC
#define Inh_Relay2_Pin GPIO_PIN_6
#define Inh_Relay2_GPIO_Port GPIOC
#define Inh_Relay1_Pin GPIO_PIN_7
#define Inh_Relay1_GPIO_Port GPIOC
#define Relay1_Pin GPIO_PIN_8
#define Relay1_GPIO_Port GPIOA
#define Relay2_Pin GPIO_PIN_9
#define Relay2_GPIO_Port GPIOA
#define Relay3_Pin GPIO_PIN_10
#define Relay3_GPIO_Port GPIOA
#define Relay4_Pin GPIO_PIN_11
#define Relay4_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define RELAY_PIN_GPIO_GROUP GPIOA
#define LED_GPIO_GRP         GPIOC
#define INH_RELAY_GPIO_GRP   GPIOC
#define XPORT_RST_GRP        GPIOC

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
