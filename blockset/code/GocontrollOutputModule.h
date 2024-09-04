/**************************************************************************************
 * \file         GOcontrollOutputModule.h
 * \brief        Utility functions to interface the GOcontroll output module.
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
 *SOFTWARE. \endinternal
 ****************************************************************************************/
#ifndef GOCONTROLL_OUTPUT_MODULE_H_
#define GOCONTROLL_OUTPUT_MODULE_H_

/****************************************************************************************
 * Include files
 ****************************************************************************************/
#include "GocontrollProcessorboard.h"
#include "stdio.h"

/****************************************************************************************
 * Module specific defines
 ****************************************************************************************/
#define OUTPUTMODULE6CHANNEL (1)
#define OUTPUTMODULE10CHANNEL (2)

#define OUTPUTFUNC_DISABLED 1  // channel is unused

#define OUTPUTFUNC_6CH_HALFBRIDGE 2  // half bridge duty cycle controlled
#define OUTPUTFUNC_6CH_LOWSIDEDUTY \
  3  // low side switching duty cycle
     // controlled
#define OUTPUTFUNC_6CH_HIGHSIDEDUTY \
  4  // high side switching duty cycle controlled
#define OUTPUTFUNC_6CH_LOWSIDEBOOL 5   // low side switching on or off (0-1)
#define OUTPUTFUNC_6CH_HIGHSIDEBOOL 6  // high side switching on or off (0-1)
#define OUTPUTFUNC_6CH_PEAKANDHOLD \
  7  // peak and hold function (uses half bridge switching)
#define OUTPUTFUNC_6CH_FREQUENCYOUT 8  // frequency output (0-500Hz)

#define OUTPUTFUNC_10CH_HIGHSIDEDUTY \
  2  // high side switching duty cycle controlled
#define OUTPUTFUNC_10CH_HIGHSIDEBOOL 3  // high side switching on or off (0-1)
#define OUTPUTFUNC_10CH_PEAKANDHOLD \
  4  // peak and hold function (uses high side switching)
#define OUTPUTFUNC_10CH_FREQUENCYOUT 5  // frequency output (0-500Hz)

#define OUTPUTFREQ_100HZ 1      // duty cycle frequency
#define OUTPUTFREQ_200HZ 2      // duty cycle frequency
#define OUTPUTFREQ_500HZ 3      // duty cycle frequency
#define OUTPUTFREQ_1KHZ 4       // duty cycle frequency
#define OUTPUTFREQ_6CH_2KHZ 5   // duty cycle frequency (6 channel module only)
#define OUTPUTFREQ_6CH_5KHZ 6   // duty cycle frequency (6 channel module only)
#define OUTPUTFREQ_6CH_10KHZ 7  // duty cycle frequency (6 channel module only)

#define PEAKCURRENTMAX 3500  // max allowed duty cycle
#define CURRENTMAXMAX 4000   // max allowed current through a channel

#define OUTPUTFREQCHANNEL1AND2 \
  0  // macro for selecting the frequency channels on the output modules
#define OUTPUTFREQCHANNEL3AND4 \
  1  // macro for selecting the frequency channels on the output modules
#define OUTPUTFREQCHANNEL5AND6 \
  2  // macro for selecting the frequency channels on the output modules
#define OUTPUTFREQCHANNEL7AND8_10CH \
  3  // macro for selecting the frequency channels on the output modules (10
     // channel module only)
#define OUTPUTFREQCHANNEL9AND10_10CH \
  4  // macro for selecting the frequency channels on the output modules (10
     // channel module only)

#define OUTPUTCHANNEL1 0
#define OUTPUTCHANNEL2 1
#define OUTPUTCHANNEL3 2
#define OUTPUTCHANNEL4 3
#define OUTPUTCHANNEL5 4
#define OUTPUTCHANNEL6 5
#define OUTPUTCHANNEL7 6
#define OUTPUTCHANNEL8 7
#define OUTPUTCHANNEL9 8
#define OUTPUTCHANNEL10 9

#define OUTPUTFUNCMASK 0b11110000
#define OUTPUTFREQMASK 0b00001111

/****************************************************************************************
 * Data declarations
 ****************************************************************************************/
typedef struct {
  uint8_t configuration[10];
  uint16_t value[10];
  uint16_t dutyCycle[10];
  uint32_t syncCounter[6];
  int16_t current[6];
  uint16_t currentMax[6];
  uint16_t peakCurrent[10];
  uint16_t peakTime[10];
  int16_t temperature;
  int16_t ground;
  uint16_t supply;
  uint16_t totalCurrent;
  uint16_t inputVoltage;
  uint32_t errorCode;
  uint8_t communicationCheck;
  uint32_t moduleIdentifier;
  uint8_t moduleType;
  uint8_t moduleSlot;
  uint32_t sw_version;
} _outputModule;

/****************************************************************************************
 * Function prototypes
 ****************************************************************************************/

/***************************************************************************************
** \brief	Function that sends the configuration data to the output module
** \param	outputModule pointer to a _outputModule struct that holds the
*data for the module configuration
** \return	None
****************************************************************************************/
void OutputModule_Configuration(_outputModule *outputModule);

/***************************************************************************************
** \brief	Function that sends the output data to the output module
** \param	outputModule pointer to a _outputModule struct that holds the
*data for the module configuration
** \return	None
****************************************************************************************/
void OutputModule_SendValues(_outputModule *outputModule);

/***************************************************************************************
** \brief	Function that sets the module slot for an output module
** \param	outputModule pointer to a _outputModule struct that holds the
*data for the module configuration
** \param	moduleSlot the slot that the module is inserted in
** \return	None
****************************************************************************************/
void OutputModule_SetModuleSlot(_outputModule *outputModule,
                                uint8_t moduleSlot);

/***************************************************************************************
** \brief	Function that sets the module type, 6 or 10 channel
** \param	outputModule pointer to a _outputModule struct that holds the
*data for the module configuration
** \param	moduleType the type of module, either outputModule6CHANNEL or
*outputModule10CHANNEL
** \return	None
****************************************************************************************/
void OutputModule_SetModuleType(_outputModule *outputModule,
                                uint8_t moduleType);

/***************************************************************************************
** \brief	Function that Configures an input channel on a 6 channel module
** \param	outputModule pointer to a _outputModule struct that holds the
*data for the module configuration
** \param	channel The channel to be configured
** \param 	func the function of the channel, value from 1-8, use macro
*INPUTFUNC*
** \param 	pull_up pull up resistor for channel use macros related to
*module type INPUTPULLUP*
** \param 	pull_down pull down resistor for channel use macros related to
*module type INPUTPULLDOWN*
** \param 	pulses_per_rotation pulse per rotation for rotary encoders range
*from 0-200
** \param 	analog_filter_samples analog filter samples per measurement rang
*from 0 -1000
** \return	none
****************************************************************************************/
void OutputModule6ch_ConfigureChannel(_outputModule *outputModule,
                                      uint8_t channel, uint8_t func,
                                      uint16_t currentMax,
                                      uint16_t peak_current,
                                      uint16_t peak_time);

/***************************************************************************************
** \brief	Function that Configures an input channel on a 10 channel module
** \param	outputModule pointer to a _outputModule struct that holds the
*data for the module configuration
** \param	channel The channel to be configured
** \param 	func the function of the channel, value from 1-8, use macro
*INPUTFUNC*
** \param 	pull_up pull up resistor for channel use macros related to
*module type INPUTPULLUP*
** \param 	pull_down pull down resistor for channel use macros related to
*module type INPUTPULLDOWN*
** \return	none
****************************************************************************************/
void OutputModule10ch_ConfigureChannel(_outputModule *outputModule,
                                       uint8_t channel, uint8_t func,
                                       uint16_t peak_current,
                                       uint16_t peak_time);

/***************************************************************************************
** \brief	Function that initializes sensor supplies on the module for 6
*channel modules
** \param	outputModule pointer to a _outputModule struct that holds the
*data for the module configuration
** \param	channel the frequency channel to be configured
** \param	frequency the frequency at which this channel should put out its
*PWM signal
** \return	None
****************************************************************************************/
void OutputModule_ConfigureFrequency(_outputModule *outputModule,
                                     uint8_t channel, uint8_t frequency);

#endif /* GOCONTROLL_OUTPUT_MODULE_H_ */
