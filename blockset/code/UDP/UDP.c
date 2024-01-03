/************************************************************************************//**
* \file         UDP.c
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

#include "UDP.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include "SYS_config.h"

struct UDP_Buffer udp_buffers[UDPBUFFNUM];

void UdpInitialize(uint16_t port, char* ip, int broadcast) {
	struct sockaddr_in server_addr;

	// Create UDP socket:
	socket_desc = socket(AF_INET, SOCK_DGRAM, 0);

	if(socket_desc < 0){
		fprintf(stderr, "Error while creating UDP socket\n");
		return;
	}

	if (broadcast) {
		if (setsockopt(socket_desc, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)))
			fprintf(stderr, "Could set socket to broadcast\n");
	}

	// Set port and IP:
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);

	// Bind to the set port and IP:
	if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		fprintf(stderr,"Couldn't bind to the port\n");
	}
	return ;
}

void UdpReceive(void) {
	uint8_t client_message[UDPBUFFSIZE];
	// Receive client's message:
	while (recvfrom(socket_desc, client_message, sizeof(client_message), MSG_DONTWAIT, NULL, NULL) > 0){
		if client_message[0] < UDPBUFFNUM { //prevent segfault
			memcpy(udp_buffers[client_message[0]].buffer, client_message, UDPBUFFSIZE);
			udp_buffers[client_message[0]].new_message = true;
		}
	}
}

void getUdpBuffer(size_t id, uint8_t* message, size_t message_len, uint8_t* new_message) {
	if (id < UDPBUFFNUM) {
		memcpy(message, udp_buffers[id].buffer, message_len);
		*new_message = udp_buffers[id].new_message;
		udp_buffers[id].new_message = false;
	}
}

void UdpSend(uint16_t port, char* ip, uint8_t id, uint8_t* message, size_t message_length) {
	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(port);
	client_addr.sin_addr.s_addr = inet_addr(ip);
	int client_struct_length = sizeof(client_addr);
	message[0] = id;

	if (sendto(socket_desc, message, message_length, 0,
			(struct sockaddr*)&client_addr, client_struct_length) < 0){
		fprintf(stderr, "Can't send UDP packet\n");
	}
}

void initUdpBuffers(void) {
	for (int i = 0; i < UDPBUFFNUM; i++){
		memset(udp_buffers[i].buffer, 0 , sizeof(udp_buffers[i].buffer));
	}
}

void UdpTerminate(void) {
	// Close the socket
	close(socket_desc);
}


/************************************ end of UDP.c ******************************/