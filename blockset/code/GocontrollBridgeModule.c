/************************************************************************************//**
* \file         GOcontrollBridgeModule.c
* \brief        Utility functions to interface the GOcontroll bridge module.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2023 (c)  by GOcontroll   http://www.gocontroll.com      All rights reserved
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
#include "GocontrollBridgeModule.h"
#include "GocontrollProcessorboard.h"
#include <stdlib.h>

/****************************************************************************************
* Macro definitions
****************************************************************************************/


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static uint8_t bridgeModuleDataTx[BRIDGEMODULEMESSAGELENGTH+MESSAGEOVERLENGTH];
static uint8_t bridgeModuleDataRx[BRIDGEMODULEMESSAGELENGTH+MESSAGEOVERLENGTH];

extern _hardwareConfig hardwareConfig;

/****************************************************************************************/

void BridgeModule_Configuration(_bridgeModule *bridgeModule)
{
	for(uint8_t channel = 0; channel <2; channel++)
	{
	bridgeModuleDataTx[6+channel]						= bridgeModule->configuration[channel];
	*(uint16_t*) &bridgeModuleDataTx[12+(channel*2)]	= 4000; //bridgeModule->maxCurrent[channel];
	}

	GocontrollProcessorboard_SendSpi(1, BRIDGEMODULEMESSAGELENGTH, 0x2d,0x01,0x00,0x00, bridgeModule->moduleSlot, &bridgeModuleDataTx[0],0);
}

/****************************************************************************************/

void BridgeModule_SendValues(_bridgeModule *bridgeModule)
{
	for(uint8_t channel = 0; channel <2; channel++)
	{
	*(uint16_t*) &bridgeModuleDataTx[(channel*6)+6]		= bridgeModule->value[channel];
	*(uint32_t*) &bridgeModuleDataTx[(channel*6)+8]		= bridgeModule->syncCounter[channel];
	}

	if(GocontrollProcessorboard_SendReceiveSpi(1, BRIDGEMODULEMESSAGELENGTH, 0x2e,0x01,0x00,0x00, bridgeModule->moduleSlot, &bridgeModuleDataTx[0], &bridgeModuleDataRx[0]))
	{
		if( *(uint16_t*) &bridgeModuleDataRx[2] == 303)
		{
		bridgeModule->temperature 	= *(int16_t*)&bridgeModuleDataRx[6];
		bridgeModule->ground 		= *(uint16_t*)&bridgeModuleDataRx[8];
			for (uint8_t channel = 0; channel <2; channel++)
			{
			bridgeModule->current[channel] = *(int16_t*)&bridgeModuleDataRx[(channel*2)+10];
			}
		}
	}
}

/****************************************************************************************/

void BridgeModule_SetModuleSlot(_bridgeModule *bridgeModule, uint8_t moduleSlot) {
	if (moduleSlot < hardwareConfig.moduleNumber){
		if (hardwareConfig.moduleOccupancy[moduleSlot] != SLOTFREE) {
			hardwareConfig.moduleOccupancy[moduleSlot] = SLOTOCCUPIED;
			bridgeModule->moduleSlot = moduleSlot;
			return;
		}
		printf("module slot %d is contested by multiple module claims, check *SetModuleSlot init functions for double slot claims.\n", moduleSlot+1);
		exit(-1);
	}
	printf("Invalid module slot selected for a bridge module, selected %d, but the range is 1-%d.\n", moduleSlot + 1, hardwareConfig.moduleNumber);
	exit(-1);
}

/****************************************************************************************/

void BridgeModule_ConfigureChannel(_bridgeModule *bridgeModule, uint8_t channel, uint8_t func, uint8_t freq) {
	if (channel > 1) {
		printf("Configured channel is out of range for bridge module in slot %d, range is 1-10, entered is %d, please use the macros to configure channels.",
		bridgeModule->moduleSlot +1,channel +1);
		exit(-1);
	}
	if ((func == 0) || (func > 6)) {
		printf("Invalid function set for channel %d, bridge module in slot %d, please use the macros to set the channel function\n",
		channel+1, bridgeModule->moduleSlot +1);
		exit(-1);
	}
	if ((freq == 0) || (freq > 7)) {
		printf("Invalid frequency set for channel %d, bridge module in slot %d, please use the macros to set the channel frequency\n",
		channel+1, bridgeModule->moduleSlot+1);
		exit(-1);
	}
	bridgeModule->configuration[channel] = freq | (func << 4);
	return;
}