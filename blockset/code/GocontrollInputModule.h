/************************************************************************************//**
* \file         GOcontrollInputModule.h
* \brief        Utility functions to interface the GOcontroll input module.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2024 (c)  by GOcontroll   http://www.gocontroll.com      All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* \endinternal
****************************************************************************************/
#ifndef GOCONTROLL_INPUT_MODULE_H_
#define GOCONTROLL_INPUT_MODULE_H_

/****************************************************************************************
* Include files
****************************************************************************************/
#include "GocontrollProcessorboard.h"
#include "stdio.h"

/****************************************************************************************
* Module specific defines
****************************************************************************************/
#define INPUTMODULE6CHANNEL		(1)
#define INPUTMODULE10CHANNEL 	(2)

#define INPUTFUNC_12BITADC      1
#define INPUTFUNC_MVANALOG      2
#define INPUTFUNC_DIGITAL_IN    3
#define INPUTFUNC_FREQUENCY     4
#define INPUTFUNC_DUTY_LOW      5
#define INPUTFUNC_DUTY_HIGH     6
#define INPUTFUNC_RPM           7
#define INPUTFUNC_PULSECOUNTER  8

#define INPUTVOLTAGERANGE_5V    0
#define INPUTVOLTAGERANGE_12V   1
#define INPUTVOLTAGERANGE_24V   2

#define INPUTPULLUP_NULL        0
#define INPUTPULLUP_3_3K        1
#define INPUTPULLUP6CH_4_7K     2
#define INPUTPULLUP6CH_10K      3

#define INPUTPULLDOWN_NULL      0
#define INPUTPULLDOWN_3_3K      1
#define INPUTPULLDOWN6CH_4_7K   2
#define INPUTPULLDOWN6CH_10K    3

#define INPUTSENSSUPPLYON       1
#define INPUTSENSSUPPLYOFF      2

#define PULSESPERROTATIONMAX    200
#define ANALOGSAMPLESMAX        1000

#define INPUTCHANNEL1           0
#define INPUTCHANNEL2           1
#define INPUTCHANNEL3           2
#define INPUTCHANNEL4           3
#define INPUTCHANNEL5           4
#define INPUTCHANNEL6           5
#define INPUTCHANNEL7           6
#define INPUTCHANNEL8           7
#define INPUTCHANNEL9           8
#define INPUTCHANNEL10          9

/****************************************************************************************
* Data declarations
****************************************************************************************/
extern uint8_t resistorMatrix[4];

typedef struct
{
uint8_t configuration[10];
uint8_t interface[10];
uint8_t callibrationValue1[10];
uint8_t callibrationValue2[10];
uint8_t callibrationValue3[6];
uint8_t callibrationValue4[6];
int32_t value[10];
uint32_t syncCounter[6];
uint8_t pulscounterResetTrigger[10];
uint32_t moduleIdentifier;
uint8_t sensorSupply1;
uint8_t sensorSupply2;
uint8_t sensorSupply3;
uint8_t moduleType;
uint8_t moduleSlot;
} _inputModule;

/****************************************************************************************
* Function prototypes
****************************************************************************************/

/***************************************************************************************
** \brief	Function that sends the configuration data to the input module
** \param	inputModule pointer to a _inputModule struct that holds the relevant modules configuration
** \return	None
****************************************************************************************/
void InputModule_Configuration(_inputModule *inputModule);

/***************************************************************************************
** \brief	Function that retrieves data from the input module
** \param	inputModule pointer to a _inputModule struct that holds the relevant modules configuration
** \return	None
****************************************************************************************/
void InputModule_ReceiveValues(_inputModule *inputModule);

/***************************************************************************************
** \brief	Function that sends a new pulscounter value to a specified channel from the input module
** \param	inputModule pointer to a _inputModule struct that holds the relevant modules configuration
** \param	channel The channel on which the counter needs to be set
** \param	value The new value of the counter
** \param	trigger Change on this input will trigger a send to the module
** \return	None
****************************************************************************************/
void InputModule_ResetPulsCounter(_inputModule *inputModule, uint8_t channel, int32_t value, uint8_t trigger);

/***************************************************************************************
** \brief	Function that sets the module slot for an input module
** \param	inputModule pointer to a _inputModule struct that holds the relevant modules configuration
** \param	moduleSlot the slot that the module is inserted in
** \return	None
****************************************************************************************/
void InputModule_SetModuleSlot(_inputModule *inputModule, uint8_t moduleSlot);

/***************************************************************************************
** \brief	Function that sets the module type, 6 or 10 channel
** \param	inputModule pointer to a _inputModule struct that holds the relevant modules configuration
** \param	moduleType the type of module, either INPUTMODULE6CHANNEL or INPUTMODULE10CHANNEL
** \return	None
****************************************************************************************/
void InputModule_SetModuleType(_inputModule *inputModule, uint8_t moduleType);

/***************************************************************************************
** \brief	Function that Configures an input channel on a 6 channel module
** \param	inputModule pointer to a _inputModule struct that holds the relevant modules configuration
** \param	channel The channel to be configured
** \param 	func the function of the channel, value from 1-8, use macro INPUTFUNC*
** \param 	voltage_range the voltage range of an analog measurement, use macro INPUTVOLTAGERANGE*
** \param 	pull_up pull up resistor for channel use macros related to module type INPUTPULLUP*
** \param 	pull_down pull down resistor for channel use macros related to module type INPUTPULLDOWN*
** \param 	pulses_per_rotation pulse per rotation for rotary encoders range from 0-200
** \param 	analog_filter_samples analog filter samples per measurement rang from 0 -1000
** \return	none
****************************************************************************************/
void InputModule6ch_ConfigureChannel(_inputModule *inputModule, uint8_t channel, uint8_t func, uint8_t voltage_range,
	uint8_t pull_up, uint8_t pull_down, uint8_t pulses_per_rotation, uint16_t analog_filter_samples);

/***************************************************************************************
** \brief	Function that Configures an input channel on a 10 channel module
** \param	inputModule pointer to a _inputModule struct that holds the relevant modules configuration
** \param	channel The channel to be configured
** \param 	func the function of the channel, value from 1-8, use macro INPUTFUNC*
** \param 	pull_up pull up resistor for channel use macros related to module type INPUTPULLUP*
** \param 	pull_down pull down resistor for channel use macros related to module type INPUTPULLDOWN*
** \return	none
****************************************************************************************/
void InputModule10ch_ConfigureChannel(_inputModule *inputModule, uint8_t channel, uint8_t func, uint8_t pull_up, uint8_t pull_down);

/***************************************************************************************
** \brief	Function that initializes sensor supplies on the module for 6 channel modules
** \param	inputModule pointer to a _inputModule struct that holds the relevant modules configuration
** \param	supply1 state of supply number 1 for 6 channel modules, either INPUTSENSSUPPLYON or INPUTSENSSUPPLYOFF
** \param	supply2 state of supply number 2 for 6 channel modules, either INPUTSENSSUPPLYON or INPUTSENSSUPPLYOFF
** \param	supply3 state of supply number 3 for 6 channel modules, either INPUTSENSSUPPLYON or INPUTSENSSUPPLYOFF
** \return	None
****************************************************************************************/
void InputModule6ch_ConfigureSupply(_inputModule *inputModule, uint8_t supply1, uint8_t supply2, uint8_t supply3);

/***************************************************************************************
** \brief	Function that initializes sensor supplies on the module
** \param	inputModule pointer to a _inputModule struct that holds the relevant modules configuration
** \param	supply1 state of the sensor supply, either INPUTSENSSUPPLYON or INPUTSENSSUPPLYOFF
** \return	None
****************************************************************************************/
void InputModule10ch_ConfigureSupply(_inputModule *inputModule, uint8_t supply1);

#endif /* GOCONTROLL_INPUT_MODULE_H_ */