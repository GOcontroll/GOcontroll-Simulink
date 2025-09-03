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

void GocontrollProcessorboard_Delay1msPreOs(uint32_t times) {
	return;	 // busy loop here?
}

/****************************************************************************************/

void GocontrollProcessorboard_Delay1ms(uint32_t times) {
	osDelay(times * 1000);	// figure that out
}

/****************************************************************************************/

int GocontrollProcessorboard_SpiDevice(uint8_t moduleSlot) { return 0; }

/****************************************************************************************/

int GocontrollProcessorboard_ModuleReset(uint8_t moduleSlot) { return 0; }

/****************************************************************************************/

int8_t GocontrollProcessorboard_ResetStateModule(uint8_t module,
												 uint8_t state) {
	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboard_LedInitialize(void) { return 0; }

/****************************************************************************************/

int GocontrollProcessorboard_LedControl(uint8_t led, _ledColor color,
										uint8_t value) {
	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboard_EscapeFromBootloader(uint8_t module,
												  uint8_t *dataTx,
												  uint8_t *dataRx) {
	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboard_SendSpi(uint8_t command, uint8_t dataLength,
									 uint8_t id1, uint8_t id2, uint8_t id3,
									 uint8_t id4, uint8_t module,
									 uint8_t *dataTx, uint32_t delay) {
	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboard_SendReceiveSpi(uint8_t command, uint8_t dataLength,
											uint8_t id1, uint8_t id2,
											uint8_t id3, uint8_t id4,
											uint8_t module, uint8_t *dataTx,
											uint8_t *dataRx) {
	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboard_ControllerActive(uint8_t state) { return 0; }

/****************************************************************************************/

float GocontrollProcessorboard_ControllerTemperature(void) { return 0; }

/****************************************************************************************/

void GocontrollProcessorboard_GetHardwareVersion(void) {
	memset(&hardwareConfig, 0, sizeof(_hardwareConfig));

	hardwareConfig.moduleNumber = 2;
	hardwareConfig.ledControl = 0;
	hardwareConfig.adcControl = 0;
}
