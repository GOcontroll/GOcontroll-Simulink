/**************************************************************************************
 * \file   GOcontrollStatusLeds.h
 * \brief  Platform-independent interface for GOcontroll status LED control.
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

#ifndef GOCONTROLL_STATUS_LEDS_H
#define GOCONTROLL_STATUS_LEDS_H

#include "GocontrollProcessorboard.h"

/****************************************************************************************
** \brief     Initialize the status LEDs.
**            STM32H5: starts TIM3 PWM channels 1-3 (PC6=Blue, PC7=Red, PC8=Green).
**            Linux:   initializes RUKR I2C LED controller if present.
** \return    0 on success, -1 on failure
****************************************************************************************/
int GOcontrollStatusLeds_Initialize(void);

/****************************************************************************************
** \brief     Control a single status LED color channel.
** \param     led    LED index (STM32H5: always 1 / Linux: 1-4)
** \param     color  LED color channel (_ledColor enum: RED, GREEN, BLUE)
** \param     value  Brightness (STM32H5: 0-255 PWM / Linux: 0=off, non-zero=on)
** \return    0 on success, -1 on failure
****************************************************************************************/
int GOcontrollStatusLeds_LedControl(uint8_t led, _ledColor color, uint8_t value);

#endif /* GOCONTROLL_STATUS_LEDS_H */
