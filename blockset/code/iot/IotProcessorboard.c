/**************************************************************************************
 * \file   IotProcessorboard.c
 * \brief  Functions that handels the low level control of the GOcontroll
 * hardware.
 * \internal
 *----------------------------------------------------------------------------------------
 *                          C O P Y R I G H T
 *----------------------------------------------------------------------------------------
 * Copyright 2025 (c) by GOcontroll http://www.gocontroll.com All rights
 * reserved
 *
 *----------------------------------------------------------------------------------------
 *                            L I C E N S E
 *----------------------------------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * \endinternal
 ****************************************************************************************/

/****************************************************************************************
 * Include files
 ****************************************************************************************/

// #include "IotProcessorboard.h"
#include <stdint.h>
#include <string.h>

#include "GocontrollProcessorboard.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "spi.h"
#include "stm32h5xx_hal.h"
#include "tim.h"

#include "SEGGER_RTT.h"

/****************************************************************************************
 * Macro definitions
 ****************************************************************************************/
#define LOW 0
#define HIGH 1

// #define DEBUG 1

#ifndef __min
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#endif	// __min

extern _hardwareConfig hardwareConfig;

/****************************************************************************************/

void GocontrollProcessorboard_Delay1ms(uint32_t times) { HAL_Delay(times); }

/****************************************************************************************/

void GocontrollProcessorboard_Delay1msOS(uint32_t times) { osDelay(times); }

/****************************************************************************************/

int8_t GocontrollProcessorboard_ResetStateModule(uint8_t module,
												 uint8_t state) {
	if (module == 0) {
		HAL_GPIO_WritePin(MOD1_RESET_GPIO_Port, MOD1_RESET_Pin, !state);
	} else if (module == 1) {
		HAL_GPIO_WritePin(MOD2_RESET_GPIO_Port, MOD2_RESET_Pin, !state);
	}
	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboard_LedInitialize(void) {
	
	GocontrollProcessorboard_LedControl(1, LED_COLOR_RED, 0);
	GocontrollProcessorboard_LedControl(1, LED_COLOR_GREEN, 0);
	GocontrollProcessorboard_LedControl(1, LED_COLOR_BLUE, 0);
	/* Initialize the PWM controller */
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboard_LedControl(uint8_t led, _ledColor color,
										uint8_t value) {
	
	if (led != 1) {
		return -1;
	}

	/* H5 IoT: TIM3 CH1=LED_B(PC6), CH2=LED_R(PC7), CH3=LED_G(PC8) */
	switch (color) {
		case LED_COLOR_RED:
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,(uint32_t)value);
			break;
		case LED_COLOR_GREEN:
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,(uint32_t)value);
			break;
		case LED_COLOR_BLUE:
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint32_t)value);
			break;
	}
	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboard_EscapeFromBootloader(uint8_t module,
												  uint8_t* dataTx,
												  uint8_t* dataRx) {
	dataTx[0] = 19;
	dataTx[1] = BOOTMESSAGELENGTH - 1;
	*(uint16_t*)&dataTx[2] = 19;

	dataTx[BOOTMESSAGELENGTH - 1] = GocontrollProcessorboard_CheckSumCalculator(
		&dataTx[0], BOOTMESSAGELENGTH - 1);

	if (module == 0) {
		HAL_GPIO_WritePin(SPI_MOD1_CS_GPIO_Port, SPI_MOD1_CS_Pin,
						  GPIO_PIN_RESET);
	} else if (module == 1) {
		HAL_GPIO_WritePin(SPI_MOD2_CS_GPIO_Port, SPI_MOD2_CS_Pin,
						  GPIO_PIN_RESET);
	}

	HAL_SPI_TransmitReceive(&hspi1, &dataTx[0], &dataRx[0],
							BOOTMESSAGELENGTHCHECK, 500);

	if (module == 0) {
		HAL_GPIO_WritePin(SPI_MOD1_CS_GPIO_Port, SPI_MOD1_CS_Pin, GPIO_PIN_SET);
	} else if (module == 1) {
		HAL_GPIO_WritePin(SPI_MOD2_CS_GPIO_Port, SPI_MOD2_CS_Pin, GPIO_PIN_SET);
	}

	if (dataRx[1] <= BOOTMESSAGELENGTHCHECK) {
		if (GocontrollProcessorboard_CheckSumCalculator(
				&dataRx[0], dataRx[1]) == dataRx[dataRx[1]]) {
			return 0;
		}
	}
	return -1;
}

/****************************************************************************************/

int GocontrollProcessorboard_SendSpi(uint8_t command, uint8_t dataLength,
									 uint8_t id1, uint8_t id2, uint8_t id3,
									 uint8_t id4, uint8_t module,
									 uint8_t* dataTx, uint32_t delay) {
	dataTx[0] = command;
	dataTx[1] = dataLength - 1;
	dataTx[2] = id1;
	dataTx[3] = id2;
	dataTx[4] = id3;
	dataTx[5] = id4;

	dataTx[dataLength - 1] =
		GocontrollProcessorboard_CheckSumCalculator(&dataTx[0], dataLength - 1);

	if (module == 0) {
		HAL_GPIO_WritePin(SPI_MOD1_CS_GPIO_Port, SPI_MOD1_CS_Pin,
						  GPIO_PIN_RESET);
	} else if (module == 1) {
		HAL_GPIO_WritePin(SPI_MOD2_CS_GPIO_Port, SPI_MOD2_CS_Pin,
						  GPIO_PIN_RESET);
	}
	// round delay in us up to ms
	GocontrollProcessorboard_Delay1ms(delay / 1000 + (delay % 1000 != 0));
	HAL_SPI_Transmit(&hspi1, &dataTx[0], dataLength + MESSAGEOVERLENGTH, 500);

	if (module == 0) {
		HAL_GPIO_WritePin(SPI_MOD1_CS_GPIO_Port, SPI_MOD1_CS_Pin, GPIO_PIN_SET);
	} else if (module == 1) {
		HAL_GPIO_WritePin(SPI_MOD2_CS_GPIO_Port, SPI_MOD2_CS_Pin, GPIO_PIN_SET);
	}
	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboard_SendReceiveSpi(uint8_t command, uint8_t dataLength,
											uint8_t id1, uint8_t id2,
											uint8_t id3, uint8_t id4,
											uint8_t module, uint8_t* dataTx,
											uint8_t* dataRx) {
	dataTx[0] = command;
	dataTx[1] = dataLength - 1;
	dataTx[2] = id1;
	dataTx[3] = id2;
	dataTx[4] = id3;
	dataTx[5] = id4;

	dataTx[dataLength - 1] =
		GocontrollProcessorboard_CheckSumCalculator(&dataTx[0], dataLength - 1);

	if (module == 0) {
		HAL_GPIO_WritePin(SPI_MOD1_CS_GPIO_Port, SPI_MOD1_CS_Pin,
						  GPIO_PIN_RESET);
	} else if (module == 1) {
		HAL_GPIO_WritePin(SPI_MOD2_CS_GPIO_Port, SPI_MOD2_CS_Pin,
						  GPIO_PIN_RESET);
	}

	HAL_SPI_TransmitReceive(&hspi1, &dataTx[0], &dataRx[0],
							dataLength + MESSAGEOVERLENGTH, 500);

	if (module == 0) {
		HAL_GPIO_WritePin(SPI_MOD1_CS_GPIO_Port, SPI_MOD1_CS_Pin, GPIO_PIN_SET);
	} else if (module == 1) {
		HAL_GPIO_WritePin(SPI_MOD2_CS_GPIO_Port, SPI_MOD2_CS_Pin, GPIO_PIN_SET);
	}

	if (dataRx[1] == dataLength - 1) {
		if (GocontrollProcessorboard_CheckSumCalculator(
				&dataRx[0], dataLength - 1) == dataRx[dataLength - 1]) {
			return 0;
		}
	}
	return -1;
}

/****************************************************************************************/

int GocontrollProcessorboard_ControllerActive(uint8_t state) {
	HAL_GPIO_WritePin(KL15_CONTROLLER_UCO_GPIO_Port, KL15_CONTROLLER_UCO_Pin,
					  (GPIO_PinState)state);
	return 0;
}

/****************************************************************************************/

float GocontrollProcessorboard_ControllerTemperature(void) { return 0; }

/****************************************************************************************/

void GocontrollProcessorboard_GetHardwareVersion(void) {
	memset(&hardwareConfig, 0, sizeof(_hardwareConfig));

	hardwareConfig.moduleNumber = 2;
	hardwareConfig.ledControl = 0;
	hardwareConfig.adcControl = 0;
}
