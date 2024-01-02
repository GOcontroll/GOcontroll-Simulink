/************************************************************************************//**
* \file         GOcontrollOutputModule.c
* \brief        Utility functions to interface the GOcontroll output module.
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
#include "GocontrollOutputModule.h"
#include "GocontrollProcessorboard.h"
#include <stdlib.h>
#include <string.h>

/****************************************************************************************
* Macro definitions
****************************************************************************************/


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static uint8_t outputModuleDataTx[OUTPUTMODULE10CHMESSAGELENGTH+MESSAGEOVERLENGTH];
static uint8_t outputModuleDataRx[OUTPUTMODULE10CHMESSAGELENGTH+MESSAGEOVERLENGTH];

const uint8_t OUTPUTMODULE6CHANNELID[] = {20,20,2};
const uint8_t OUTPUTMODULE10CHANNELID[] = {20,20,3};

extern _hardwareConfig hardwareConfig;

/****************************************************************************************/

void OutputModule_Configuration(_outputModule *outputModule)
{
	if(outputModule->moduleType == OUTPUTMODULE6CHANNEL)
	{
		for(uint8_t channel = 0; channel <6; channel++)
		{
		outputModuleDataTx[channel+6]					= outputModule->configuration[channel];
		*(uint16_t*) &outputModuleDataTx[channel*2+12]	= outputModule->currentMax[channel];
		}

		if (hardwareConfig.moduleOccupancy[outputModule->moduleSlot][4] >= 2) {
			GocontrollProcessorboard_SendSpi(outputModule->moduleSlot+1, OUTPUTMODULE6CHMESSAGELENGTH, 1,22,2,1, outputModule->moduleSlot, &outputModuleDataTx[0],0);
		} else {
			GocontrollProcessorboard_SendSpi(1, OUTPUTMODULE6CHMESSAGELENGTH, 101,0,0,0, outputModule->moduleSlot, &outputModuleDataTx[0],0);
		}

		for(uint8_t channel = 0; channel <6; channel++)
		{
		*(uint16_t*) &outputModuleDataTx[channel*2+6]		= outputModule->peakCurrent[channel];
		*(uint16_t*) &outputModuleDataTx[channel*2+18]		= outputModule->peakTime[channel];
		}

		/* The second initialization message is delayed by 500 us because the module needs to handle the first message */
		if (hardwareConfig.moduleOccupancy[outputModule->moduleSlot][4] >= 2) {
			GocontrollProcessorboard_SendSpi(outputModule->moduleSlot+1, OUTPUTMODULE6CHMESSAGELENGTH, 1,22,2,2, outputModule->moduleSlot, &outputModuleDataTx[0],500);
		} else {
			GocontrollProcessorboard_SendSpi(1, OUTPUTMODULE6CHMESSAGELENGTH, 111,0,0,0, outputModule->moduleSlot, &outputModuleDataTx[0],500);
		}
	}
	else if(outputModule->moduleType == OUTPUTMODULE10CHANNEL)
	{
		for(uint8_t channel = 0; channel <10; channel++)
		{
		outputModuleDataTx[channel+6]					= outputModule->configuration[channel];
		}
		/* Hardcoded maximum 10 channel output module current */
		*(uint16_t*) &outputModuleDataTx[16]			= 10000;

		/* Module ID check SPI communication protocol document */
		GocontrollProcessorboard_SendSpi(1, OUTPUTMODULE10CHMESSAGELENGTH, 1,23,2,1, outputModule->moduleSlot, &outputModuleDataTx[0],0);

		for(uint8_t channel = 0; channel <10; channel++)
		{
		*(uint16_t*) &outputModuleDataTx[channel*2+6]		= outputModule->peakCurrent[channel];
		*(uint16_t*) &outputModuleDataTx[channel*2+26]		= outputModule->peakTime[channel];
		}

		/* The second initialization message is delayed by 500 us because the module needs to handle the first message */
		/* Module ID check SPI communication protocol document */
		GocontrollProcessorboard_SendSpi(1, OUTPUTMODULE10CHMESSAGELENGTH, 1,23,2,2, outputModule->moduleSlot, &outputModuleDataTx[0],500);
	}
}

/****************************************************************************************/

void OutputModule_SendValues(_outputModule *outputModule)
{
	int res = 0;
	outputModule->errorCode = 0x10000000;

	if(outputModule->moduleType == OUTPUTMODULE6CHANNEL)
	{
		for(uint8_t channel = 0; channel <6; channel++)
		{
		*(uint16_t*) &outputModuleDataTx[(channel*6)+6]		= outputModule->value[channel];
		*(uint32_t*) &outputModuleDataTx[(channel*6)+8]		= outputModule->syncCounter[channel];
		}
		if (hardwareConfig.moduleOccupancy[outputModule->moduleSlot][4] >= 2) {
			res = GocontrollProcessorboard_SendReceiveSpi(outputModule->moduleSlot+1, OUTPUTMODULE6CHMESSAGELENGTH, 1,22,3,1, outputModule->moduleSlot, &outputModuleDataTx[0], &outputModuleDataRx[0]);
		} else {
			res = GocontrollProcessorboard_SendReceiveSpi(1, OUTPUTMODULE6CHMESSAGELENGTH, 102,0,0,0, outputModule->moduleSlot, &outputModuleDataTx[0], &outputModuleDataRx[0]);
		}
		if(res==0)
		{
			outputModule->temperature 	= *(int16_t*)&outputModuleDataRx[6];
			outputModule->ground 		= *(uint16_t*)&outputModuleDataRx[8];
			outputModule->errorCode 	= *(uint32_t*)&outputModuleDataRx[22];

			for (uint8_t channel = 0; channel <6; channel++)
			{
				outputModule->current[channel] = *(int16_t*)&outputModuleDataRx[(channel*2)+10];
			}
			/* Correct reception so decrease the error counter */
			if(outputModule->communicationCheck > 0)
			{
				outputModule->communicationCheck --;
			}
		}
		/* At this point we have a wrong checksum. So increase the error counter */
		else
		{
		outputModule->communicationCheck ++;
		}
	}
	else if(outputModule->moduleType == OUTPUTMODULE10CHANNEL)
	{
	for(uint8_t channel = 0; channel <10; channel++)
		{
		*(uint16_t*) &outputModuleDataTx[(channel*2)+6]		= outputModule->value[channel];
		}

		/* Module ID check SPI communication protocol document */
		if(GocontrollProcessorboard_SendReceiveSpi(1, OUTPUTMODULE10CHMESSAGELENGTH, 1,23,3,1, outputModule->moduleSlot, &outputModuleDataTx[0], &outputModuleDataRx[0])==0)
		{
			/* Module ID check SPI communication protocol document */
			if(	outputModuleDataRx[2] == 2  &&
				outputModuleDataRx[3] == 23 &&
				outputModuleDataRx[4] == 4  &&
				outputModuleDataRx[5] == 1  ){
			outputModule->temperature 		= *(int16_t*)&outputModuleDataRx[6];
			outputModule->ground 			= *(uint16_t*)&outputModuleDataRx[8];
			outputModule->inputVoltage 		= *(uint16_t*)&outputModuleDataRx[10];
			outputModule->totalCurrent 		= *(uint16_t*)&outputModuleDataRx[12];
			}
			/* Correct reception so decrease the error counter */
			if(outputModule->communicationCheck > 0)
			{
				outputModule->communicationCheck --;
			}
		}
		/* At this point we have a wrong checksum. So increase the error counter */
		else
		{
		outputModule->communicationCheck ++;
		}
	}

	/* When communicationCheck > 0 it means that there are repeated checksum errors */
	if(outputModule->communicationCheck > 0)
	{
	outputModule->errorCode 	|= 0x20000000;
	}
	/* if no communication errors are there, clear flag */
	else
	{
	outputModule->errorCode 	&= ~0x20000000;
	}
}

/****************************************************************************************/

void OutputModule_SetModuleSlot(_outputModule *outputModule, uint8_t moduleSlot) {
	if (moduleSlot < hardwareConfig.moduleNumber){
		if (outputModule->moduleType == OUTPUTMODULE6CHANNEL){
			if (!memcmp(hardwareConfig.moduleOccupancy, OUTPUTMODULE6CHANNELID, 3)) {
				outputModule->moduleSlot = moduleSlot;
				return;
			}
		} else {
			if (!memcmp(hardwareConfig.moduleOccupancy, OUTPUTMODULE10CHANNELID, 3)) {
				outputModule->moduleSlot = moduleSlot;
				return;
			}
		}
		printf("module slot %d is contested by multiple module claims, check *SetModuleSlot init functions for double slot claims.\n", moduleSlot+1);
		exit(-1);
	}
	printf("Invalid module slot selected for an output module, selected %d, but the range is 1-%d.\n", moduleSlot + 1, hardwareConfig.moduleNumber);
	exit(-1);
}

/****************************************************************************************/

void OutputModule_SetModuleType(_outputModule *outputModule, uint8_t moduleType) {
	if (moduleType == OUTPUTMODULE6CHANNEL || moduleType == OUTPUTMODULE10CHANNEL){
		outputModule->moduleType = moduleType;
		return;
	}
	printf("Invalid module type selected for output module in slot %d, please use the macros to set a module type.\n", outputModule->moduleSlot +1);
	exit(-1);
}

/****************************************************************************************/

void OutputModule6ch_ConfigureChannel(_outputModule *outputModule, uint8_t channel, uint8_t func,
	uint16_t currentMax, uint16_t peak_current, uint16_t peak_time) {

	if (outputModule->moduleType != OUTPUTMODULE6CHANNEL) {
		printf("Incorrect module type selected for channel %d, output module in slot %d, this function is only meant for 6 channel modules\n",
		outputModule->moduleSlot+1, channel+1);
		exit(-1);
	}
	if ((func > 8) || (func == 0)) {
		printf("invalid function set for channel %d, output module in slot %d, range is 1-8, entered is %d, please use the macros to configure channels.\n",
		channel+1, outputModule->moduleSlot+1, func);
		exit(-1);
	}
	if (channel > 5) {
		printf("Configured channel is out of range for output module in slot %d, range is 1-6, entered is %d, please use the macros to configure channels.\n",
		outputModule->moduleSlot+1, channel+1);
		exit(-1);
	}
	if (currentMax > CURRENTMAXMAX ) {
		printf("Configured maximum current is out of range for channel %d, output module in slot %d, range is 0-%d, entered is %d.\n",
		channel+1,outputModule->moduleSlot+1, CURRENTMAXMAX, currentMax);
		exit(-1);
	}
	if ( peak_current > PEAKCURRENTMAX ) {
		printf("Configured peak dutycycle is out of range for channel %d, output module in slot %d, range is 0-%d, entered is %d.\n",
		channel+1, outputModule->moduleSlot+1, PEAKCURRENTMAX, peak_current);
		exit(-1);
	}
	// if ( peak_time > PEAKTIMEMAX ) {
	// 	printf("Configured pulses per rotation is out of range for channel %d, range is 0-%d, entered is %d\n",channel, PULSESPERROTATIONMAX, pulses_per_rotation);
	// 	exit(-1);
	// }

	//start configuring
	//clear the func field
	outputModule->configuration[channel] = outputModule->configuration[channel] & OUTPUTFREQMASK;
	//set the new values in the func field
	outputModule->configuration[channel] = outputModule->configuration[channel] | (func << 4);
	//set the other values
	outputModule->currentMax[channel] = currentMax;
	outputModule->peakCurrent[channel] = peak_current;
	outputModule->peakTime[channel] = peak_time;
}

/****************************************************************************************/

void OutputModule10ch_ConfigureChannel(_outputModule *outputModule, uint8_t channel, uint8_t func,
	uint16_t peak_current, uint16_t peak_time) {

	if (outputModule->moduleType != OUTPUTMODULE10CHANNEL) {
		printf("Incorrect module type selected for channel %d, output module in slot %d, this function is only meant for 6 channel modules\n",
		channel+1, outputModule->moduleSlot+1);
		exit(-1);
	}
	if ((func > 5) || (func == 0)) {
		printf("invalid function set for channel %d, output module in slot %d, range is 1-5, entered is %d, please use the macros to configure channels.\n",
		channel+1, outputModule->moduleSlot+1, func);
		exit(-1);
	}
	if (channel>9) {
		printf("Configured channel is out of range for output module in slot %d, range is 1-10, entered is %d, please use the macros to configure channels.\n",
		outputModule->moduleSlot+1, channel+1);
		exit(-1);
	}
	if ( peak_current > PEAKCURRENTMAX ) {
		printf("Configured peak dutycycle is out of range for channel %d, output module in slot %d, range is 0-%d, entered is %d\n",
		channel+1, outputModule->moduleSlot+1, PEAKCURRENTMAX, peak_current);
		exit(-1);
	}
	//start configuring
	//clear the func field
	outputModule->configuration[channel] = outputModule->configuration[channel] & OUTPUTFREQMASK;
	//set the new values in the func field
	outputModule->configuration[channel] = outputModule->configuration[channel] | (func << 4);
	//set the other values
	outputModule->peakCurrent[channel] = peak_current;
	outputModule->peakTime[channel] = peak_time;
}

/****************************************************************************************/

void OutputModule_ConfigureFrequency(_outputModule *outputModule, uint8_t channel, uint8_t frequency) {

	if (outputModule->moduleType == OUTPUTMODULE6CHANNEL) {
		if (channel > 2) {
			printf("Frequency channel out of range for output module in slot %d, range is 0-2, entered was %d, please use the macros to configure channels.\n",
			outputModule->moduleSlot + 1, channel);
			exit(-1);
		}
		if ((frequency == 0) || (frequency > 7)){
			printf("Configured frequency on frequency channel %d, output module in slot %d, is out of range, please use the output frequency macros.\n",
			channel, outputModule->moduleSlot + 1);
			exit(-1);
		}
	} else if (outputModule->moduleType == OUTPUTMODULE10CHANNEL) {
		if (channel > 4) {
			printf("Frequency channel out of range for output module in slot %d, range is 0-4, entered was %d\n",
			outputModule->moduleSlot + 1, channel);
			exit(-1);
		}
		if ((frequency == 0) || (frequency > 4)){
			printf("Configured frequency on frequency channel %d, output module in slot %d, is out of range, please use the macros to configure channels.\n",
			channel, outputModule->moduleSlot + 1);
			exit(-1);
		}
	} else {
		printf("Invalid module type set for output Module in slot %d, unable to configure the frequency\n", outputModule->moduleSlot + 1);
		exit(-1);
	}
	//start configuring
	//clear the frequency field
	outputModule->configuration[channel*2] = outputModule->configuration[channel] & OUTPUTFUNCMASK;
	outputModule->configuration[channel*2 +1] = outputModule->configuration[channel] & OUTPUTFUNCMASK;
	//set the new values in the frequency field
	outputModule->configuration[channel*2] = outputModule->configuration[channel] | frequency;
	outputModule->configuration[channel*2 +1] = outputModule->configuration[channel] | frequency;
	return;
}