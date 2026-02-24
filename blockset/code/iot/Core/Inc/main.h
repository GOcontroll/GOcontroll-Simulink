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
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

extern uint32_t main_thread_stack;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RTC_ALARM_OUT_Pin GPIO_PIN_13
#define RTC_ALARM_OUT_GPIO_Port GPIOC
#define INT2_ACC_Pin GPIO_PIN_0
#define INT2_ACC_GPIO_Port GPIOC
#define INT2_ACC_EXTI_IRQn EXTI0_IRQn
#define STM32_UART_TX_Pin GPIO_PIN_2
#define STM32_UART_TX_GPIO_Port GPIOA
#define STM32_UART_RX_Pin GPIO_PIN_3
#define STM32_UART_RX_GPIO_Port GPIOA
#define SPI_MOD_SCK_Pin GPIO_PIN_5
#define SPI_MOD_SCK_GPIO_Port GPIOA
#define SPI_MOD_MISO_Pin GPIO_PIN_6
#define SPI_MOD_MISO_GPIO_Port GPIOA
#define SPI_MOD_MOSI_Pin GPIO_PIN_7
#define SPI_MOD_MOSI_GPIO_Port GPIOA
#define MOD2_RESET_Pin GPIO_PIN_4
#define MOD2_RESET_GPIO_Port GPIOC
#define MOD2_INT_Pin GPIO_PIN_5
#define MOD2_INT_GPIO_Port GPIOC
#define MOD2_INT_EXTI_IRQn EXTI5_IRQn
#define KL15_ADC_Pin GPIO_PIN_0
#define KL15_ADC_GPIO_Port GPIOB
#define KL30_ADC_Pin GPIO_PIN_1
#define KL30_ADC_GPIO_Port GPIOB
#define ESP32_GPIO0_Pin GPIO_PIN_2
#define ESP32_GPIO0_GPIO_Port GPIOB
#define ESP32_GPIO1_Pin GPIO_PIN_10
#define ESP32_GPIO1_GPIO_Port GPIOB
#define CAN2_RX_UCI_Pin GPIO_PIN_12
#define CAN2_RX_UCI_GPIO_Port GPIOB
#define CAN2_TX_UCO_Pin GPIO_PIN_13
#define CAN2_TX_UCO_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_6
#define LED_B_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_7
#define LED_R_GPIO_Port GPIOC
#define LED_G_Pin GPIO_PIN_8
#define LED_G_GPIO_Port GPIOC
#define I2C_ACC_SDA_Pin GPIO_PIN_9
#define I2C_ACC_SDA_GPIO_Port GPIOC
#define I2C_ACC_SCL_Pin GPIO_PIN_8
#define I2C_ACC_SCL_GPIO_Port GPIOA
#define CAN1_SILENT_UCO_Pin GPIO_PIN_9
#define CAN1_SILENT_UCO_GPIO_Port GPIOA
#define CAN2_SILENT_UCO_Pin GPIO_PIN_10
#define CAN2_SILENT_UCO_GPIO_Port GPIOA
#define CAN1_RX_UCI_Pin GPIO_PIN_11
#define CAN1_RX_UCI_GPIO_Port GPIOA
#define CAN1_TX_UCO_Pin GPIO_PIN_12
#define CAN1_TX_UCO_GPIO_Port GPIOA
#define KL15_CONTROLLER_UCO_Pin GPIO_PIN_12
#define KL15_CONTROLLER_UCO_GPIO_Port GPIOC
#define SPI_MOD1_CS_Pin GPIO_PIN_2
#define SPI_MOD1_CS_GPIO_Port GPIOD
#define SPI_MOD2_CS_Pin GPIO_PIN_2
#define SPI_MOD2_CS_GPIO_Port GPIOD
#define MOD1_RESET_Pin GPIO_PIN_4
#define MOD1_RESET_GPIO_Port GPIOB
#define I2C_MOD_SMBA_Pin GPIO_PIN_5
#define I2C_MOD_SMBA_GPIO_Port GPIOB
#define I2C_MOD_SCL_Pin GPIO_PIN_6
#define I2C_MOD_SCL_GPIO_Port GPIOB
#define I2C_MOD_SDA_Pin GPIO_PIN_7
#define I2C_MOD_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
