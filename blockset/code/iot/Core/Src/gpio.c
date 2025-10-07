/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NINA_RESET_GPIO_Port, NINA_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, KL15_CONTROLLER_UCO_Pin|SPI_MOD1_CS_Pin|SPI_MOD2_CS_Pin|MOD2_RESET_Pin
                          |MOD1_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, CAN2_SILENT_UCO_Pin|USB_PCIE_VBUS_EN_Pin|PCIE_ENABLE_Pin|PCIE_RESET_Pin
                          |SPI_PCIE_CS_Pin|CAN1_SILENT_UCO_Pin|POWER_EN_PCIE_Pin|USB_NINA_VBUS_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : NINA_RESET_Pin */
  GPIO_InitStruct.Pin = NINA_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(NINA_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MOD1_INT_Pin MOD2_INT_Pin */
  GPIO_InitStruct.Pin = MOD1_INT_Pin|MOD2_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : KL15_CONTROLLER_UCO_Pin SPI_MOD1_CS_Pin SPI_MOD2_CS_Pin MOD2_RESET_Pin
                           MOD1_RESET_Pin */
  GPIO_InitStruct.Pin = KL15_CONTROLLER_UCO_Pin|SPI_MOD1_CS_Pin|SPI_MOD2_CS_Pin|MOD2_RESET_Pin
                          |MOD1_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : CAN2_SILENT_UCO_Pin USB_PCIE_VBUS_EN_Pin PCIE_ENABLE_Pin PCIE_RESET_Pin
                           SPI_PCIE_CS_Pin CAN1_SILENT_UCO_Pin POWER_EN_PCIE_Pin USB_NINA_VBUS_EN_Pin */
  GPIO_InitStruct.Pin = CAN2_SILENT_UCO_Pin|USB_PCIE_VBUS_EN_Pin|PCIE_ENABLE_Pin|PCIE_RESET_Pin
                          |SPI_PCIE_CS_Pin|CAN1_SILENT_UCO_Pin|POWER_EN_PCIE_Pin|USB_NINA_VBUS_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PGOOD_5V_Pin PGOOD_PCIE_Pin */
  GPIO_InitStruct.Pin = PGOOD_5V_Pin|PGOOD_PCIE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : INT1_ACC_Pin INT2_ACC_Pin */
  GPIO_InitStruct.Pin = INT1_ACC_Pin|INT2_ACC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
