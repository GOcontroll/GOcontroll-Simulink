/**************************************************************************************
 * \file   GOcontrollStatusLeds.c
 * \brief  Status LED control for GOcontroll platforms.
 *
 *         Platform selection via preprocessor define:
 *           GOCONTROLL_IOT  →  STM32H5 (Moduline IOT)
 *           (default)       →  Linux/IMX8 (Moduline IV / Moduline Mini)
 *
 *         STM32H5 hardware mapping (TIM3 PWM):
 *           TIM3_CH1 = PC6 = Blue
 *           TIM3_CH2 = PC7 = Red
 *           TIM3_CH3 = PC8 = Green
 *           Brightness range: 0-255
 *
 *         Linux hardware mapping:
 *           LED_RUKR : I2C RUKR controller at /dev/i2c-2, address 0x14
 *           LED_GPIO : sysfs /sys/class/leds/Status<n>-<r|g|b>/brightness
 *           Brightness: 0=off, non-zero=on
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

#include "GOcontrollStatusLeds.h"

/****************************************************************************************
 * STM32H5 (Moduline IOT) implementation
 ****************************************************************************************/
#ifdef GOCONTROLL_IOT

#include "stm32h5xx_hal.h"
#include "tim.h"

int GOcontrollStatusLeds_Initialize(void) {
	/* Reset all channels before starting PWM */
	GOcontrollStatusLeds_LedControl(1, LED_COLOR_RED,   0);
	GOcontrollStatusLeds_LedControl(1, LED_COLOR_GREEN, 0);
	GOcontrollStatusLeds_LedControl(1, LED_COLOR_BLUE,  0);
	/* Start PWM on TIM3: CH1=Blue(PC6), CH2=Red(PC7), CH3=Green(PC8) */
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	return 0;
}

int GOcontrollStatusLeds_LedControl(uint8_t led, _ledColor color, uint8_t value) {
	if (led != 1) {
		return -1;
	}
	switch (color) {
		case LED_COLOR_RED:
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (uint32_t)value);
			break;
		case LED_COLOR_GREEN:
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, (uint32_t)value);
			break;
		case LED_COLOR_BLUE:
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint32_t)value);
			break;
	}
	return 0;
}

/****************************************************************************************
 * Linux/IMX8 (Moduline IV / Moduline Mini) implementation
 ****************************************************************************************/
#else

#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>

#define LOW 0

extern _hardwareConfig hardwareConfig;

int GOcontrollStatusLeds_Initialize(void) {
	if (hardwareConfig.ledControl == LED_RUKR) {
		const uint8_t addr = 0x14;
		uint8_t dataTx[2];
		static int i2cDevice = 0;

		if ((i2cDevice = open("/dev/i2c-2", O_RDWR)) < 0) {
			close(i2cDevice);
			printf("Error I2C open for LED's.\n");
			return -1;
		}
		if (ioctl(i2cDevice, I2C_SLAVE, addr) < 0) {
			close(i2cDevice);
			printf("Error I2C require bus for LED's.\n");
			return -1;
		}
		/* Reset the RUKR controller */
		dataTx[0] = 0x17;
		dataTx[1] = 0xFF;
		if (write(i2cDevice, dataTx, 2) != 2) {
			close(i2cDevice);
			printf("Error I2C write to bus for LED's.\n");
			return -1;
		}
		/* Enable the RUKR controller (Chip_EN) */
		dataTx[0] = 0x00;
		dataTx[1] = 0x40;
		if (write(i2cDevice, dataTx, 2) != 2) {
			close(i2cDevice);
			printf("Error I2C write to bus for LED's.\n");
			return -1;
		}
		close(i2cDevice);
	}
	return 0;
}

int GOcontrollStatusLeds_LedControl(uint8_t led, _ledColor color, uint8_t value) {
	if (hardwareConfig.ledControl == LED_RUKR) {
		/* RUKR I2C register map: LED1=0x0A, LED2=0x0D, LED3=0x10, LED4=0x13 (+color offset) */
		static uint8_t dataTx[3] = {0};
		const int addr = 0x14;
		static int i2cDevice = 0;

		if      (led == 1) dataTx[0] = 0x0A + color;
		else if (led == 2) dataTx[0] = 0x0D + color;
		else if (led == 3) dataTx[0] = 0x10 + color;
		else if (led == 4) dataTx[0] = 0x13 + color;
		else               return -1;

		dataTx[1] = value;

		if ((i2cDevice = open("/dev/i2c-2", O_RDWR)) < 0) {
			close(i2cDevice);
			printf("Error I2C open for LED's.\n");
			return -1;
		}
		if (ioctl(i2cDevice, I2C_SLAVE, addr) < 0) {
			close(i2cDevice);
			printf("Error I2C require bus for LED's.\n");
			return -1;
		}
		if (write(i2cDevice, dataTx, 2) != 2) {
			close(i2cDevice);
			printf("Error I2C write to bus for LED's.\n");
			return -1;
		}
		close(i2cDevice);
		return 0;

	} else if (hardwareConfig.ledControl == LED_GPIO) {
		/* sysfs LED path: /sys/class/leds/Status<led>-<r|g|b>/brightness */
		char path[40];
		switch (color) {
			case LED_COLOR_RED:
				snprintf(path, 40, "/sys/class/leds/Status%d-r/brightness", led);
				break;
			case LED_COLOR_GREEN:
				snprintf(path, 40, "/sys/class/leds/Status%d-g/brightness", led);
				break;
			case LED_COLOR_BLUE:
				snprintf(path, 40, "/sys/class/leds/Status%d-b/brightness", led);
				break;
		}

		static const char s_values_str[] = "01";
		int ledControl = open(path, O_WRONLY);
		if (ledControl == -1) {
			fprintf(stderr, "Error GPIO write led %d!\n", led);
			close(ledControl);
			return -1;
		}
		if (1 != write(ledControl, &s_values_str[LOW == value ? 0 : 1], 1)) {
			fprintf(stderr, "Error GPIO write led %d!\n", led);
			close(ledControl);
			return -1;
		}
		close(ledControl);

	} else if (hardwareConfig.ledControl == NOT_INSTALLED) {
		return 0;
	}
	return 0;
}

#endif /* GOCONTROLL_IOT */
