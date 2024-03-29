/************************************************************************************//**
* \file         GOcontroll420InputModule.c
* \brief        Utility functions to interface the GOcontroll 4-20 mA input module.
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
#include "Gocontroll420maInputModule.h"
#include "GocontrollProcessorboard.h"
#include <stdlib.h>

/****************************************************************************************
* Macro definitions
****************************************************************************************/


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static uint8_t inputModule420maDataTx[INPUTMODULE420MAMESSAGELENGTH+MESSAGEOVERLENGTH];
static uint8_t inputModule420maDataRx[INPUTMODULE420MAMESSAGELENGTH+MESSAGEOVERLENGTH];

/****************************************************************************************/

void InputModule420ma_Configuration(_inputModule420ma *inputModule420ma)
{
uint8_t dataPointer = 6;

	for(uint8_t pointer = 0; pointer <10; pointer++)
	{
		inputModule420maDataTx[dataPointer++]				= inputModule420ma->configuration[pointer];
	}

inputModule420maDataTx[16] = inputModule420ma->supply16va;
inputModule420maDataTx[17] = inputModule420ma->supply16vb;

GocontrollProcessorboard_SendSpi(1, INPUTMODULE420MAMESSAGELENGTH, 1,13,2,1, inputModule420ma->moduleSlot, &inputModule420maDataTx[0],0);
}

/****************************************************************************************/

void InputModule420ma_ReceiveValues(_inputModule420ma *inputModule420ma)
{
	if(GocontrollProcessorboard_SendReceiveSpi(1, INPUTMODULE420MAMESSAGELENGTH, 2,13,3,1, inputModule420ma->moduleSlot, &inputModule420maDataTx[0], &inputModule420maDataRx[0])==0)
	{
		for (uint8_t pointer = 0; pointer <10; pointer++)
		{
			inputModule420ma->value[pointer] 		= *(uint16_t*)&inputModule420maDataRx[(pointer*2)+6];
		}
	}
}

/****************************************************************************************/