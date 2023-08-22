/************************************************************************************//**
* \file         UDP.h
* \brief        Handle sending and receiving UDP messages
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
#ifndef UDP_H
#define UDP_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "SYS_config.h"

#ifndef UDPBUFFNUM
#define UDPBUFFNUM 0
#endif

#ifndef UDPBUFFSIZE
#define UDPBUFFSIZE 0
#endif


/* struct that holds a UDP buffer */
struct UDP_Buffer{
    uint8_t buffer[UDPBUFFSIZE];
    uint8_t new_message;
};

/* Globals */

int socket_desc;

/**
 * @brief Initialize the udp socket
 * 
 * @param port port to listen on
 * @param ip ip to listen on
 * @param broadcast allow for broadcasting
 */
void UdpInitialize(uint16_t port, char* ip, int broadcast);


/**
 * @brief empty the linux socket receive buffer 
 */
void UdpReceive(void);


/**
 * @brief Get a message from the udp buffers
 * 
 * @param id the identifier of the message desired
 * @param message the message
 * @param new_message was there a new message
 */
void getUdpBuffer(size_t id, uint8_t* message, uint8_t* new_message);

/**
 * @brief send a udp packet
 * 
 * @param port port to which it needs to be sent
 * @param ip ip to which it needs to be sent
 * @param message the packet
 * @param message_length packet length
 */
void UdpSend(uint16_t port, char* ip, uint8_t* message, size_t message_length);

/**
 * @brief init the udp buffers
 */
void initUdpBuffers(void);

/**
 * @brief kill the udp socket and free the buffers 
 */
void UdpTerminate(void);

#endif // __UDP_H

/************************************ end of UDP.h ******************************/