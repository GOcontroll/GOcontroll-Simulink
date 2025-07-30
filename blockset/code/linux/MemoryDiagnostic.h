/************************************************************************************//**
* \file         MemoryDiagnostic.h
* \brief        Handle the reading and storing of diagnostic codes.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com   All rights reserved
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
#ifndef _MEMORY_DIAGNOSTIC_H
#define _MEMORY_DIAGNOSTIC_H

#include <stdint.h>                                   /* ANSI C types                  */

#define DIAGNOSTICSTART		1
#define DIAGNOSTICFREEZE	2
#define DIAGNOSTICSTOP		3

void 		MemoryDiagnostic_InitializeMemory(void);
void 		MemoryDiagnostic_WriteToMemory(uint32_t spn, uint8_t fmi, uint8_t oc, char* freezedDescription, float freezedParameter, uint8_t messageType);
uint16_t 	MemoryDiagnostic_CountDiagCodes(void);
void 		MemoryDiagnostic_DeleteAllCodes(void);
uint32_t 	MemoryDiagnostic_DiagCodeOnIndex(uint16_t index);
void 		MemoryDiagnostic_DeleteSingleCode(uint32_t spn, uint8_t fmi, uint8_t oc);

#endif // _MEMORY_DIAGNOSTIC_H

/************************************ end of MemoryDiagnostic.h ******************************/