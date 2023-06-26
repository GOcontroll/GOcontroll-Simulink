/************************************************************************************//**
* \file         XcpTargetSpecific.h
* \brief        XCP functions to interface the stack with a target device
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright (c) 2019  by HAN Automotive  http://www.han.nl           All rights reserved
*  Copyright (c) 2023  by GOcontroll	  http://www.gocontroll.com   All rights reserved
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
* General information about the XCP stack functionality
****************************************************************************************
* This is a basic target independent implementation of the XCP protocol. At this time it
* only takes care of the parameter upload and variable download functions.
*
* This stack needs the following files:
*	- XcpStack.c
*	- XcpStack.h
*	- XcpTargetSpecific.c
*	- XcpTargetSpecific.h (this file)
*
* The XcpTargetSpecific.c file holds the stack entry and exit functions wich can be
* adapted to the desired transport communication bus and target device
*
* XcpStack.c <--> XcpTargetSpecific.c <--> target device
*
* To receive and send the data in a proper way, the incomming and outging data array's
* need to meet a specific build up:
* Incomming data from master:
* 	- dataReceived[0] 		= XCP command
* 	- dataReceived[1...n]	= XCP data
*
* Outgoing data to master:
* 	- dataToSend[0] 		= XCP Message length (excluded the byte for length and checksum)
* 	- dataToSend[1]			= XCP positive (0xff) or negative (0xfe) reply
* 	- dataToSend[2...n]		= XCP data that needs to be send back to the master
* 	- dataToSend[n+1]		= XCP checksum added if checksum is enabled.
****************************************************************************************

****************************************************************************************/

/****************************************************************************************
* Include guard
****************************************************************************************/
#ifndef XCP_STACK_INC_XCPTARGETSPECIFIC_H_
#define XCP_STACK_INC_XCPTARGETSPECIFIC_H_

/****************************************************************************************
* Include files
****************************************************************************************/
#include "stdio.h"
#include "XcpStack.h"


/****************************************************************************************
* XCPSTATICMEMORY
* Set the number of bytes to reservate for XCP. This setting is ignored if
* DYNAMICMEMORYALLOCATION or DYNAMICMEMORYALLOCATIONFREERTOS is set to 1
****************************************************************************************/

/****************************************************************************************
* DYNAMICMEMORYALLOCATION
* This macro definition enables the use of dynamic memory allocation in case no operating
* system is used. In this case, malloc() and free() are used for allocation.
* This functionality needs possibly additional compiler flag to enable dynamic memory allocation
* LINKER_FLAGS  -mthumb -specs=nosys.specs -specs=nano.specs
* Possibly the linker script needs to be adapted to provide the .end identifiers for memory range
*		PROVIDE ( end = . );
*		PROVIDE ( _end = . );
* Be aware to choose between DYNAMICMEMORYALLOCATION and DYNAMICMEMORYALLOCATIONFREERTOS
****************************************************************************************/

/****************************************************************************************
* DYNAMICMEMORYALLOCATIONFREERTOS
* This macro definition enables the use of dynamic memory allocation in case the FreeRTOS
* operating system is used. In this case, pvPortMalloc() and pvPortFree() are used for allocation.
* Be aware to choose between DYNAMICMEMORYALLOCATION and DYNAMICMEMORYALLOCATIONFREERTOS
****************************************************************************************/

#define XCPSTATICMEMORY					(2048)
#define DYNAMICMEMORYALLOCATION			(1)
#define DYNAMICMEMORYALLOCATIONFREERTOS	(0)

#if DYNAMICMEMORYALLOCATION == 1 &&  DYNAMICMEMORYALLOCATIONFREERTOS == 1
	#error Both DYNAMICMEMORYALLOCATION and DYNAMICMEMORYALLOCATIONFREERTOS are defined. Please define only one option
#endif





/****************************************************************************************
* Type definitions
****************************************************************************************/
typedef struct{
char channel [7];
}_eventChannel;

/****************************************************************************************
* Data declarations
****************************************************************************************/
uint32_t uniqueIdLengthAdress;
uint32_t uniqueIdStringAdress;

/****************************************************************************************
* Function prototypes
****************************************************************************************/

/****************************************************************************************
** \brief     Initializes the XCP slave communication module
** \param     aArgument pointer to the arguments
** \return    none
****************************************************************************************/
void *XcpInitialize(void *aArgument);

/***************************************************************************************
** \brief     This function sends the data from the stack to the specified communication
**			  bus.
** \param	  data A pointer to the data array that holds de data to be send to the master
** \return    Send status 1 if succeded 0 if not send.
****************************************************************************************/
uint8_t XcpSendData(uint8_t* data);

/***************************************************************************************
** \brief     This function reads the data from a specified memory location. In this
**			  function, the way to read from a memory location can be programmed.
** \param	  data A pointer to the data array to store the readed data in
** \param	  elements The number of bytes to be read from a specific memory location
** \param	  location The memory location where to read the data from.
** \return    none.
****************************************************************************************/
void XcpReadData(uint8_t* data,uint8_t elements, uint32_t*location);

/***************************************************************************************
** \brief     This function writes the data to a specified memory location. In this
**			  function, the way to write to a memory location can be programmed.
** \param	  data A pointer to the data array that holds the data that needs to be written
** \param	  elements The number of bytes to be write to a specific memory location
** \param	  location The memory location where to write the data to.
** \return    none.
****************************************************************************************/
void XcpWriteData(uint8_t *data,uint8_t elements, uint64_t location);

/***************************************************************************************
** \brief     Utility function to stop the XCP connection that is made.
** \param	  none.
** \return    none.
****************************************************************************************/
void XcpStopConnection(void);

#endif

/* End of XcpTargetSpecific.h */
