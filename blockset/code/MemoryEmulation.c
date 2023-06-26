/************************************************************************************//**
* \file         MemoryEmulation.c
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
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "MemoryEmulation.h"

/****************************************************************************************/

void MemoryEmulation_InitializeMemory(void)
{
	/* Check if folder is already available. If not, create one */
	struct stat st = {0};

	if (stat("/usr/mem-sim", &st) == -1) {
    mkdir("/usr/mem-sim", 0555);
	}
}

/****************************************************************************************/

void MemoryEmulation_WriteToMemory(char* key, float value, float* oldValue)
{
	/* Check if old value is valid */
	if(oldValue != NULL)
	{
		/* Check if signal is changed. If not, quit function */
		if(value == *oldValue){return;}

		/* Value is changed so update the old value with the new one */
		*oldValue = value;
	}
	/* If oldValue has no valid pointer, it means initialization call */
	else
	{
		/* Check if a value is already stored. If yes, quit function */
		float dummy = 0xffffffff;
		MemoryEmulation_ReadFromMemory(key,&dummy);
		/* If dummy is changed, the value is already there so ignore init value */
		if(dummy != 0xffffffff)
		{
		return;
		}
	}

	/* Create string from value */
	char valueString[15];
	gcvt(value, 10, valueString);

	/* Write string to file */
	int fileId = 0;
	/* Open file to write data to */
	fileId = open(key, O_WRONLY | O_CREAT | O_NONBLOCK | O_TRUNC, 0555);

	/* Write the data to the file*/
	write(fileId, valueString, strlen(valueString));

	/* Close the file descriptor */
	close(fileId);
}

/****************************************************************************************/

void MemoryEmulation_ReadFromMemory(char* key, float *value)
{
	int fileId = 0;

	if((fileId = open(key, O_RDONLY | O_NONBLOCK)) <= 0)
	{
	/* Here means the file is not opend properly */
	/* To be sure it is closed, force a close */
	close(fileId);
	return;
	}

	/* If we are here, we have acces to a valid file */
	char tempValue[15] = {0};
	/* Read the content of the file */
	read(fileId, &tempValue[0], 15);
	/* Close the file descriptor */
	close(fileId);
	/* Write the content as a float and return*/
	*value = strtof(tempValue, NULL);
}



