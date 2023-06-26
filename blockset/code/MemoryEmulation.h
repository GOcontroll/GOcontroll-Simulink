/************************************************************************************//**
* \file         MemoryEmulation.h
* \brief        Handle the reading and writing of key value pairs to the filesystem
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2023 (c) by GOcontroll      http://www.gocontroll.com   All rights reserved
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
#ifndef _MEMORY_EMULATION_H
#define _MEMORY_EMULATION_H

#include <stdint.h>                                   /* ANSI C types                  */

/***************************************************************************************
** \brief	Function that needs to be called once during initialisation to create the folder
**			to store data.
** \param	None
** \return	None.
****************************************************************************************/
void MemoryEmulation_InitializeMemory(void);

/***************************************************************************************
** \brief	Writes a key-value pair to memory
** \param	key file path, like "/usr/mem-sim/key"(non-volatile memory) or "/dev/shm/key"(volatile memory)
** \param	value the value to write to memory
** \param	oldValue previous value, gets automatically updated when the function is called.
** \return	None
****************************************************************************************/
void MemoryEmulation_WriteToMemory(char* key, float value, float* oldValue);

/***************************************************************************************
** \brief	reads a key-value pair from memory
** \param	key file path, like "/usr/mem-sim/key"(non-volatile memory) or "/dev/shm/key"(volatile memory)
** \param	value pointer to the value that gets read from memory
** \return	None
****************************************************************************************/
void MemoryEmulation_ReadFromMemory(char* key, float *value);

#endif // _MEMORY_EMULATION_H

/************************************ end of MemoryEmulation.h ******************************/