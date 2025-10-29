/**************************************************************************************
 * \file   GOcontrollProcessorboard.c
 * \brief  Functions that handels the low level control of the GOcontroll
 * hardware.
 * \internal
 *----------------------------------------------------------------------------------------
 *                          C O P Y R I G H T
 *----------------------------------------------------------------------------------------
 * Copyright 2024 (c) by GOcontroll http://www.gocontroll.com All rights
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
#include "GocontrollProcessorboard.h"

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "print.h"

_hardwareConfig hardwareConfig;

void GocontrollProcessorboard_RegisterModule(uint8_t slot, uint8_t* rx) {
	memcpy(hardwareConfig.moduleOccupancy[slot], &rx[6], 7);
	info("module %d registered, firmware: [ ", slot + 1);
	for (uint8_t i = 0; i < 7; i++) {
		info("%d, ", hardwareConfig.moduleOccupancy[slot][i]);
	}
	info("]\n");
}

int GocontrollProcessorboard_ModuleInitialize(uint8_t moduleslot) {
	int res;
	if (moduleslot >= hardwareConfig.moduleNumber) {
		return -ENODEV;
	}
	for (uint8_t i = 0; i < 5; i++) {
		GocontrollProcessorboard_ResetStateModule(moduleslot, 1);
		GocontrollProcessorboard_Delay1ms(2);
		GocontrollProcessorboard_ResetStateModule(moduleslot, 0);
		GocontrollProcessorboard_Delay1ms(2);
		uint8_t dataTxBoot[BOOTMESSAGELENGTHCHECK] = {0};
		uint8_t dataRxBoot[BOOTMESSAGELENGTHCHECK] = {0};
		res = GocontrollProcessorboard_EscapeFromBootloader(
			moduleslot, dataTxBoot, dataRxBoot);

		dbg("bootloader:\n[");
		for (uint8_t j = 0; j < BOOTMESSAGELENGTH; j++) {
			dbg("%d, ", dataRxBoot[j]);
		}
		dbg("]\n");

		// checksum faulty, but a module seems to be there, retry
		if (res &&
			(dataRxBoot[0] == 9 || dataRxBoot[1] == BOOTMESSAGELENGTH - 1 ||
			 dataRxBoot[2] == 9)) {
			dbg("checksum error\n");
			continue;
		}
		// checksum correct but message doesn't come from the bootloader
		if (!res &&
			(dataRxBoot[0] != 9 || dataRxBoot[1] != BOOTMESSAGELENGTH - 1 ||
			 dataRxBoot[2] != 9)) {
			dbg("message incorrect\n");
			continue;
		}
		// no module present, don't loop multiple times.
		if (dataRxBoot[0] == 255) {
			dbg("no module present\n");
			break;
		}
		uint8_t dataTxFirm[BOOTMESSAGELENGTHCHECK] = {0};
		uint8_t dataRxFirm[BOOTMESSAGELENGTHCHECK] = {0};
		GocontrollProcessorboard_Delay1ms(2);
		res = GocontrollProcessorboard_EscapeFromBootloader(
			moduleslot, dataTxFirm, dataRxFirm);
		dbg("firmware:\n[");
		for (uint8_t j = 0; j <= dataRxFirm[1]; j++) {
			dbg("%d, ", dataRxFirm[j]);
		}
		dbg("]\n");
		if (!res && dataRxFirm[0] != 9 && dataRxFirm[2] != 9 &&
			dataRxFirm[1] != 0) {
			GocontrollProcessorboard_RegisterModule(moduleslot, dataRxBoot);
			GocontrollProcessorboard_Delay1ms(4);
			return 0;
		}
	}
	return -ENODEV;
}

uint8_t GocontrollProcessorboard_CheckSumCalculator(uint8_t* array,
													uint8_t length) {
	uint8_t checkSum = 0;
	for (uint8_t pointer = 0; pointer < length; pointer++) {
		checkSum += array[pointer];
	}
	return checkSum;
}
