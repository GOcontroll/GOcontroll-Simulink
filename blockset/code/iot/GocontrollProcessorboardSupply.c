/**************************************************************************************
 * \file         GocontrollProcessorboardSupply.c
 * \brief        Functions to read the ADC from the GOcontroll Moduline.
 * \internal
 *----------------------------------------------------------------------------------------
 *                          C O P Y R I G H T
 *----------------------------------------------------------------------------------------
 *  Copyright 2024 (c)  by GOcontroll   http://www.gocontroll.com      All
 *rights reserved
 *
 *----------------------------------------------------------------------------------------
 *                            L I C E N S E
 *----------------------------------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE. endinternal
 *
 ****************************************************************************************/

/****************************************************************************************
 * Include files
 ****************************************************************************************/

#include "GocontrollProcessorboardSupply.h"

#include "GocontrollProcessorboard.h"

/****************************************************************************************
 * Data declarations to store supply voltages
 ****************************************************************************************/

_controllerSupply controllerSupply;

extern _hardwareConfig hardwareConfig;

/****************************************************************************************
 * Function prototypes
 ****************************************************************************************/

// init in tcl?
// HAL_ADC_Start() periodically measure the inputs?

// not needed?
int GocontrollProcessorboardSupply_Voltage(uint8_t supply, uint16_t *value) {
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

int GocontrollProcessorboardSupply_ReadAdc(uint8_t supply, uint16_t *value) {
	// KL15 PC4 KL30 PC5
	// HAL_ADC_GetValue()
	return 0;
}

/****************************************************************************************/
// not needed?
void *GocontrollProcessorboardSupply_ReadAdcThread(void *arg) {}