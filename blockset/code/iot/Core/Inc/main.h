/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define NINA_RESET_Pin GPIO_PIN_3
#define NINA_RESET_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_0
#define LED_R_GPIO_Port GPIOA
#define LED_G_Pin GPIO_PIN_1
#define LED_G_GPIO_Port GPIOA
#define LED_B_Pin GPIO_PIN_2
#define LED_B_GPIO_Port GPIOA
#define SPI_NINA_CS_UCI_Pin GPIO_PIN_4
#define SPI_NINA_CS_UCI_GPIO_Port GPIOA
#define SPI_NINA_SCK_Pin GPIO_PIN_5
#define SPI_NINA_SCK_GPIO_Port GPIOA
#define SPI_NINA_MISO_Pin GPIO_PIN_6
#define SPI_NINA_MISO_GPIO_Port GPIOA
#define SPI_NINA_MOSI_Pin GPIO_PIN_7
#define SPI_NINA_MOSI_GPIO_Port GPIOA
#define KL15_ADC_Pin GPIO_PIN_4
#define KL15_ADC_GPIO_Port GPIOC
#define KL30_ADC_Pin GPIO_PIN_5
#define KL30_ADC_GPIO_Port GPIOC
#define MOD1_INT_Pin GPIO_PIN_0
#define MOD1_INT_GPIO_Port GPIOB
#define MOD1_INT_EXTI_IRQn EXTI0_IRQn
#define MOD2_INT_Pin GPIO_PIN_1
#define MOD2_INT_GPIO_Port GPIOB
#define MOD2_INT_EXTI_IRQn EXTI1_IRQn
#define KL15_CONTROLLER_UCO_Pin GPIO_PIN_7
#define KL15_CONTROLLER_UCO_GPIO_Port GPIOE
#define SPI_MOD1_CS_Pin GPIO_PIN_8
#define SPI_MOD1_CS_GPIO_Port GPIOE
#define SPI_MOD2_CS_Pin GPIO_PIN_9
#define SPI_MOD2_CS_GPIO_Port GPIOE
#define MOD_SYNC_CLOCK_Pin GPIO_PIN_10
#define MOD_SYNC_CLOCK_GPIO_Port GPIOE
#define MOD2_RESET_Pin GPIO_PIN_11
#define MOD2_RESET_GPIO_Port GPIOE
#define SPI_MOD_SCK_Pin GPIO_PIN_12
#define SPI_MOD_SCK_GPIO_Port GPIOE
#define SPI_MOD_MISO_Pin GPIO_PIN_13
#define SPI_MOD_MISO_GPIO_Port GPIOE
#define SPI_MOD_MOSI_Pin GPIO_PIN_14
#define SPI_MOD_MOSI_GPIO_Port GPIOE
#define MOD1_RESET_Pin GPIO_PIN_15
#define MOD1_RESET_GPIO_Port GPIOE
#define I2C_ACC_SCL_Pin GPIO_PIN_10
#define I2C_ACC_SCL_GPIO_Port GPIOB
#define I2C_ACC_SDA_Pin GPIO_PIN_11
#define I2C_ACC_SDA_GPIO_Port GPIOB
#define CAN2_RX_UCI_Pin GPIO_PIN_12
#define CAN2_RX_UCI_GPIO_Port GPIOB
#define CAN2_TX_UCO_Pin GPIO_PIN_13
#define CAN2_TX_UCO_GPIO_Port GPIOB
#define USB_PCIE_DM_Pin GPIO_PIN_14
#define USB_PCIE_DM_GPIO_Port GPIOB
#define USB_PCIE_DP_Pin GPIO_PIN_15
#define USB_PCIE_DP_GPIO_Port GPIOB
#define UART_PCIE_TX_UCO_Pin GPIO_PIN_8
#define UART_PCIE_TX_UCO_GPIO_Port GPIOD
#define UART_PCIE_RX_UCI_Pin GPIO_PIN_9
#define UART_PCIE_RX_UCI_GPIO_Port GPIOD
#define CAN2_SILENT_UCO_Pin GPIO_PIN_10
#define CAN2_SILENT_UCO_GPIO_Port GPIOD
#define UART_PCIE_CTS_UCI_Pin GPIO_PIN_11
#define UART_PCIE_CTS_UCI_GPIO_Port GPIOD
#define UART_PCIE_RTS_UCO_Pin GPIO_PIN_12
#define UART_PCIE_RTS_UCO_GPIO_Port GPIOD
#define USB_PCIE_VBUS_EN_Pin GPIO_PIN_13
#define USB_PCIE_VBUS_EN_GPIO_Port GPIOD
#define PCIE_ENABLE_Pin GPIO_PIN_14
#define PCIE_ENABLE_GPIO_Port GPIOD
#define PCIE_RESET_Pin GPIO_PIN_15
#define PCIE_RESET_GPIO_Port GPIOD
#define PGOOD_5V_Pin GPIO_PIN_6
#define PGOOD_5V_GPIO_Port GPIOC
#define PGOOD_PCIE_Pin GPIO_PIN_7
#define PGOOD_PCIE_GPIO_Port GPIOC
#define I2C_PCIE_SDA_Pin GPIO_PIN_9
#define I2C_PCIE_SDA_GPIO_Port GPIOC
#define I2C_PCIE_SCL_Pin GPIO_PIN_8
#define I2C_PCIE_SCL_GPIO_Port GPIOA
#define UART_NINA_TX_UCO_Pin GPIO_PIN_9
#define UART_NINA_TX_UCO_GPIO_Port GPIOA
#define UART_NINA_RX_UCI_Pin GPIO_PIN_10
#define UART_NINA_RX_UCI_GPIO_Port GPIOA
#define USB_NINA_DM_Pin GPIO_PIN_11
#define USB_NINA_DM_GPIO_Port GPIOA
#define USB_NINA_DP_Pin GPIO_PIN_12
#define USB_NINA_DP_GPIO_Port GPIOA
#define SPI_PCIE_SCK_Pin GPIO_PIN_10
#define SPI_PCIE_SCK_GPIO_Port GPIOC
#define SPI_PCIE_MISO_Pin GPIO_PIN_11
#define SPI_PCIE_MISO_GPIO_Port GPIOC
#define SPI_PCIE_MOSI_Pin GPIO_PIN_12
#define SPI_PCIE_MOSI_GPIO_Port GPIOC
#define SPI_PCIE_CS_Pin GPIO_PIN_0
#define SPI_PCIE_CS_GPIO_Port GPIOD
#define CAN1_TX_UCO_Pin GPIO_PIN_1
#define CAN1_TX_UCO_GPIO_Port GPIOD
#define CAN1_SILENT_UCO_Pin GPIO_PIN_2
#define CAN1_SILENT_UCO_GPIO_Port GPIOD
#define POWER_EN_PCIE_Pin GPIO_PIN_3
#define POWER_EN_PCIE_GPIO_Port GPIOD
#define USB_NINA_VBUS_EN_Pin GPIO_PIN_4
#define USB_NINA_VBUS_EN_GPIO_Port GPIOD
#define INT1_ACC_Pin GPIO_PIN_6
#define INT1_ACC_GPIO_Port GPIOD
#define INT1_ACC_EXTI_IRQn EXTI9_5_IRQn
#define INT2_ACC_Pin GPIO_PIN_7
#define INT2_ACC_GPIO_Port GPIOD
#define INT2_ACC_EXTI_IRQn EXTI9_5_IRQn
#define I2C_MOD_SMBA_Pin GPIO_PIN_5
#define I2C_MOD_SMBA_GPIO_Port GPIOB
#define I2C_MOD_SCL_Pin GPIO_PIN_6
#define I2C_MOD_SCL_GPIO_Port GPIOB
#define I2C_MOD_SDA_Pin GPIO_PIN_7
#define I2C_MOD_SDA_GPIO_Port GPIOB
#define CAN1_RX_UCI_Pin GPIO_PIN_8
#define CAN1_RX_UCI_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
