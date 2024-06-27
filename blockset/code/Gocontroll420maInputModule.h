/************************************************************************************//**
* \file         GOcontroll420InputModule.c
* \brief        Utility functions to interface the GOcontroll 4-20 mA input module.
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
#ifndef GOCONTROLL_INPUT420MA_MODULE_H_
#define GOCONTROLL_INPUT420MA_MODULE_H_

/****************************************************************************************
* Include files
****************************************************************************************/
#include "GocontrollProcessorboard.h"
#include "stdio.h"

/****************************************************************************************
* Module specific defines
****************************************************************************************/



/****************************************************************************************
* Data declarations
****************************************************************************************/

typedef struct
{
uint8_t configuration[10];
uint16_t value[10];
uint32_t syncCounter[10];
uint8_t status;
uint32_t moduleIdentifier;
uint8_t supply16ch[5];
uint8_t moduleSlot;
uint32_t sw_version;
} _inputModule420ma;

/****************************************************************************************
* Function prototypes
****************************************************************************************/

/***************************************************************************************
** \brief	Function that sends the configuration data to the input module
** \param	inputModule420ma pointer to a _inputModule420ma struct that holds the relevant modules configuration
** \return	None
****************************************************************************************/
void InputModule420ma_Configuration(_inputModule420ma *inputModule420ma);

/***************************************************************************************
** \brief	Function that retrieves data from the input module
** \param	inputModule420ma pointer to a _inputModule420ma struct that holds the relevant modules configuration
** \return	None
****************************************************************************************/
void InputModule420ma_ReceiveValues(_inputModule420ma *inputModule420ma);



#endif /* GOCONTROLL_INPUT420MA_MODULE_H_ */