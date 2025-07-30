/************************************************************************************//**
* \file         XcpStack.h
* \brief        XCP main functions for measurement and callibrations
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright (c) 2019  by HAN Automotive  http://www.han.nl           All rights reserved
*  Copyright (c) 2024  by GOcontroll	  http://www.gocontroll.com   All rights reserved
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
*	- XcpStack.h (this file)
*	- XcpTargetSpecific.c
*	- XcpTargetSpecific.h
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
#ifndef XCPSTACK_H
#define XCPSTACK_H 1

/****************************************************************************************
* Include files
****************************************************************************************/
#include "stdint.h"		//For data types: uint8_t, uint16_t, uint32_t

/****************************************************************************************
* typedefs for the XcpStack
****************************************************************************************/

/****************************************************************************************
* Data declarations
****************************************************************************************/
extern uint32_t uniqueIdLength;
extern char uniqueIdString[];

/****************************************************************************************
* Function prototypes
****************************************************************************************/

/***************************************************************************************
** \brief     Configures the basic parameters of the XCP stack. This function can be
**            called during system initialisation.
** \param	  checksum Checksum active. 1 = checksum on  0 = checksum off
** \param	  ctoLength Set the communication length of the cto
** \param	  dtoLength Set the communication length of the dto
** \return    none.
****************************************************************************************/
void XcpDynamicConfigurator (uint8_t checksum, uint8_t ctoLength, uint16_t dtoLength);

/***************************************************************************************
** \brief     XCP core function wich determines the command from the incomming data stream
**            and direct to the right functionality
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  receivedLength The number of relevant bytes available in the current data array
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
void XcpCommunicationHandling(uint8_t *dataReceived, uint32_t receivedLength, uint8_t *dataToSend);

/***************************************************************************************
** \brief     Function that takes care of the actual data tranmission from slave to master.
**            This function needs to be called periodically. For example each 10ms. The
**			  prescaler is used in this function to increase or decrease the frequency of
**			  data upload to the master
** \param	  none.
** \return    none.
****************************************************************************************/
void XcpDataTransmission(void);


#endif
/* End of XcpStack.h */