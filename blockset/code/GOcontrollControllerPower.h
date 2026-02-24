/**************************************************************************************
 * \file   GOcontrollControllerPower.h
 * \brief  Platform-independent interface for GOcontroll controller power
 *         management (supply voltage measurement and active-state control).
 *         Supports STM32H5 (IOT) and Linux (IMX8) platforms via preprocessor
 *         conditional compilation. Define GOCONTROLL_IOT for STM32H5 targets.
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

#ifndef GOCONTROLL_CONTROLLER_POWER_H
#define GOCONTROLL_CONTROLLER_POWER_H

#include <stdint.h>

typedef struct {
	uint16_t batteryVoltage;
	uint16_t k15aVoltage;
	uint16_t k15bVoltage;
	uint16_t k15cVoltage;
} _controllerSupply;

/****************************************************************************************
** \brief     Read a supply voltage from the cached ADC data.
**            The ADC thread must be running (via GOcontrollControllerPower_StartAdcThread)
**            before calling this function.
** \param     supply  Supply index: 1=K30/battery, 2=K15-A, 3=K15-B, 4=K15-C
**                    (STM32H5: only 1 and 2 are valid)
** \param     value   Pointer to variable to store the voltage in mV.
** \return    0 on success, -1 on invalid supply index.
****************************************************************************************/
int GOcontrollControllerPower_Voltage(uint8_t supply, uint16_t* value);

/****************************************************************************************
** \brief     Start the background ADC sampling thread.
**            STM32H5: creates a CMSIS-RTOS osThread.
**            Linux:   creates a POSIX pthread.
** \param     sample_time_ms  Sampling interval in milliseconds.
****************************************************************************************/
void GOcontrollControllerPower_StartAdcThread(uint32_t sample_time_ms);

/****************************************************************************************
** \brief     Stop the background ADC sampling thread.
**            STM32H5: signals the thread to exit.
**            Linux:   signals and joins the thread.
****************************************************************************************/
void GOcontrollControllerPower_StopAdcThread(void);

/****************************************************************************************
** \brief     Control the controller-active relay/output.
**            STM32H5: writes the KL15_CONTROLLER_UCO GPIO pin.
**            Linux:   writes to /sys/class/leds/power-active/brightness.
** \param     state  1 = active (on), 0 = inactive (off).
** \return    0 on success, -1 on failure.
****************************************************************************************/
int GOcontrollControllerPower_ControllerActive(uint8_t state);

#endif /* GOCONTROLL_CONTROLLER_POWER_H */
