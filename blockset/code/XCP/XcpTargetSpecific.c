/************************************************************************************//**
* \file         XcpTargetSpecific.c
* \brief        XCP functions to interface the stack with a target device
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
*	- XcpStack.h
*	- XcpTargetSpecific.c (this file)
*	- XcpTargetSpecific.h
*
* The XcpTargetSpecific.c file holds the stack entry and exit functions wich can be
* adapted to the desired transport communication bus and target device
*
* XcpStack.c <--> XcpTargetSpecific.c <--> target device
*
* To receive and send the data in a proper way, the incoming and outgoing data array's
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
* Include files
****************************************************************************************/
#include "XcpStack.h"
#include "XcpTargetSpecific.h"
#include <netinet/in.h>								/* For socketaddr_in				*/
#include <sys/socket.h>								/* TCP and UDP socket				*/
#include <netinet/ip.h>
#include <arpa/inet.h>								/* For htons, htonl, ntohs, ntohl	*/
#include <sys/types.h>
#include <stdio.h>									/* For fprintf()					*/
#include <netdb.h>
#include <linux/in.h>
#include <string.h>									/* Needed to display errno			*/
#include <errno.h>									/* For errno						*/
#include <unistd.h>									/* For read() and write()			*/
#include "SYS_config.h"								/* For the XCP_PORT_NUM				*/
#include <netinet/tcp.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define XCPUSB 1
#define XCPETH 2
#define XCPCAN 3

#define XCPMAXDTOLENGTH 255
#define XCPMAXCTOLENGTH 255
#define DEBUG 0
#define TIMEOUT 1

/****************************************************************************************
* Type definitions
****************************************************************************************/

typedef struct{
uint32_t xcpDtoId;
uint8_t xcpMacCanChannel;
}_xcpCanParameters;

/** \brief XCP DTO packet type with TCP/IP header  */
typedef union
{
  struct
  {
    uint16_t len;
    uint16_t counter;
    uint8_t data[XCPMAXDTOLENGTH];
  } s;
  uint8_t raw[4+XCPMAXDTOLENGTH];
} __attribute__((packed)) tMacNetXcpDtoPacket;

/** \brief XCP CTO packet type with TCP/IP header  */
typedef union
{
  struct
  {
    uint16_t len;
    uint16_t counter;
    uint8_t data[XCPMAXCTOLENGTH];
  } s;
  uint8_t raw[4+XCPMAXCTOLENGTH];
} __attribute__((packed)) tMacNetXcpCtoPacket;

_xcpCanParameters xcpCanParameters;


//TODO Channels must be assigned during timer creation.
_eventChannel eventChannel[3] ={
		{"EvChnl1"},
		{"EvChnl2"},
		{"EvChnl3"}};

/****************************************************************************************
* Function prototypes
****************************************************************************************/
int ServeXcpConnection(void);

void expired(union sigval timer_data);

/****************************************************************************************
* Data declarations
****************************************************************************************/
/** \brief File descriptor for the XCP connection with the client. */
int xcpSocketResult, XcpSocket;
static struct sockaddr_in addr_HT;
static int slen = sizeof(addr_HT);
static uint8_t dataToSend[120] = {0};
static int timeout_sec = 0;
bool timeout_active = false;

uint32_t uniqueIdLengthAdress;
uint32_t uniqueIdStringAdress;
int XcpConnection_fd;

/** \brief Variables that hold the ECU identifier and the length of the string */
uint32_t uniqueIdLength = (uint32_t)kXcpStationIdLength;
char uniqueIdString[] = kXcpStationIdString;

timer_t countDownTimer = 0;

struct t_eventData{
    int myData;
};
struct t_eventData eventData = { .myData = 0 };
static struct sigevent sev = {0};
struct itimerspec its;

/****************************************************************************************
** \brief     Function to call when timeout occurred
** \param     timer_data
** \return    none
**
****************************************************************************************/
void expired(union sigval timer_data){
	char data[3];
	// Somehow stop XCP?
	fprintf(stderr,"Timer expired, killing XCP\n");
	data[0] = 0xdd; //START_STOP_DAQ_LIST
	data[1] = 0x00; //Mode 00 = stop DAQ list
	//data[2] = 0x01; // DAQ list 1
	XcpCommunicationHandling((uint8_t *) &data, (uint32_t) 3, (uint8_t *) &dataToSend); //Call XCP stack for command received
	//data[2] = 0x02; // DAQ list 2
	//XcpCommunicationHandling((uint8_t *) &data, (uint32_t) 3, (uint8_t *) &dataToSend); //Call XCP stack for command received
	//data[2] = 0x03; // DAQ list 3
	//XcpCommunicationHandling((uint8_t *) &data, (uint32_t) 3, (uint8_t *) &dataToSend); //Call XCP stack for command received
	its.it_value.tv_sec  = 0;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec  = 0;
	its.it_interval.tv_nsec = 0;
	int res = timer_settime(countDownTimer, 0, &its, NULL);
	if (res != 0){
		fprintf(stderr, "Error timer_settime: %s\n", strerror(errno));
	}
}

/****************************************************************************************
** \brief     Initializes the XCP slave communication module, tcp/ip
** \param     aArgument pointer to the arguments
** \return    none
****************************************************************************************/
void *XcpInitialize_tcp(void *aArgument){
	struct sockaddr_in XcpSocketAddr,XCPclientAddr;	/* Structs to hold the XCP address	*/
	//int XcpSocket;							/* File descriptors					*/
	socklen_t XCPclientAddrLen; 					/* Length of XCP client address		*/
	char XcpClientAddrStr[INET_ADDRSTRLEN];			/* IP address of client				*/
	XCPclientAddrLen = sizeof(struct sockaddr_in);	/* Address length, needed to store	*/

	#if defined(XCP_ENABLE_DAQ_TIMESTAMP)
		/* reset the timestamp */
		xcpTimestampValue = 0;
	#endif

	XcpDynamicConfigurator (0, 255, 255);

	/* Initialize the TCP socket for XCP communication */
	XcpSocket = socket(AF_INET, SOCK_STREAM, 0);	/* Create the socket */
	if (XcpSocket == -1)
		fprintf(stderr,"Error in XCPinit; create socket: %s\n",strerror(errno));
	else if(DEBUG == 1)
		fprintf(stderr,"Successfully created socket\n");

	/* Enable possibility to reconnect next time */
	if (setsockopt(XcpSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    fprintf(stderr,"setsockopt(SO_REUSEADDR) failed");

	memset(&XcpSocketAddr, 0, sizeof(struct sockaddr_in)); 	/* Clear the socket address */
	XcpSocketAddr.sin_family = AF_INET;						/* Set as internet socket */
	XcpSocketAddr.sin_addr.s_addr = INADDR_ANY; 			/* Accept any IP address */
	XcpSocketAddr.sin_port = htons(XCP_PORT_NUM); 			/* Host to network byte order, port nr */

	xcpSocketResult = bind(XcpSocket,(struct sockaddr *)&XcpSocketAddr,sizeof(XcpSocketAddr)); 	/* Bind the socket to an address */
	if(xcpSocketResult == -1)
		fprintf(stderr,"Error in XCPinit; bind socket: %s\n",strerror(errno));
	else if(DEBUG == 1)
		fprintf(stderr,"Successfully bound socket\n");

	xcpSocketResult = listen(XcpSocket, SOMAXCONN); 	/* Set the socket to listen mode */
	if(xcpSocketResult == -1)
		fprintf(stderr,"Error in XCPinit; listen to socket: %s\n",strerror(errno));
	else if(DEBUG == 1)
		fprintf(stderr,"Successfully listening to socket\n");

	unsigned int timeout = 5000;

	if (setsockopt(XcpSocket, SOL_TCP, TCP_USER_TIMEOUT, &timeout, sizeof(timeout))<0)
		fprintf(stderr,"setsockopt(SO_SNDTIMEO) failed");

	for(;;)
	{
		/* From here the actual XCP communication is started */
		XcpConnection_fd = accept(XcpSocket, (struct sockaddr *)&XCPclientAddr, &XCPclientAddrLen); /* Accept incoming from all IP addresses */
		if(XcpConnection_fd == -1){
			fprintf(stderr,"Error in XCPinit; accepting incoming connection%s\n",strerror(errno));
			if(close(XcpConnection_fd)==-1) /* Close connection when accept returns an error */
				fprintf(stderr,"XcpInitialize: error while trying to close the connection: %s\n",strerror(errno));
			continue;
		}else if(DEBUG == 1)
			fprintf(stderr,"Successfully accepted incoming connection from client\n");

		if(inet_ntop(AF_INET,&XCPclientAddr.sin_addr.s_addr,XcpClientAddrStr,sizeof(XcpClientAddrStr))!=NULL)
			fprintf(stderr,"Incoming connection from client: %s \n",inet_ntoa(XCPclientAddr.sin_addr));
		else
			fprintf(stderr,"Unable to get client IP address: %s\n",strerror(errno));

		/* XCP Task */
		while(ServeXcpConnection() != -1){}
		if(DEBUG == 1)
			fprintf(stderr,"ServeXcpConnection() returned an error\n");
		if(close(XcpConnection_fd)==-1) /* Close connection when ServeXcpConnection() returns an error */
			fprintf(stderr,"XcpInitialize: error while trying to close the connection: %s\n",strerror(errno));
	}

return 0;
}

/****************************************************************************************
** \brief     Initializes the XCP slave communication module, udp/ip
** \param     pointer to the arguments
** \return    none
**
****************************************************************************************/
void *XcpInitialize_udp(void *aArgument){
	struct sockaddr_in XcpSocketAddr;	/* Structs to hold the XCP address	*/
	int result;							/* File descriptors					*/
	timeout_active = true;
	
	#if defined(XCP_ENABLE_DAQ_TIMESTAMP)
		/* reset the timestamp */
		xcpTimestampValue = 0;
	#endif
	
	XcpDynamicConfigurator (0, 255, 255);

	/* Initialize the UDP socket for XCP communication */
	XcpConnection_fd = socket(AF_INET, SOCK_DGRAM, 0);	/* Create the socket */
	if (XcpConnection_fd == -1)
		fprintf(stderr,"Error in XCPinit; create socket: %s\n",strerror(errno));
	else if(DEBUG == 1)
		fprintf(stderr,"Successfully created socket\n");
	
	memset(&XcpSocketAddr, 0, sizeof(struct sockaddr_in)); 	/* Clear the socket address */
	XcpSocketAddr.sin_family = AF_INET;						/* Set as internet socket */
	XcpSocketAddr.sin_addr.s_addr = INADDR_ANY; 			/* Accept any IP address */
	XcpSocketAddr.sin_port = htons(XCP_PORT_NUM); 			/* Host to network byte order short, port nr */
	
	result = bind(XcpConnection_fd,(struct sockaddr *)&XcpSocketAddr,sizeof(XcpSocketAddr)); 	/* Bind the socket to an address */
	if(result == -1)
		fprintf(stderr,"Error in XCPinit; bind socket: %s\n",strerror(errno));
	else if(DEBUG == 1)
		fprintf(stderr,"Successfully bound socket\n");

	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = &expired;
	sev.sigev_value.sival_ptr = &eventData;
	int res = timer_create(CLOCK_REALTIME, &sev, &countDownTimer);
	if (res != 0){
		fprintf(stderr, "Error timer_create: %s\n", strerror(errno));
	}

	for(;;)
	{
		/* XCP Task */
		while(ServeXcpConnection() != -1){}
		if(DEBUG == 1)
			fprintf(stderr,"ServeXcpConnection() returned an error\n");
	}

return 0;
}

/****************************************************************************************
** \brief     Checks for messages from the XCP Client and calls XCP command accordingly
** \param     void
** \param     void
** \return    int which is -1 when an error occurred
****************************************************************************************/
int ServeXcpConnection(void)
{

	tMacNetXcpCtoPacket ctoPacket;
	ssize_t readCnt;
	uint8_t buf[32]; /* Buffer to hold XCP message */

	/* Try to read bytes from buffer and keep reading until empty */
	do{
	/* Check for received messages */
	readCnt = recvfrom(XcpConnection_fd,(void *)buf,4,MSG_PEEK, (struct sockaddr * restrict) &addr_HT, (socklen_t * restrict) &slen);/* Receive first 4 bytes to be able to read the XCP counter and length*/
	#if(DEBUG==1)
	fprintf(stderr,"recv1: %li\n", readCnt);
	#endif
	if(readCnt != 4){
		if(readCnt == -1)
		{
			fprintf(stderr,"ServeXcpConnection failed to read first 4 bytes: %s\n",strerror(errno));
			return -1;
		}else if(readCnt == 0){/* Socket is closed with FIN */
			if(DEBUG==1)
				fprintf(stderr,"ServeXcpConnection: Connection closed\n");
			return -1;
		}
	}else{
		/* Copy the first 4 bytes into the CTO packet buffer, which is the XCP header with
         * the length of the counter.
		 */
		memcpy(ctoPacket.raw, (void *)buf, 4);

		/* Now we have the length we can read the rest of the message */
		readCnt = recvfrom(XcpConnection_fd,(uint8_t *)&ctoPacket,(size_t)ctoPacket.s.len+4,0, (struct sockaddr * restrict) &addr_HT, (socklen_t * restrict) &slen);/* Read the rest of the bytes in the receive buffer */
		#if(DEBUG==1)
		fprintf(stderr,"recv2: %li\n", readCnt);
		#endif
		if(readCnt == 0) {
			fprintf(stderr, "Connection lost, closing socket...\n");
			return -1;
		}else if(readCnt != ctoPacket.s.len + 4){
			fprintf(stderr,"ServeXcpConnection failed to read rest of message: %s\n",strerror(errno));
		}else{
			#if(DEBUG ==1)
				//Print out the data of the message
				fprintf(stderr,"Message from HANtune: ");
				for(int i=0;i<ctoPacket.s.len;i++){
				fprintf(stderr,"%x ",ctoPacket.s.data[i]);
				}
				fprintf(stderr,"\n");
			#endif
			if(timeout_active){
				if (ctoPacket.s.data[0] == 0xFE){//Disconnect message, turn off timer
					timeout_sec = 0;
				}
				
				/* Set timer values, it_value and it_interval must not be zero */
				its.it_value.tv_sec  = timeout_sec;
				its.it_value.tv_nsec = 0;
				its.it_interval.tv_sec  = timeout_sec;
				its.it_interval.tv_nsec = 0;
				
				int res = timer_settime(countDownTimer, 0, &its, NULL);
				if (res != 0){
					fprintf(stderr, "Error timer_settime: %s\n", strerror(errno));
				}
			}
			XcpCommunicationHandling((uint8_t *) &ctoPacket.s.data, (uint32_t) ctoPacket.s.len, (uint8_t *) &dataToSend); //Call XCP stack for command received
		}
	}
	}while(readCnt > 0); /* End of do while loop */
	return 0;
}

/***************************************************************************************
** \brief     This function sends the data from the stack to the specified communication
**			  bus.
** \param	  data A pointer to the data array that holds de data to be send to the master
** \return    Send status 1 if succeded 0 if not send.
****************************************************************************************/
uint8_t XcpSendData(uint8_t *data)
{
	static uint16_t CTR = 0; /* Counter according to XCP over TCP specification */
	tMacNetXcpDtoPacket dtoPacket;
	ssize_t res;

	/* Set counter and length in DTO packet */
	dtoPacket.s.len = (uint16_t) *data; //First byte holds the data length
	dtoPacket.s.counter = CTR;
	/* Update the counter value for each new packet */
	CTR++;
	/* Copy data to packet */
	memcpy(dtoPacket.s.data, data +1, dtoPacket.s.len);
	
	addr_HT.sin_port = htons(50000); //TODO port now hardcoded: should be setting in Simulink
	/* Write data to XCP connection */
	res = sendto(XcpConnection_fd, (void *)&dtoPacket.raw[0], (size_t) dtoPacket.s.len + 4, MSG_DONTWAIT,(struct sockaddr*) &addr_HT, slen);
	if (res != (dtoPacket.s.len + 4))
	{
		#if(DEBUG==1)
		fprintf(stderr, "sent %ld bytes, which should have been %d\n", res, dtoPacket.s.len + 4);
		fprintf(stderr,"XcpSendData: Error in sending: %s\n",strerror(errno));
		#endif
		/* Disconnect to make sure sending stops */
		if(close(XcpConnection_fd)==-1) /* Close connection when ServeXcpConnection() returns an error */
			fprintf(stderr,"XcpSendData: error while trying to close the connection: %s\nLikely already closed in XcpInitialize.",strerror(errno));
		return -1; //Message not send: error
	}
	return 0;
	
} /*** end of XcpSendData ***/

/***************************************************************************************
** \brief     This function reads the data from a specified memory location. In this
**			  function, the way to read from a memory location can be programmed.
** \param	  data A pointer to the data array to store the readed data in
** \param	  elements The number of bytes to be read from a specific memory location
** \param	  location The memory location where to read the data from.
** \return    none.
****************************************************************************************/
void XcpReadData(uint8_t* data,uint8_t elements, uint32_t* location)
{
	switch(elements)
	{
	case 1:				*(uint8_t*)data 	= *(uint8_t*)location;				break;
	case 2:				*(uint16_t*)data 	= *(uint16_t*)location;				break;
	case 4:				*(uint32_t*)data 	= *(uint32_t*)location;				break;
	case 8:				*(uint64_t*)data 	= *(uint64_t*)location;				break;
	}
}

/***************************************************************************************
** \brief     This function writes the data to a specified memory location. In this
**			  function, the way to write to a memory location can be programmed.
** \param	  data A pointer to the data array that holds the data that needs to be written
** \param	  elements The number of bytes to be write to a specific memory location
** \param	  location The memory location where to write the data to.
** \return    none.
****************************************************************************************/
void XcpWriteData(uint8_t *data, uint8_t elements, uint64_t location)
{
//TODO check for write protected area's in memory

	switch(elements)
	{
	case 1:				*(uint8_t*)location = *(uint8_t*)&data[0];				break;
	case 2:				*(uint16_t *)location =  *(uint16_t*)&data[0];			break;
	case 4:				*(uint32_t *)location =  *(uint32_t*)&data[0];			break;
	case 8:				*(uint64_t *)location =  *(uint64_t*)&data[0];			break;
	}
}

/***************************************************************************************
** \brief     Utility function to stop the XCP connection that is made.
** \param	  none.
** \return    none.
****************************************************************************************/
void XcpStopConnection(void)
{
	close(XcpSocket);

	#if(DEBUG == 1)
		printf("XCP socket closed\n");
	#endif
}

/************************************************************************************//**
** \brief     This function executes a specific user command.
** \param	  A pointer to the data array that holds the data that was received
** \return    none.
**
****************************************************************************************/
uint8_t XcpUserCmd(uint8_t *dataReceived)
{
	switch (dataReceived[1])
	{
		case 0x10:
			timeout_sec = (int)(dataReceived[2] << 8) + dataReceived[3];
			timeout_sec = 2 * timeout_sec; // Make sure the timeout in the target is
			// longer than the one in HANtune
			
			/* Set timer values, it_value and it_interval */
			its.it_value.tv_sec  = timeout_sec;
			its.it_interval.tv_sec  = timeout_sec;
			
			int res = timer_settime(countDownTimer, 0, &its, NULL);
			if (res != 0){
				fprintf(stderr, "Error timer_settime: %s\n", strerror(errno));
			} else if(DEBUG==1){
				fprintf(stderr,"New timeout value received: %d seconds\n",timeout_sec);
			}
			
			return 0;
		case 0x11:			
			return 0;
		default:
			return -1;
	}
}

/* End of XcpTargetSpecific.c */
