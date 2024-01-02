/************************************************************************************//**
* \file			CANdriver.h
* \brief		Utility functions to interface the GOcontroll CAN busses.
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



#ifndef CANDRIVER_H
#define CANDRIVER_H

/****************************************************************************************
* Include files
****************************************************************************************/
/** CAN driver header file **/
#include <stdbool.h>
#include "stdio.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "arpa/inet.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "unistd.h"
#include "netinet/in.h"
#include "netdb.h"
#include "linux/can.h"
#include "linux/can/raw.h"
#include "sys/ioctl.h"
#include "net/if.h"
#include "SYS_config.h"


/** Defines **/

#ifndef CANBUFSIZE
#define CANBUFSIZE 10
#endif

/** Typedefs **/
/* Struct to hold CAN message information */
struct CANbuffer_t{
	canid_t can_id;
	bool newFlag;
	struct can_frame frame;
};

typedef struct{
	uint8_t socketCreated[4];
	uint16_t initErrorCounter[4];
	int socket[4];
}_canConnection;

_canConnection canConnection;

/** Global variables **/
int can_socket; /* File descriptor for CAN socket */
struct can_filter CANfilter[4][CANBUFSIZE]; /* CANfilter to filter receive ID's */
struct CANbuffer_t CANbuffer[4][CANBUFSIZE]; /* CANbuffer to hold received messages */

/** Function prototypes **/

/***************************************************************************************
** \brief     Creates a CAN socket to be able to read and write CAN messages from.
** \param     canInterface number of the can bus
** \return    nothing
****************************************************************************************/
void CANsocket(uint8_t canInterface);

/***************************************************************************************
** \brief     Gets a CAN message from the HANcoder receive buffer.
** \param     can_id the identifier of the can message to get.
** \param     canInterface the can interface that the message is supposed to be received on.
** \param     frame the received can frame will be stored at this pointer.
** \param	  newFlag a flag that indicates whether the retrieved message is new.
** \return    int which is -1 when an error has occurred or 0 if there is no error.
****************************************************************************************/
int getCANbuffer(canid_t can_id,uint8_t canInterface, struct can_frame **frame, bool *newFlag);

/***************************************************************************************
** \brief     Create a filter for the socketCAN.
** \param     oldCANid CAN identifier, to be overwritten
** \param     canInterface CAN identifier, to replace the old one
** \param 	  newCANid The new identifier to set in the buffer
** \return    -1 when an error has occurred, 0 upon success.
****************************************************************************************/
int updateCANreceiveFilter(canid_t oldCANid,uint8_t canInterface, canid_t newCANid);

/***************************************************************************************
** \brief     Change the identifier in the CANbuffer.
** \param     oldCANid CAN identifier, to be overwritten
** \param 	  canInterface the CAN interface of which the buffer needs to be altered
** \param     newCANid CAN identifier, to replace the old one
** \return    int which is -1 when an error has occurred
****************************************************************************************/
int updateCANreceiveBuffer(canid_t oldCANid,uint8_t canInterface, canid_t newCANid);

/***************************************************************************************
** \brief     Checks for messages in the socketCAN buffer and puts them in the HANcoder
** 			  buffer.
** \param     void
** \return    -1 when an error has occurred, 0 upon success
****************************************************************************************/
int serveCANconnection(void);

/** Extra info **/
/* canid_t
 * Controller Area Network Identifier structure
 *
 * bit 0-28	: CAN identifier (11/29 bit)
 * bit 29	: error message frame flag (0 = data frame, 1 = error message)
 * bit 30	: remote transmission request flag (1 = rtr frame)
 * bit 31	: frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */

 #endif /* CANDRIVER_H */
