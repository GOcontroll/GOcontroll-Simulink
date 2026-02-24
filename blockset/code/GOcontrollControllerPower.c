/**************************************************************************************
 * \file   GOcontrollControllerPower.c
 * \brief  Controller power management for GOcontroll platforms.
 *
 *         Platform selection via preprocessor define:
 *           GOCONTROLL_IOT  →  STM32H5 (Moduline IOT)
 *           (default)       →  Linux/IMX8 (Moduline IV / Moduline Mini)
 *
 *         STM32H5 hardware mapping (ADC1):
 *           ADC1_CH9  (index 0) = K30/battery voltage
 *           ADC1_CH10 (index 1) = K15-A voltage
 *           Voltage range: 0-65535 mV (uint16_t)
 *
 *         Linux hardware mapping:
 *           ADC_MCP3004 : IIO driver (mcp3004), 4 channels
 *           ADC_ADS1015 : I2C ADS1015 at /dev/i2c-2, address 0x48
 *           Voltage range: 0-65535 mV (uint16_t)
 *
 *         Controller-active output:
 *           STM32H5 : KL15_CONTROLLER_UCO GPIO
 *           Linux   : /sys/class/leds/power-active/brightness
 * \internal
 *----------------------------------------------------------------------------------------
 *                          C O P Y R I G H T
 *----------------------------------------------------------------------------------------
 * Copyright 2025 (c) by GOcontroll http://www.gocontroll.com All rights reserved
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

#include "GOcontrollControllerPower.h"

/****************************************************************************************
 * STM32H5 (Moduline IOT) implementation
 ****************************************************************************************/
#ifdef GOCONTROLL_IOT

#include "adc.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "SEGGER_RTT.h"

/* ADC channel mapping: index 0 = K30/battery (CH9), index 1 = K15-A (CH10) */
static uint32_t s_channels[2] = {ADC_CHANNEL_9, ADC_CHANNEL_10};

static _controllerSupply s_controllerSupply;

static struct {
	uint8_t  thread_run;
	uint32_t sample_time;
} s_adcThreadArgs;

static osThreadId_t s_adcThreadId;

static int readAdc(uint8_t index, uint16_t* value) {
	ADC_ChannelConfTypeDef ADCChannelConfiguration = {0};

	HAL_ADC_Stop(&hadc1);
	ADCChannelConfiguration.Channel     = s_channels[index];
	ADCChannelConfiguration.Rank        = ADC_REGULAR_RANK_1;
	ADCChannelConfiguration.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
	ADCChannelConfiguration.SingleDiff  = ADC_SINGLE_ENDED;
	ADCChannelConfiguration.OffsetNumber = ADC_OFFSET_NONE;
	ADCChannelConfiguration.Offset       = 0u;

	HAL_ADC_ConfigChannel(&hadc1, &ADCChannelConfiguration);
	HAL_ADC_Start(&hadc1);

	if (HAL_ADC_PollForConversion(&hadc1, 500) != HAL_OK) {
		return -1;
	}

	*value = (uint16_t)(float)(((HAL_ADC_GetValue(&hadc1) * 0.805) / 1200) * 11400);
	return 0;
}

static void adcThreadFunc(void* arg) {
	(void)arg;
	uint32_t tick = osKernelGetTickCount();

	while (s_adcThreadArgs.thread_run) {
		tick += s_adcThreadArgs.sample_time;
		readAdc(0, &s_controllerSupply.batteryVoltage);
		readAdc(1, &s_controllerSupply.k15aVoltage);
		osDelayUntil(tick);
	}
	osThreadExit();
}

int GOcontrollControllerPower_Voltage(uint8_t supply, uint16_t* value) {
	switch (supply) {
		case 1:
			SEGGER_RTT_printf(0, "K30: %d\n", s_controllerSupply.batteryVoltage);
			*value = s_controllerSupply.batteryVoltage;
			break;
		case 2:
			SEGGER_RTT_printf(0, "K15: %d\n", s_controllerSupply.k15aVoltage);
			*value = s_controllerSupply.k15aVoltage;
			break;
		default:
			return -1;
	}
	return 0;
}

void GOcontrollControllerPower_StartAdcThread(uint32_t sample_time_ms) {
	static const osThreadAttr_t adc_thread_attributes = {
		.name       = "adc_thread",
		.stack_size = 128 * 4,
		.priority   = (osPriority_t) osPriorityNormal,
	};
	s_adcThreadArgs.sample_time = sample_time_ms;
	s_adcThreadArgs.thread_run  = 1;
	s_adcThreadId = osThreadNew(adcThreadFunc, NULL, &adc_thread_attributes);
}

void GOcontrollControllerPower_StopAdcThread(void) {
	s_adcThreadArgs.thread_run = 0;
}

int GOcontrollControllerPower_ControllerActive(uint8_t state) {
	HAL_GPIO_WritePin(KL15_CONTROLLER_UCO_GPIO_Port, KL15_CONTROLLER_UCO_Pin,
	                  (GPIO_PinState)state);
	return 0;
}

/****************************************************************************************
 * Linux/IMX8 (Moduline IV / Moduline Mini) implementation
 ****************************************************************************************/
#else

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>

#include "GocontrollProcessorboard.h"
#include "iio.h"

#define LOW 0

extern _hardwareConfig hardwareConfig;

static _controllerSupply s_controllerSupply;

static struct {
	uint8_t  thread_run;
	uint32_t sample_time;
} s_adcThreadArgs;

static pthread_t s_adcThreadId;

static struct iio_device*  s_iioMCP;
static struct iio_channel* s_adcChannels[4];

static int readAdc(uint8_t supply, uint16_t* value) {
	if (hardwareConfig.adcControl == ADC_MCP3004) {
		char   buf[100];
		ssize_t res;
		size_t  bufSize = sizeof buf;

		switch (supply) {
			case 1: res = iio_channel_attr_read(s_adcChannels[2], "raw", buf, bufSize); break;
			case 2: res = iio_channel_attr_read(s_adcChannels[3], "raw", buf, bufSize); break;
			case 3: res = iio_channel_attr_read(s_adcChannels[0], "raw", buf, bufSize); break;
			case 4: res = iio_channel_attr_read(s_adcChannels[1], "raw", buf, bufSize); break;
			default: return -1;
		}
		(void)res;
		/* 25.54 = ((3.35/1023)/1.5)*11700 */
		*value = (uint16_t)((float)(strtof(buf, NULL) * 25.54));
		return 0;
	}

	if (hardwareConfig.adcControl == ADC_ADS1015) {
		static uint8_t dataTxConfig[3]  = {0};
		static uint8_t dataTxConvert[1] = {0};
		static uint8_t dataRx[2]        = {0};

		static int   i2cDevice    = 0;
		static float decimalFactor = 4.095 / 2047;

		const int addr = 0x48;

		const int convertContact1 = 0xC3;
		const int convertContact2 = 0xD3;
		const int convertContact3 = 0xE3;
		const int convertBattery  = 0xF3;

		dataTxConfig[0] = 0x01;
		dataTxConfig[1] = 0xF3;
		dataTxConfig[2] = 0xE3;

		dataTxConvert[0] = 0x00;

		if      (supply == 1) dataTxConfig[1] = convertBattery;
		else if (supply == 2) dataTxConfig[1] = convertContact1;
		else if (supply == 3) dataTxConfig[1] = convertContact2;
		else if (supply == 4) dataTxConfig[1] = convertContact3;
		else                  return -1;

		if ((i2cDevice = open("/dev/i2c-2", O_RDWR)) < 0) {
			printf("Failed to open the bus for voltage measurement.\n");
			close(i2cDevice);
			return -1;
		}
		if (ioctl(i2cDevice, I2C_SLAVE, addr) < 0) {
			printf("Failed to acquire bus access and/or talk to slave.\n");
			close(i2cDevice);
			return -1;
		}
		if (write(i2cDevice, dataTxConfig, 3) != 3) {
			printf("Failed to write to the i2c bus.\n");
			close(i2cDevice);
			return -1;
		}
		if (write(i2cDevice, dataTxConvert, 1) != 1) {
			printf("Failed to write to the i2c bus.\n");
			close(i2cDevice);
			return -1;
		}
		if (read(i2cDevice, dataRx, 2) != 2) {
			printf("Failed to read from the i2c bus.\n");
			close(i2cDevice);
			return -1;
		}
		close(i2cDevice);

		int16_t valueTemp = (dataRx[0] << 4) | ((dataRx[1] & 0xf0) >> 4);
		if (((dataRx[0] & 0x80) >> 7) == 1) {
			valueTemp = 0;
		}
		*value = (uint16_t)((float)(((valueTemp * decimalFactor) / 1.5) * 11700));
	}

	return 0;
}

static void* adcThreadFunc(void* arg) {
	(void)arg;
	struct iio_context* iioContext = NULL;
	__useconds_t sample_time = (__useconds_t)(s_adcThreadArgs.sample_time * 1000);

	if (hardwareConfig.adcControl == ADC_MCP3004) {
		iioContext = iio_create_local_context();
		uint8_t channel_count = 0;
		s_iioMCP = iio_context_find_device(iioContext, "mcp3004");
		for (uint8_t i = 0; i < iio_device_get_channels_count(s_iioMCP); ++i) {
			struct iio_channel* chn = iio_device_get_channel(s_iioMCP, i);
			if (iio_channel_get_attrs_count(chn) == 2) {
				s_adcChannels[channel_count] = chn;
				channel_count++;
			}
		}
	}

	while (s_adcThreadArgs.thread_run) {
		readAdc(1, &s_controllerSupply.batteryVoltage);
		readAdc(2, &s_controllerSupply.k15aVoltage);
		readAdc(3, &s_controllerSupply.k15bVoltage);
		readAdc(4, &s_controllerSupply.k15cVoltage);
		usleep(sample_time);
	}

	if (iioContext != NULL) {
		iio_context_destroy(iioContext);
	}
	return 0;
}

int GOcontrollControllerPower_Voltage(uint8_t supply, uint16_t* value) {
	switch (supply) {
		case 1: *value = s_controllerSupply.batteryVoltage; break;
		case 2: *value = s_controllerSupply.k15aVoltage;    break;
		case 3: *value = s_controllerSupply.k15bVoltage;    break;
		case 4: *value = s_controllerSupply.k15cVoltage;    break;
		default: return -1;
	}
	return 0;
}

void GOcontrollControllerPower_StartAdcThread(uint32_t sample_time_ms) {
	s_adcThreadArgs.sample_time = sample_time_ms;
	s_adcThreadArgs.thread_run  = 1;
	pthread_create(&s_adcThreadId, NULL, adcThreadFunc, NULL);
}

void GOcontrollControllerPower_StopAdcThread(void) {
	s_adcThreadArgs.thread_run = 0;
	pthread_join(s_adcThreadId, NULL);
}

int GOcontrollControllerPower_ControllerActive(uint8_t state) {
	static int controllerActive = 0;

	if (controllerActive == 0) {
		char path[45];
		snprintf(path, 41, "/sys/class/leds/power-active/brightness");
		controllerActive = open(path, O_WRONLY);

		if (-1 == controllerActive) {
			fprintf(stderr, "Error GPIO write controller active!\n");
			return -1;
		}
	}

	static const char s_values_str[] = "01";

	if (1 != write(controllerActive, &s_values_str[LOW == state ? 0 : 1], 1)) {
		fprintf(stderr, "Error GPIO write controller active!\n");
		return -1;
	}
	return 0;
}

#endif /* GOCONTROLL_IOT */
