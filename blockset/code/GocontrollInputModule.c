/************************************************************************************//**
* \file         GOcontrollInputModule.c
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include "GocontrollInputModule.h"
#include "GocontrollProcessorboard.h"
#include <stdlib.h>
#include <string.h>

/****************************************************************************************
* Macro definitions
****************************************************************************************/


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static uint8_t inputModuleDataTx[INPUTMODULE6CHMESSAGELENGTH+MESSAGEOVERLENGTH];
static uint8_t inputModuleDataRx[INPUTMODULE6CHMESSAGELENGTH+MESSAGEOVERLENGTH];

uint8_t resistorMatrix[4] = {0,3,1,2};

const uint8_t INPUTMODULE6CHANNELID[] = {20,10,1};
const uint8_t INPUTMODULE10CHANNELID[] = {20,10,2};

extern _hardwareConfig hardwareConfig;

/****************************************************************************************/

void InputModule_Configuration(_inputModule *inputModule)
{
	uint8_t dataPointer = 6;

		if(inputModule->moduleType == INPUTMODULE6CHANNEL)
		{
			for(uint8_t pointer = 0; pointer <6; pointer++)
			{
			inputModuleDataTx[dataPointer++]				= inputModule->configuration[pointer];
			inputModuleDataTx[dataPointer++]				= inputModule->interface[pointer];
			inputModuleDataTx[dataPointer++]				= inputModule->callibrationValue1[pointer];
			inputModuleDataTx[dataPointer++]				= inputModule->callibrationValue2[pointer];
			inputModuleDataTx[dataPointer++]				= inputModule->callibrationValue3[pointer];
			inputModuleDataTx[dataPointer++]				= inputModule->callibrationValue4[pointer];
			}

			inputModuleDataTx[42] = inputModule->sensorSupply1;
			inputModuleDataTx[43] = inputModule->sensorSupply2;
			inputModuleDataTx[44] = inputModule->sensorSupply3;

			if (hardwareConfig.moduleOccupancy[inputModule->moduleSlot][4] >= 2) {
				GocontrollProcessorboard_SendSpi(inputModule->moduleSlot+1, INPUTMODULE6CHMESSAGELENGTH, 1,11,2,1, inputModule->moduleSlot, &inputModuleDataTx[0],0);
			} else {
				GocontrollProcessorboard_SendSpi(1, INPUTMODULE6CHMESSAGELENGTH, 1,0,0,0, inputModule->moduleSlot, &inputModuleDataTx[0],0);
			}
		}
		else if(inputModule->moduleType == INPUTMODULE10CHANNEL)
		{

			for(uint8_t pointer = 0; pointer <10; pointer++)
			{
			inputModuleDataTx[dataPointer++]				= inputModule->configuration[pointer];
			inputModuleDataTx[dataPointer++]				= inputModule->interface[pointer];
			inputModuleDataTx[dataPointer++]				= inputModule->callibrationValue1[pointer];
			inputModuleDataTx[dataPointer++]				= inputModule->callibrationValue2[pointer];
			}

			inputModuleDataTx[46] = inputModule->sensorSupply1;

			GocontrollProcessorboard_SendSpi(1, INPUTMODULE10CHMESSAGELENGTH, 1,12,2,1, inputModule->moduleSlot, &inputModuleDataTx[0],0);
		}
}

/****************************************************************************************/

void InputModule_ReceiveValues(_inputModule *inputModule)
{
	int res = 0;
	if(inputModule->moduleType == INPUTMODULE6CHANNEL)
	{
		if (hardwareConfig.moduleOccupancy[inputModule->moduleSlot][4] >= 2) {
			res = GocontrollProcessorboard_SendReceiveSpi(inputModule->moduleSlot+1, INPUTMODULE6CHMESSAGELENGTH, 2,11,3,1, inputModule->moduleSlot, &inputModuleDataTx[0], &inputModuleDataRx[0]);
		} else {
			res = GocontrollProcessorboard_SendReceiveSpi(1, INPUTMODULE6CHMESSAGELENGTH, 2,0,0,0, inputModule->moduleSlot, &inputModuleDataTx[0], &inputModuleDataRx[0]);
		}
		if(res==0)
		{
			for (uint8_t pointer = 0; pointer <6; pointer++)
			{
				inputModule->value[pointer] 		= *(int32_t*)&inputModuleDataRx[(pointer*8)+6];
				inputModule->syncCounter[pointer] 	= *(int32_t*)&inputModuleDataRx[(pointer*8)+10];
			}
		}
	}
	else if(inputModule->moduleType == INPUTMODULE10CHANNEL)
	{
		if(GocontrollProcessorboard_SendReceiveSpi(1, INPUTMODULE10CHMESSAGELENGTH, 2,12,3,1, inputModule->moduleSlot, &inputModuleDataTx[0], &inputModuleDataRx[0])==0)
		{
			for (uint8_t pointer = 0; pointer <10; pointer++)
			{
				inputModule->value[pointer] 		= *(int32_t*)&inputModuleDataRx[(pointer*4)+6];
			}
		}
	}
}

/****************************************************************************************/

void InputModule_ResetPulsCounter(_inputModule *inputModule, uint8_t channel, int32_t value, uint8_t trigger)
{
	if(inputModule->moduleType == INPUTMODULE6CHANNEL && channel > 5)
	{
		return;
	}
	else if(inputModule->moduleType == INPUTMODULE10CHANNEL && channel > 9)
	{
		return;
	}

	if(inputModule->pulscounterResetTrigger[channel] != trigger)
	{
		inputModuleDataTx[6]				= channel;
		*(int32_t*) &inputModuleDataTx[7]  	= value;

		if(inputModule->moduleType == INPUTMODULE6CHANNEL)
		{
			if (hardwareConfig.moduleOccupancy[inputModule->moduleSlot][4] >= 2) {
				GocontrollProcessorboard_SendSpi(inputModule->moduleSlot+1, INPUTMODULE6CHMESSAGELENGTH, 1,11,3,2, inputModule->moduleSlot, &inputModuleDataTx[0],0);
			} else {
				GocontrollProcessorboard_SendSpi(1, INPUTMODULE6CHMESSAGELENGTH, 3,0,0,0, inputModule->moduleSlot, &inputModuleDataTx[0],0);
			}
		}
		else if(inputModule->moduleType == INPUTMODULE10CHANNEL)
		{
			GocontrollProcessorboard_SendSpi(1, INPUTMODULE10CHMESSAGELENGTH, 1,12,3,2, inputModule->moduleSlot, &inputModuleDataTx[0],0);
		}

		inputModule->pulscounterResetTrigger[channel] = trigger;
	}
}

/****************************************************************************************/

void InputModule_SetModuleSlot(_inputModule *inputModule, uint8_t moduleSlot) {
	if (moduleSlot < hardwareConfig.moduleNumber){
		if (inputModule->moduleType == INPUTMODULE6CHANNEL){
			if (!memcmp(hardwareConfig.moduleOccupancy, INPUTMODULE6CHANNELID, 3)) {
				inputModule->moduleSlot = moduleSlot;
				return;
			}
		} else {
			if (!memcmp(hardwareConfig.moduleOccupancy, INPUTMODULE10CHANNELID, 3)) {
				inputModule->moduleSlot = moduleSlot;
				return;
			}
		}
		printf("module slot %d is contested by multiple module claims, check *SetModuleSlot init functions for double slot claims.\n", moduleSlot+1);
		exit(-1);
	}
	printf("Invalid module slot selected for an input module, selected %d, but the range is 1-%d.\n", moduleSlot + 1, hardwareConfig.moduleNumber);
	exit(-1);
}

/****************************************************************************************/

void InputModule_SetModuleType(_inputModule *inputModule, uint8_t moduleType) {
	if (moduleType == INPUTMODULE6CHANNEL || moduleType == INPUTMODULE10CHANNEL){
		inputModule->moduleType = moduleType;
		return;
	}
	printf("Invalid module type selected for input module in slot %d, please use the macros to set a module type.\n", inputModule->moduleSlot +1);
	exit(-1);
}

/****************************************************************************************/

void InputModule6ch_ConfigureChannel(_inputModule *inputModule, uint8_t channel, uint8_t func, uint8_t voltage_range,
	uint8_t pull_up, uint8_t pull_down, uint8_t pulses_per_rotation, uint16_t analog_filter_samples) {

	if (inputModule->moduleType != INPUTMODULE6CHANNEL) {
		printf("Incorrect module type selected for channel %d, input module in slot %d, this function is only meant for 6 channel modules.\n",
		channel +1, inputModule->moduleSlot +1);
		exit(-1);
	}
	if ((func > 8) || (func == 0)) {
		printf("invalid function set for channel %d, input module in slot %d, range is 1-8, entered is %d, please use the macros to configure channels.\n",
		channel+1, inputModule->moduleSlot +1, func);
		exit(-1);
	}

	if (channel > 5) {
		printf("Configured channel is out of range for input module in slot %d, range is 1-6, entered is %d, please use the macros to configure channels.\n",
		inputModule->moduleSlot +1, channel+1);
		exit(-1);
	}
	if (voltage_range > 2) {
		printf("Configured voltage range is out of range for channel %d, input module in slot %d, range is 0-2, entered is %d, please use the macros to configure channels.\n",
		channel+1, inputModule->moduleSlot +1, voltage_range);
		exit(-1);
	}
	if (analog_filter_samples > ANALOGSAMPLESMAX ) {
		printf("Configured analog filter samples is out of range for channel %d, input module in slot %d, range is 0-%d, entered is %d.\n",
		channel+1, inputModule->moduleSlot +1, ANALOGSAMPLESMAX, analog_filter_samples);
		exit(-1);
	}
	if ( pulses_per_rotation > PULSESPERROTATIONMAX ) {
		printf("Configured pulses per rotation is out of range for channel %d, input module in slot %d, range is 0-%d, entered is %d.\n",
		channel+1, inputModule->moduleSlot +1, PULSESPERROTATIONMAX, pulses_per_rotation);
		exit(-1);
	}
	if (pull_up > 3) {
		printf("Invalid pull up configuration for channel %d, input module in slot %d, range is 0-3, entered is %d, please use the macros to configure channels.\n",
		channel+1, inputModule->moduleSlot +1, pull_up);
		exit(-1);
	}
	if (pull_down > 3) {
		printf("Invalid pull down configuration for channel %d, input module in slot %d, range is 0-3, entered is %d, please use the macros to configure channels.\n",
		channel+1, inputModule->moduleSlot +1, pull_down);
		exit(-1);
	}
	//start configuring
	inputModule->configuration[channel] = func;
	inputModule->interface[channel] = (resistorMatrix[pull_up]) | (resistorMatrix[pull_down]<<2) | (voltage_range << 6);
	if (func > 2) {
		inputModule->callibrationValue1[channel] = pulses_per_rotation;
	} else {
		inputModule->callibrationValue1[channel] = analog_filter_samples>>8;
		inputModule->callibrationValue2[channel] = analog_filter_samples;
	}
}

/****************************************************************************************/

void InputModule10ch_ConfigureChannel(_inputModule *inputModule, uint8_t channel, uint8_t func, uint8_t pull_up, uint8_t pull_down) {

	if (inputModule->moduleType != INPUTMODULE10CHANNEL) {
		printf("Incorrect module type selected for channel %d, input module in slot %d, this function is only meant for 6 channel modules\n",
		channel+1, inputModule->moduleSlot +1);
		exit(-1);
	}
	if ((func > 8) || (func == 0)) {
		printf("Invalid function set for channel %d, input module in slot %d, range is 1-8, entered is %d, please use the macros to configure channels.\n",
		channel+1, inputModule->moduleSlot +1, func);
		exit(-1);
	}
	if (channel>9) {
		printf("Configured channel is out of range for input module in slot %d, range is 1-10, entered is %d, please use the macros to configure channels.\n",
		inputModule->moduleSlot +1, channel+1);
		exit(-1);
	}
	if (pull_up > 1) {
		printf("Invalid pull up configuration for channel %d, input module in slot %d, range is 0-1, entered is %d, please use the macros to configure channels.\n",
		channel+1, inputModule->moduleSlot +1, pull_up);
		exit(-1);
	}
	if (pull_down > 1) {
		printf("Invalid pull down configuration for channel %d, input module in slot %d, range is 0-1, entered is %d, please use the macros to configure channels.\n",
		channel+1, inputModule->moduleSlot +1, pull_down);
		exit(-1);
	}
	inputModule->configuration[channel] = func;
	inputModule->interface[channel] = (resistorMatrix[pull_up]) | (resistorMatrix[pull_down]<<2);
}

/****************************************************************************************/

void InputModule6ch_ConfigureSupply(_inputModule *inputModule, uint8_t supply1, uint8_t supply2, uint8_t supply3) {

	if (inputModule->moduleType != INPUTMODULE6CHANNEL) {
		printf("Incorrect module type selected for supply configuration in slot %d, this function is only meant for 6 channel modules.\n",
		inputModule->moduleSlot +1);
		exit(-1);
	}
	if ((supply1 == 1 || supply1 == 2) && (supply2 == 1 || supply2 == 2) && (supply3 == 1 || supply3 == 2)){
		inputModule->sensorSupply1 = supply1;
		inputModule->sensorSupply2 = supply2;
		inputModule->sensorSupply3 = supply3;
		return;
	}
	printf("Incorrect sensor supply option set for inputmodule in slot %d, please use the macros INPUTSENSSUPPLYON and INPUTSENSSUPPLYOFF.\n",
	inputModule->moduleSlot +1);
	exit(-1);
}

/****************************************************************************************/

void InputModule10ch_ConfigureSupply(_inputModule *inputModule, uint8_t supply1) {
	if (inputModule->moduleType != INPUTMODULE10CHANNEL) {
		printf("Incorrect module type selected for supply configuration in slot %d, this function is only meant for 10 channel modules.\n",
		inputModule->moduleSlot +1);
		exit(-1);
	}
	if (supply1 == 1 || supply1 == 2) {
		inputModule->sensorSupply1 = supply1;
		return;
	}
	printf("Incorrect sensor supply option set for inputmodule in slot %d, please use the macros INPUTSENSSUPPLYON and INPUTSENSSUPPLYOFF.\n",
	inputModule->moduleSlot +1);
	exit(-1);
}