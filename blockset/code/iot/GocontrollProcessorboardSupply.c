/**************************************************************************************
 * \file         GocontrollProcessorboardSupply.c
 * \brief        Functions to read the ADC from the GOcontroll Moduline.
 * \internal
 *----------------------------------------------------------------------------------------
 *                          C O P Y R I G H T
 *----------------------------------------------------------------------------------------
 * Copyright 2024 (c) by GOcontroll http://www.gocontroll.com All rights
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
 * SOFTWARE. endinternal
 *
 ****************************************************************************************/

/****************************************************************************************
 * Include files
 ****************************************************************************************/

#include "GocontrollProcessorboardSupply.h"

#include "adc.h"
#include "cmsis_os.h"

/****************************************************************************************
 * Data declarations to store supply voltages
 ****************************************************************************************/

_controllerSupply controllerSupply;

uint32_t channels[2] = {ADC_CHANNEL_15, ADC_CHANNEL_14};

/****************************************************************************************
 * Function prototypes
 ****************************************************************************************/

int GocontrollProcessorboardSupply_Voltage(uint8_t supply, uint16_t* value) {
	switch (supply) {
		case 1: {
			*value = controllerSupply.batteryVoltage;
			break;
		}
		case 2: {
			*value = controllerSupply.k15aVoltage;
			break;
		}
		default: {
			return -1;
		}
	}

	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboardSupply_ReadAdc(uint8_t supply, uint16_t* value) {
	ADC_ChannelConfTypeDef ADCChannelConfiguration;

	HAL_ADC_Stop(&hadc1);
	ADCChannelConfiguration.Channel = channels[supply];
	ADCChannelConfiguration.Rank = 1;
	ADCChannelConfiguration.SamplingTime = ADC_SAMPLETIME_56CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &ADCChannelConfiguration);
	HAL_ADC_Start(&hadc1);

	if (HAL_ADC_PollForConversion(&hadc1, 500) != HAL_OK) {
		return -1;
	}

	*value =
		(uint16_t)(float)(((HAL_ADC_GetValue(&hadc1) * 0.805) / 1200) * 11400);
	return 0;
}

/****************************************************************************************/

void GocontrollProcessorboardSupply_ReadAdcThread(void* arg) {
	struct ControllerSupplyThreadArgs* args =
		(struct ControllerSupplyThreadArgs*)arg;
	uint32_t tick = osKernelGetTickCount();

	while (args->thread_run) {
		tick += args->sample_time;
		/* Execute actual conversions of the adc */
		GocontrollProcessorboardSupply_ReadAdc(
			0, &controllerSupply.batteryVoltage);
		GocontrollProcessorboardSupply_ReadAdc(1,
											   &controllerSupply.k15aVoltage);
		osDelayUntil(tick);
	}
	osThreadExit();
}