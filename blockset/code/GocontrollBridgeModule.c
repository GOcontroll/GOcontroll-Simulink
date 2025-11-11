/**************************************************************************************
 * \file   GOcontrollBridgeModule.c
 * \brief  Utility functions to interface the GOcontroll bridge module.
 * \internal
 *----------------------------------------------------------------------------------------
 *                          C O P Y R I G H T
 *----------------------------------------------------------------------------------------
 * Copyright 2024 (c)  by GOcontroll http://www.gocontroll.com All rights
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
#include "GocontrollBridgeModule.h"

#include <errno.h>
#include <string.h>

#include "GocontrollProcessorboard.h"
#include "print.h"

/****************************************************************************************
 * Macro definitions
 ****************************************************************************************/

/****************************************************************************************
 * Local data declarations
 ****************************************************************************************/
static uint8_t
	bridgeModuleDataTx[BRIDGEMODULEMESSAGELENGTH + MESSAGEOVERLENGTH];
static uint8_t
	bridgeModuleDataRx[BRIDGEMODULEMESSAGELENGTH + MESSAGEOVERLENGTH];

const uint8_t BRIDGEMODULECHANNELID[] = {20, 10, 1};

const uint32_t VERSIONSPIPROTOCOLV2_BRIDGE = 2 << 16;

extern _hardwareConfig hardwareConfig;

/****************************************************************************************/

int BridgeModule_Configuration(_bridgeModule* bridgeModule) {
	// module not registered
	if (hardwareConfig.moduleOccupancy[bridgeModule->moduleSlot][0] == 0) {
		return -ENODEV;
	}

	bridgeModule->sw_version =
		hardwareConfig.moduleOccupancy[bridgeModule->moduleSlot][4] << 16 |
		hardwareConfig.moduleOccupancy[bridgeModule->moduleSlot][5] << 8 |
		hardwareConfig.moduleOccupancy[bridgeModule->moduleSlot][6];

	for (uint8_t channel = 0; channel < 2; channel++) {
		bridgeModuleDataTx[6 + channel] = bridgeModule->configuration[channel];
		*(uint16_t*)&bridgeModuleDataTx[12 + (channel * 2)] =
			4000;  // bridgeModule->maxCurrent[channel];
	}
	if (bridgeModule->sw_version >= VERSIONSPIPROTOCOLV2_BRIDGE) {
		return GocontrollProcessorboard_SendSpi(
			bridgeModule->moduleSlot + 1, BRIDGEMODULEMESSAGELENGTH, 1, 21, 2,
			1, bridgeModule->moduleSlot, &bridgeModuleDataTx[0], 0);
	} else {
		return GocontrollProcessorboard_SendSpi(
			1, BRIDGEMODULEMESSAGELENGTH, 0x2d, 0x01, 0x00, 0x00,
			bridgeModule->moduleSlot, &bridgeModuleDataTx[0], 0);
	}
}

/****************************************************************************************/

int BridgeModule_SendValues(_bridgeModule* bridgeModule) {
	// module not registered
	if (hardwareConfig.moduleOccupancy[bridgeModule->moduleSlot][0] == 0) {
		return -ENODEV;
	}

	int res = 0;
	for (uint8_t channel = 0; channel < 2; channel++) {
		*(uint16_t*)&bridgeModuleDataTx[(channel * 6) + 6] =
			bridgeModule->value[channel];
		*(uint32_t*)&bridgeModuleDataTx[(channel * 6) + 8] =
			bridgeModule->syncCounter[channel];
	}

	if (bridgeModule->sw_version >= VERSIONSPIPROTOCOLV2_BRIDGE) {
		res = GocontrollProcessorboard_SendReceiveSpi(
			bridgeModule->moduleSlot + 1, BRIDGEMODULEMESSAGELENGTH, 1, 21, 3,
			1, bridgeModule->moduleSlot, &bridgeModuleDataTx[0],
			&bridgeModuleDataRx[0]);
	} else {
		res = GocontrollProcessorboard_SendReceiveSpi(
			1, BRIDGEMODULEMESSAGELENGTH, 0x2e, 0x01, 0, 0,
			bridgeModule->moduleSlot, &bridgeModuleDataTx[0],
			&bridgeModuleDataRx[0]);
	}
	if (res) return res;

	bridgeModule->temperature = *(int16_t*)&bridgeModuleDataRx[6];
	bridgeModule->ground = *(uint16_t*)&bridgeModuleDataRx[8];
	for (uint8_t channel = 0; channel < 2; channel++) {
		bridgeModule->current[channel] =
			*(int16_t*)&bridgeModuleDataRx[(channel * 2) + 10];
	}
	return 0;
}

/****************************************************************************************/

int BridgeModule_SetModuleSlot(_bridgeModule* bridgeModule,
							   uint8_t moduleSlot) {
	if (moduleSlot < hardwareConfig.moduleNumber) {
		if (!memcmp(hardwareConfig.moduleOccupancy[moduleSlot],
					BRIDGEMODULECHANNELID, 3)) {
			bridgeModule->moduleSlot = moduleSlot;
			return 0;
		}
		err("module slot %d is contested by multiple module claims, check "
			"*SetModuleSlot init functions for double slot claims.\n",
			moduleSlot + 1);
		return -EINVAL;
	}
	err("Invalid module slot selected for a bridge module, selected %d, but "
		"the range is 1-%d.\n",
		moduleSlot + 1, hardwareConfig.moduleNumber);
	return -EINVAL;
}

/****************************************************************************************/

int BridgeModule_ConfigureChannel(_bridgeModule* bridgeModule, uint8_t channel,
								  uint8_t func, uint8_t freq) {
	if (channel > 1) {
		err("Configured channel is out of range for bridge module in slot %d, "
			"range is 1-10, entered is %d, please use the macros to configure "
			"channels.",
			bridgeModule->moduleSlot + 1, channel + 1);
		return -EINVAL;
	}
	if ((func == 0) || (func > 6)) {
		err("Invalid function set for channel %d, bridge module in slot %d, "
			"please use the macros to set the channel function\n",
			channel + 1, bridgeModule->moduleSlot + 1);
		return -EINVAL;
	}
	if ((freq == 0) || (freq > 7)) {
		err("Invalid frequency set for channel %d, bridge module in slot %d, "
			"please use the macros to set the channel frequency\n",
			channel + 1, bridgeModule->moduleSlot + 1);
		return -EINVAL;
	}
	bridgeModule->configuration[channel] = freq | (func << 4);
	return 0;
}