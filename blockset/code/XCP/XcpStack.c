/***************************************************************************************
* \file         XcpStack.c
* \brief        XCP main functions for measurement and callibrations
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright (c) 2019  by HAN Automotive  http://www.han.nl           All rights reserved
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
*	- XcpStack.c (this file)
*	- XcpStack.h
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
* Include files
****************************************************************************************/
#include "XcpStack.h"
#include "XcpTargetSpecific.h"
#include <stdlib.h>

#if DYNAMICMEMORYALLOCATIONFREERTOS == 1
	#include "os.h"
#endif

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* switch on debug message if there is a ouput stream available */
#define DEBUGINFORMATION 				(0)

/* XCP version definitions (MSB bytes only) */
#define XCPPROTOCOLLAYERVERSION 		0x01
#define XCPTRANSLORTLAYERVERSION		0x01

/* XCP CTO and DTO message length definitions */
#define XCPMAXDTOLENGTH					(32)
#define XCPMAXCTOLENGTH					(32)

/* Defines for the type of packet to be send by the stack */
#define VARIABELESEND					0x01
#define PACKETSEND						0x02

/* XCP Master commands */
#define XCPCONNECT						0xff
#define XCPDISCONNECT					0xfe
#define XCPSTATUS						0xfd
#define XCPSYNCHRONISATION				0xfc
#define XCPCOMMUNICATIONMODEINFO		0xfb
#define XCPGETID						0xfa
#define XCPMTA							0xf6
#define XCPUPLOAD						0xf5
#define XCPSHORTUPLOAD					0xf4
#define XCPUSERCMD						0xf1
#define XCPDOWNLOAD						0xf0

/* XCP DAQ list commands */
#define XCPSETDAQPOINTER				0xe2
#define XCPWRITEDAQ						0xe1
#define XCPDAQLISTMODE					0xe0
#define XCPSTARTSTOPDAQLIST				0xde
#define XCPSTARTSTOPSYNCH				0xdd

#define XCPGETDAQPROCESSORINFO			0xda
#define XCPGETDAQRESOLUTIONINFO			0xd9
#define XCPGETDAQLISTINFO				0xd8
#define XCPGETDAQEVENTINFO				0xd7

#define XCPFREEDAQ						0xd6
#define XCPALLOCDAQ						0xd5
#define XCPALLOCODT						0xd4
#define XCPALLOCODTENTRY				0xd3

/* XCP Error codes */
#define XCPERRORCMDCYNC					0x00
#define XCPERRCMDUNKNOWN				0x20
#define XCPERRORMEMORYOVERFLOW			0x30

/****************************************************************************************
* Type definitions
****************************************************************************************/
typedef struct{
uint8_t 	bitOffset;
uint8_t 	sizeOfDaqElement;
uint8_t 	adressExtension;
uint32_t 	adressOfDaqElement;
}__attribute__((packed)) _entry;

typedef struct{
#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
_entry 		*entry;
#else
uint32_t	entry1Location;
#endif
uint8_t		nrOfOdtEntries;
}__attribute__((packed)) _odt;

typedef struct{
#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
_odt 		*odt;
#else
uint32_t	odt1Location;
#endif
uint8_t		listStatus;
uint8_t		prescalerSet;
uint8_t		prescalerActual;
uint8_t		nrOfOdts;
uint8_t		odtId;
}__attribute__((packed)) _daq;

#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
_daq *daq;
#endif

typedef struct{
uint32_t 	idRequest;
uint8_t 	daqEventRequest;
uint16_t 	nrOfDaqLists;
uint16_t 	daqList;
uint8_t 	odt;
uint8_t 	odtEntry;
uint8_t		daqMode;
uint16_t 	eventChannel;
uint8_t		transRatePres;
uint8_t		daqListPriority;
uint8_t 	status;
uint8_t		checksum;
uint8_t 	ctoLength;
uint16_t 	dtoLength;
uint8_t		active;
uint8_t 	odtIdCounter;
}_xcpCommunication;

_xcpCommunication xcpCommunication = {0};

typedef struct{
uint8_t 	adressExtension;
uint32_t 	adress;
}_xcpWrite;

_xcpWrite xcpWrite;

#if DYNAMICMEMORYALLOCATION == 0 && DYNAMICMEMORYALLOCATIONFREERTOS == 0
void* memoryStackPointer;
uint8_t xcpMemoryStack[XCPSTATICMEMORY];
#endif

static uint32_t xcpMemoryUsage;

/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void XcpPositiveResponse(uint8_t *dataToSend);
static void XcpNegativeResponse(uint8_t *dataToSend, uint8_t errorCode);
static void XcpConnectReply(uint8_t *dataToSend);
static void XcpDisconnectReply(uint8_t *dataToSend);
static void XcpStatusReply(uint8_t *dataToSend);
static void XcpSynchronisation(uint8_t *dataToSend);
static void xcpCommunicationModeReply(uint8_t *dataToSend);
static void XcpGetIdReply(uint8_t *dataToSend);
static void XcpSetMta(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpUploadReply(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpShortUploadReply(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpUserCommand(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpDownload(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpSetDaqPointer(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpWriteDaq(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpSetDaqListMode(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpStartStopDaqList(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpStartStopSynch(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpDaqProcessorInfoReply(uint8_t *dataToSend);
static void XcpDaqEventInfoReply(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpDaqResolutionInfoReply(uint8_t *dataToSend);
static void XcpFreeDaq( uint8_t *dataToSend);
static void XcpAllocDaq(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpAllocOdt(uint8_t *dataReceived, uint8_t *dataToSend);
static void XcpAllocOdtEntry(uint8_t *dataReceived, uint8_t *dataToSend);
#if DYNAMICMEMORYALLOCATION == 0 && DYNAMICMEMORYALLOCATIONFREERTOS == 0
static _daq* XcpGetDaq(uint16_t daqList);
static _odt* XcpGetOdt(uint16_t daqList, uint8_t odtValue);
static _entry* XcpGetOdtEntry(uint16_t daqList, uint8_t odtValue, uint8_t odtEntry);
#endif
static void XcpCalculateChecksum(uint8_t *dataToSend);
static void XcpStopDataTransmission(void);

/****************************************************************************************
* Data declarations
****************************************************************************************/
extern _eventChannel eventChannel[];

/***************************************************************************************
** \brief     Configures the basic parameters of the XCP stack. This function can be
**            called during system initialisation.
** \param	  checksum Checksum active. 1 = checksum on  0 = checksum off
** \param	  ctoLength Set the communication length of the cto
** \param	  dtoLength Set the communication length of the dto
** \return    none.
****************************************************************************************/
void XcpDynamicConfigurator (uint8_t checksum, uint8_t ctoLength, uint16_t dtoLength)
{
xcpCommunication.checksum = checksum;
xcpCommunication.ctoLength = ctoLength;
xcpCommunication.dtoLength = dtoLength;
}

/***************************************************************************************
** \brief     XCP core function wich determines the command from the incomming data stream
**            and direct to the right functionality
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  receivedLength The number of relevant bytes available in the current data array
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
void XcpCommunicationHandling(uint8_t *dataReceived, uint32_t receivedLength, uint8_t *dataToSend)
{
	switch(dataReceived[0]) // Command packet code
	{
		/* XCP Master commands */
		case XCPCONNECT:				XcpConnectReply(dataToSend); 							break;

		case XCPDISCONNECT:				XcpDisconnectReply(dataToSend);							break;

		case XCPSTATUS:					XcpStatusReply(dataToSend);								break;

		case XCPSYNCHRONISATION:		XcpSynchronisation(dataToSend);							break;

		case XCPCOMMUNICATIONMODEINFO:	xcpCommunicationModeReply(dataToSend);					break;

		case XCPGETID:					XcpGetIdReply(dataToSend);								break;

		case XCPMTA:					XcpSetMta(dataReceived,dataToSend);						break;

		case XCPUPLOAD:					XcpUploadReply(dataReceived,dataToSend);				break;

		case XCPSHORTUPLOAD:			XcpShortUploadReply(dataReceived,dataToSend);			break;

		case XCPUSERCMD:				XcpUserCommand(dataReceived, dataToSend);				break;

		case XCPDOWNLOAD:				XcpDownload(dataReceived,dataToSend);					break;


		/* XCP DAQ list commands */
		case XCPSETDAQPOINTER:			XcpSetDaqPointer(dataReceived, dataToSend);				break;

		case XCPWRITEDAQ:				XcpWriteDaq(dataReceived, dataToSend);					break;

		case XCPDAQLISTMODE:			XcpSetDaqListMode(dataReceived,dataToSend);				break;

		case XCPSTARTSTOPDAQLIST:		XcpStartStopDaqList(dataReceived,dataToSend);			break;

		case XCPSTARTSTOPSYNCH:			XcpStartStopSynch(dataReceived, dataToSend);			break;



		case XCPGETDAQPROCESSORINFO:	XcpDaqProcessorInfoReply(dataToSend);					break;

		case XCPGETDAQRESOLUTIONINFO:	XcpDaqResolutionInfoReply(dataToSend);					break;

		case XCPGETDAQLISTINFO:																	break;

		case XCPGETDAQEVENTINFO:		XcpDaqEventInfoReply(dataReceived, dataToSend);			break;



		case XCPFREEDAQ:				XcpFreeDaq(dataToSend);									break;

		case XCPALLOCDAQ:				XcpAllocDaq(dataReceived, dataToSend);					break;

		case XCPALLOCODT:				XcpAllocOdt(dataReceived, dataToSend);					break;

		case XCPALLOCODTENTRY:			XcpAllocOdtEntry(dataReceived, dataToSend);				break;
	}
}

/***************************************************************************************
** \brief     Function to send a positive reply to tthe master. The replay is loaded into
**            the data array dataToSend
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpPositiveResponse(uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP Positive response\n");
	#endif

	dataToSend[0] 				= 1; 							// length of packet to be send back to the master
	dataToSend[1] 				= 0xff; 						// packet response ok
	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     Function to send a negative reply to tthe master. The replay is loaded into
**            the data array dataToSend
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \param	  errorCode The error code to put in the response
** \return    none.
****************************************************************************************/
static void XcpNegativeResponse(uint8_t *dataToSend, uint8_t errorCode)
{
	#if DEBUGINFORMATION == 1
	printf("XCP negative response\n");
	#endif

	dataToSend[0] 				= 2; 							// length of packet to be send back to the master
	dataToSend[1] 				= 0xfe; 						// packet response NOT ok
	dataToSend[2] 				= errorCode;
	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     Function to send a connection reply to the master. The reply is loaded into
**            the data array dataToSend
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpConnectReply(uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP connect reply\n");
	#endif

	xcpCommunication.status = 0x00;

	dataToSend[0]				= 8; 							// length of packet to be send back to the master
	dataToSend[1] 				= 0xff; 						// packet response ok
	dataToSend[2] 				= 0x04; 						// Resource CAL/PAG, DAQ PGM
	dataToSend[3] 				= 0x80; 						// Common mode basic (byte order)
	dataToSend[4] 				= xcpCommunication.ctoLength; 	// maximum CTO (Command Transfer Objects) size in bytes
	*(uint16_t*)&dataToSend[5] 	= xcpCommunication.dtoLength;	// maximum DTO (Data Transfer Objects) size in bytes
	dataToSend[7] 				= XCPPROTOCOLLAYERVERSION; 		// XCP protocol layer version number MSB only
	dataToSend[8] 				= XCPTRANSLORTLAYERVERSION;  	// XCP transport layer version Number MSB only

	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     Function to send a reply on a disconnect command to the master. The reply is
**            loaded into the data array dataToSend
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpDisconnectReply(uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP disconnected \n");
	#endif

	XcpPositiveResponse(dataToSend);
}

/***************************************************************************************
** \brief     Function to send the status of the XCP stack back to the master. The reply
**            is loaded into the data array dataToSend
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpStatusReply(uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP status reply\n");
	#endif

	dataToSend[0] 				= 6; 							// Length of packet to be send back to the master
	dataToSend[1] 				= 0xff; 						// Packet response ok
	dataToSend[2] 				= xcpCommunication.status;		// Current session status HAN
	dataToSend[3] 				= 0x00; 						// Resource protection status
	dataToSend[4] 				= 0x20; 						// Ignored byte
	*(uint16_t*)&dataToSend[5] 	= 0x0000; 						// Session configuration ID

	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     Function to reply on a synchronisation service request from the master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpSynchronisation(uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP synchronize communication\n");
	#endif

	XcpNegativeResponse(dataToSend,XCPERRORCMDCYNC); 							//For this function, a negative response is required
}

/***************************************************************************************
** \brief     Function to send the communication mode back to the master.
**            The reply is loaded into the data array dataToSend
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void xcpCommunicationModeReply(uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP communication mode reply\n");
	#endif

	dataToSend[0] 				= 8; 							// Length of packet to be send back to the master
	dataToSend[1] 				= 0xff; 						// Packet response ok
	dataToSend[2] 				= 0x20; 						// Ignored byte
	dataToSend[3] 				= 0x00; 						// Communication mode
	dataToSend[4] 				= 0x20; 						// Ignored byte
	dataToSend[5] 				= 0x00; 						// Max BS
	dataToSend[6] 				= 0x00; 						// Min ST
	dataToSend[7] 				= 0x00;							// Ignored Byte
	dataToSend[8] 				= 0x14;							// XCP driver version
	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     Function to send the length of de ID string back to the master. The reply is
**            loaded into the data array dataToSend
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpGetIdReply(uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP Get ID reply\n");
	#endif

	xcpCommunication.idRequest = (uint16_t)uniqueIdLength;		//Store Length of Identifier



	dataToSend[0] = 8; 											// Length of packet to be send back to the master
	dataToSend[1] = 0xff; 										// Packet response ok
	dataToSend[2] = 0x00; 										// Mode
	dataToSend[3] = 0x00; 										// Ignored byte
	dataToSend[4] = 0x20; 										// Ignored byte
	*(uint32_t*)&dataToSend[5] = xcpCommunication.idRequest;	// Length of ID string
	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     First function to update a paramater in RAM. This function stores the memory
**            adres and adres extension for the next time memory write
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpSetMta(uint8_t *dataReceived, uint8_t *dataToSend)
{
	xcpWrite.adressExtension 	= dataReceived[3];
	xcpWrite.adress				= *(uint32_t*)&dataReceived[4];

	XcpPositiveResponse(dataToSend);
}

/***************************************************************************************
** \brief     Function to upload data back to the master. This is ONLY configuration data
**            so this function is NOT called frequently during XCP data upload. The data to
**			  be send depends on the requested info (idRequest or daqEventRequest)
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpUploadReply(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP upload reply\n");
	#endif

		if(xcpCommunication.idRequest !=0) 						// upload in case of identifier info request
		{
		static uint8_t stringPointer = 0;
		uint8_t pointer = 0;


		dataToSend[0] = dataReceived[1]+1; 						// length of packet to be send back to the master
		dataToSend[1] = 0xff; 									// packet response ok

			while(pointer < dataReceived[1])
			{
			dataToSend[pointer+2]= uniqueIdString[stringPointer];
			pointer ++;
			stringPointer++;
			xcpCommunication.idRequest --;
			}

			if(xcpCommunication.idRequest == 0)					// reset stringPointer for next time ID is requested.
			{
			stringPointer = 0;
			}
		}

		else if(xcpCommunication.daqEventRequest !=0) 				// upload in case of DAQ event info request
		{
		uint8_t pointer = 0;

		dataToSend[0] = dataReceived[1]+1; 						// length of packet to be send back to the master
		dataToSend[1] = 0xff; 									// packet response ok

			while(pointer < dataReceived[1])
			{
			/* extract one from daqEventRequest desciption can be found in function XcpDaqEventInfoReply*/
			dataToSend[pointer+2]= eventChannel[(xcpCommunication.daqEventRequest -1)].channel[pointer];
			pointer ++;
			}

			xcpCommunication.daqEventRequest = 0;
		}

		else /* normal long upload */
		{
		dataToSend[0] = dataReceived[1]+1; 						// length of packet to be send back to the master
		dataToSend[1] = 0xff; 									// packet response ok

			for(uint8_t dataPointer = 0; dataPointer < dataReceived[1]; dataPointer++)
			{
			XcpReadData(&dataToSend[2+dataPointer],1,(uint32_t*)(uintptr_t)xcpWrite.adress++);
			}
		}

	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     Function to upload short data back to the master. This function will only upload
**            1, 2 or 4 bytes stores in memory on the given location
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpShortUploadReply(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP daq event info reply\n");
	#endif



	uint32_t *location;
	location = (uint32_t*)(uintptr_t)*(uint32_t*)&dataReceived[4];



	dataToSend[0] 				= 1 + dataReceived[1]; 			// Length of packet to be send back to the master
	dataToSend[1] 				= 0xff; 						// Packet response ok

	for(uint8_t dataPointer = 0; dataPointer < dataReceived[1]; dataPointer++)
	{
		XcpReadData(&dataToSend[2+dataPointer],1, (uint32_t*)(((uint8_t*)location) + dataPointer));
	}

	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/************************************************************************************//**
** \brief     This command is user defined. It mustn't be used to implement 
**			  functionalities done by other services.
** \param	  Pointer to de data array with received data from XCP master
** \param	  Pointer to the data array that is send back to the master with a reply or data
** \return    none.
**
****************************************************************************************/
static void XcpUserCommand(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP user command\n");
	#endif
	// CALL USERCOMMAND, DEFINED IN XcpTargetSpecific.c
	if(XcpUserCmd(dataReceived)==0){
		XcpPositiveResponse(dataToSend); //only positive response is required
	} else {
		XcpNegativeResponse(dataToSend,XCPERRCMDUNKNOWN);
	}
}

/***************************************************************************************
** \brief     Function to write data on a specific memory location. The function XcpWriteData
**            can be found in XcpTargetSpecific.c. The target specific way to write to flash
**			  can be programmed there.
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpDownload(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP \n");
	#endif

	XcpWriteData(&dataReceived[2],dataReceived[1], xcpWrite.adress);
	XcpPositiveResponse(dataToSend);
}

/***************************************************************************************
** \brief     Function to set the pointer to the right daqlist, ODT and ODT entries.
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpSetDaqPointer(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP set daq, ODT and ODT entry pointers\n");
	#endif

	//TODO check if incomming data is not exceeding the maximum allowed DAQ lists. ODT's and entry's
	xcpCommunication.daqList 	= *(uint16_t*)&dataReceived[2];
	xcpCommunication.odt		= dataReceived[4];
	xcpCommunication.odtEntry	= dataReceived[5];

	XcpPositiveResponse(dataToSend);
}

/***************************************************************************************
** \brief     Function to fill the right information of one specific ODT entrie. The
**			  pointers are set by function XcpSetDaqPointer.
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpWriteDaq(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP writing the ODT entries\n");
	#endif


	#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
	daq[xcpCommunication.daqList].odt[xcpCommunication.odt].entry[xcpCommunication.odtEntry].bitOffset  		= dataReceived[1];
	daq[xcpCommunication.daqList].odt[xcpCommunication.odt].entry[xcpCommunication.odtEntry].sizeOfDaqElement 	= dataReceived[2];
	daq[xcpCommunication.daqList].odt[xcpCommunication.odt].entry[xcpCommunication.odtEntry].adressExtension	= dataReceived[3];

	daq[xcpCommunication.daqList].odt[xcpCommunication.odt].entry[xcpCommunication.odtEntry].adressOfDaqElement = *(uint32_t*)&dataReceived[4];
	#else
	_daq* daq = (_daq*)&xcpMemoryStack[0] + xcpCommunication.daqList; // set pointer to the apropriate daq
	_odt* odt = (_odt*)daq->odt1Location + xcpCommunication.odt;
	_entry* entry =  (_entry*)odt->entry1Location + xcpCommunication.odtEntry;

	entry->bitOffset 			= dataReceived[1];
	entry->sizeOfDaqElement 	= dataReceived[2];
	entry->adressExtension		= dataReceived[3];

	entry->adressOfDaqElement 	=  *(uint32_t*)&dataReceived[4];
	#endif

	XcpPositiveResponse(dataToSend);
}

/***************************************************************************************
** \brief     Function to configure the daq list mode.
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpSetDaqListMode(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP set daq list mode\n");
	#endif

	xcpCommunication.daqMode 			= dataReceived[1];
	xcpCommunication.daqList 			= *(uint16_t*)&dataReceived[2];
	xcpCommunication.eventChannel 		= *(uint16_t*)&dataReceived[4];
	xcpCommunication.transRatePres 		= dataReceived[6];
	xcpCommunication.daqListPriority 	= dataReceived[7];

	#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
	daq[*(uint16_t*)&dataReceived[2]].prescalerSet = dataReceived[6];
	#else
	_daq* daq = (_daq*)&xcpMemoryStack[0] + *(uint16_t*)&dataReceived[2];
	daq->prescalerSet = dataReceived[6];
	#endif
	XcpPositiveResponse(dataToSend);
}

/***************************************************************************************
** \brief     Function to activate the XCP data transfer for measurement variables.
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpStartStopDaqList(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("xcpCommunication.status is: %d\n",xcpCommunication.status);
	#endif

	dataToSend[0] 				= 2; 								// length of packet to be send back to the master
	dataToSend[1] 				= 0xff; 							// packet response ok

	#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
	dataToSend[2] 				= daq[*(uint16_t*)&dataReceived[2]].odtId; 	// First PID TODO PID dynamically programmed
	#else
	dataToSend[2] = XcpGetDaq(*(uint16_t*)&dataReceived[2])->odtId;
	#endif

	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);

	#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
	daq[*(uint16_t*)&dataReceived[2]].listStatus = dataReceived[1]; //Update each daqlist with the received status
	#else
	_daq* daq = (_daq*)&xcpMemoryStack[0] + *(uint16_t*)&dataReceived[2];
	daq->listStatus = dataReceived[1];
	#endif

		if( *(uint16_t*)&dataReceived[2] == xcpCommunication.nrOfDaqLists - 1)
		{
		xcpCommunication.status    |= (dataReceived[1]<<6)&0x40;	// update the XCP status flag
	//	xcpCommunication.daqList 	= *(uint16_t*)&dataReceived[2];	// update which daq list needs to be active
		}
}

/***************************************************************************************
** \brief     Function to reply to XCP master with daq processor information.
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
//TODO Add MACRO's for XCP daq processor information
static void XcpDaqProcessorInfoReply(uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP daq processor info reply\n");
	#endif

	dataToSend[0] 				= 8; 							// length of packet to be send back to the master
	dataToSend[1] 				= 0xff; 						// packet response ok
	dataToSend[2] 				= 0x53; 						// DAQ properties
	*(uint16_t*)&dataToSend[3] 	= 0x0000; 						// Maximal DAQ 0x0000 = Dynamic
	*(uint16_t*)&dataToSend[5] 	= 0x0003; 						// Max event channel
	dataToSend[7] 				= 0x00;							// Min DAQ
	dataToSend[8] 				= 0x00;							// DAQ key byte

	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     Function to activate the XCP data transfer for measurement variables.
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpDaqEventInfoReply(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP daq event info reply\n");
	#endif

	dataToSend[0] 				= 7; 							// Length of packet to be send back to the master
	dataToSend[1] 				= 0xff; 						// Packet response ok
	dataToSend[2] 				= 0x04; 						// Daq event properties
	dataToSend[3] 				= 0x01; 						// Max daq list
	dataToSend[4]				= 0x07; 						// Event channel name length
	dataToSend[5] 				= 0x64; 						// Event channel time cycle
	dataToSend[6] 				= 0x05;							// Event channel time unit ??
	dataToSend[7] 				= 0x00;							// Event channel priority (0x00 lowest priority)
	/* Add one because in case of daqlist 0 we want to set
		the daqEventRequest to one so the function XcpUploadReply
		knows there is a daqEventRequest*/
	xcpCommunication.daqEventRequest = dataReceived[2]+1;

	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     Function to reply to XCP master with daq resolution information.
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpDaqResolutionInfoReply(uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP daq resolution info reply\n");
	#endif

	dataToSend[0] 				= 8; 							// Length of packet to be send back to the master
	dataToSend[1] 				= 0xff; 						// Packet response ok
	dataToSend[2] 				= 0x01; 						// Daq granularity odt entry size
	dataToSend[3] 				= 0x1f; 						// Daq max odt entry size
	dataToSend[4]				= 0x00; 						// Ignored byte
	dataToSend[5] 				= 0x00; 						// Ignored byte
	dataToSend[6] 				= 0x52;							// Timestamp mode
	*(uint16_t*)&dataToSend[7]  = 0x0001;						// Timestamp ticks

	XcpCalculateChecksum(dataToSend);
	XcpSendData(dataToSend);
}

/***************************************************************************************
** \brief     Function to start or stop de synchonisation of multiple daqlists.
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
// TODO expand daqlist synchronisation functionality
static void XcpStartStopSynch(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP synchronisation command : %d\n",dataReceived[1]);
	#endif

	switch (dataReceived[1])
	{
		case 0x00:
		XcpStopDataTransmission();
		XcpPositiveResponse(dataToSend);
		break;
	}
}

/***************************************************************************************
** \brief     Function to free the memory that is allocated dynamically by the XCP daqlist
**			  configuration mechanism. (Allocation of daq, ODT and ODT entries
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpFreeDaq( uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("XCP free memory command executed.\n");
	#endif

	XcpStopDataTransmission();

	#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
			for(uint8_t daqListCount = 0; daqListCount < xcpCommunication.nrOfDaqLists; daqListCount++) // go through the available daqLists
			{
				for(uint8_t odtCount = 0; odtCount < daq[daqListCount].nrOfOdts; odtCount++)
				{
				#if DYNAMICMEMORYALLOCATIONFREERTOS == 1
				vPortFree (daq[daqListCount].odt[odtCount].entry);//Free dynamically assigned memory from ODT entries
				#else
				free(daq[daqListCount].odt[odtCount].entry);	//Free dynamically assigned memory from ODT entries
				#endif
				}
			#if DYNAMICMEMORYALLOCATIONFREERTOS == 1
			vPortFree (daq[daqListCount].odt);					//Free dynamically assigned memory from ODT's
			#else
			free(daq[daqListCount].odt);						//Free dynamically assigned memory from ODT's
			#endif
			}
		#if DYNAMICMEMORYALLOCATIONFREERTOS == 1
		vPortFree (daq);										//Free dynamically assigned memory from DAQ lists
		#else
		free(daq);												//Free dynamically assigned memory from DAQ lists
		#endif




	#else
	memoryStackPointer = &xcpMemoryStack[0];					// Reset memoryStackPointer to start address from static memory
	#endif

	xcpCommunication.odtIdCounter = 0;							//Reset the ODT ID counter for the next communication session
	xcpCommunication.nrOfDaqLists = 0;							//Clear the number of active daq lists
	xcpMemoryUsage = 0;											//Reset memory usage for recalculation

	XcpPositiveResponse(dataToSend);
}

/***************************************************************************************
** \brief     Function to dynamically assign memory for the number of daqlists
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpAllocDaq(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("Allocate memory for DAQ list's\n");
	#endif

	xcpCommunication.nrOfDaqLists = *(uint16_t*)&dataReceived[2]; 	//Number of Daq lists for this XCP device

	//TODO insert error memory overflow if number of lists exceed the maximum allowed number of lists
	#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
		if(*(uint16_t*)&dataReceived[2] > 0) // check if there is at least one list to allocate
		{
		#if DYNAMICMEMORYALLOCATIONFREERTOS == 1

			if(xPortGetFreeHeapSize() <= sizeof(_daq)) // check if there's enough free heap space available
			{
			XcpNegativeResponse(dataToSend,XCPERRORMEMORYOVERFLOW);
			return;
			}

		daq = pvPortMalloc(*(uint16_t*)&dataReceived[2]* sizeof(_daq));
		#else
		daq = malloc(*(uint16_t*)&dataReceived[2]* sizeof(_daq));
		#endif
		}
	#else
		 memoryStackPointer = &xcpMemoryStack[0];

		for(uint8_t daqPointer = 0; daqPointer < *(uint16_t*)&dataReceived[2]; daqPointer++)
		{
			/* Check if memory Stackpointer is still in the range of the reserved memory */
			if((uint32_t)memoryStackPointer >= (uint32_t)((&xcpMemoryStack[0] + XCPSTATICMEMORY) - sizeof(_daq)))
			{
			XcpNegativeResponse(dataToSend,XCPERRORMEMORYOVERFLOW);
			return;
			}
		memoryStackPointer += sizeof(_daq);
		}

	#endif

	xcpMemoryUsage += (*(uint16_t*)&dataReceived[2] * sizeof(_daq));

	XcpPositiveResponse(dataToSend);
}

/***************************************************************************************
** \brief     Function to dynamically assign memory for the number of ODT's
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpAllocOdt(uint8_t *dataReceived, uint8_t *dataToSend)
{
	#if DEBUGINFORMATION == 1
	printf("Allocate memory for the number of ODT's\n");
	#endif

	#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
	daq[*(uint16_t*)&dataReceived[2]].nrOfOdts = dataReceived[4];
	daq[*(uint16_t*)&dataReceived[2]].prescalerActual = 0;
		#if DYNAMICMEMORYALLOCATIONFREERTOS == 1

			if(xPortGetFreeHeapSize() <= sizeof(_odt)) // check if there's enough free heap space available
			{
			XcpNegativeResponse(dataToSend,XCPERRORMEMORYOVERFLOW);
			return;
			}

		daq[*(uint16_t*)&dataReceived[2]].odt = pvPortMalloc(dataReceived[4]* sizeof(_odt));
		#else
		daq[*(uint16_t*)&dataReceived[2]].odt = malloc(dataReceived[4]* sizeof(_odt)); 				//Dynamically assign memory to number of odt's
		#endif
	daq[*(uint16_t*)&dataReceived[2]].odtId = xcpCommunication.odtIdCounter;	// Assign the ODT ID of the first ODT in the daq list
	#else
	_daq* daq = (_daq*)&xcpMemoryStack[0] + *(uint16_t*)&dataReceived[2];

	daq->nrOfOdts = dataReceived[4];
	daq->odt1Location = (uint32_t)memoryStackPointer; // at this point, the memoryStackPointer points first unassigned location

	for(uint8_t odtPointer = 0; odtPointer < dataReceived[4]; odtPointer++)
	{
		/* Check if memory Stackpointer is still in the range of the reserved memory */
		if((uint32_t)memoryStackPointer >= (uint32_t)((&xcpMemoryStack[0] + XCPSTATICMEMORY) - sizeof(_odt)))
		{
		XcpNegativeResponse(dataToSend,XCPERRORMEMORYOVERFLOW);
		return;
		}
	memoryStackPointer += sizeof(_odt); // increase the stackpointer with the size of an odt struct
	}

	daq->odtId = xcpCommunication.odtIdCounter;	// Assign the ODT ID of the first ODT in the daq list

	#endif

	xcpCommunication.odtIdCounter += dataReceived[4]; // add the number of odt's so the next daqlist knows on which ID to start

	xcpMemoryUsage += (dataReceived[4] * sizeof(_odt));

	XcpPositiveResponse(dataToSend);
}

/***************************************************************************************
** \brief     Function to dynamically assign memory for the number of ODT entries
** \param	  dataReceived Pointer to de data array with received data from XCP master
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpAllocOdtEntry(uint8_t *dataReceived, uint8_t *dataToSend)
{
	//TODO check if incomming data is not exceeding the maximum allowed DAQ lists. ODT's and entry's


	#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
	daq[*(uint16_t*)&dataReceived[2]].odt[dataReceived[4]].nrOfOdtEntries = dataReceived[5];

		#if DYNAMICMEMORYALLOCATIONFREERTOS == 1

			if(xPortGetFreeHeapSize() <= sizeof(_entry)) // check if there's enough free heap space available
			{
			XcpNegativeResponse(dataToSend,XCPERRORMEMORYOVERFLOW);
			return;
			}

		daq[*(uint16_t*)&dataReceived[2]].odt[dataReceived[4]].entry = pvPortMalloc(dataReceived[5]* sizeof(_entry));
		#else
		daq[*(uint16_t*)&dataReceived[2]].odt[dataReceived[4]].entry = malloc(dataReceived[5]* sizeof(_entry));
		#endif
	#else

	_daq* daq = (_daq*)&xcpMemoryStack[0] + *(uint16_t*)&dataReceived[2]; // set pointer to the apropriate daq
	_odt* odt = (_odt*)daq->odt1Location + dataReceived[4];

	odt->nrOfOdtEntries = dataReceived[5];
	odt->entry1Location = (uint32_t)memoryStackPointer; // at this point, the memoryStackPointer points first unassigned location

	for(uint8_t odtEntryPointer = 0; odtEntryPointer < dataReceived[5]; odtEntryPointer++)
	{
		/* Check if memory Stackpointer is still in the range of the reserved memory */
		if((uint32_t)memoryStackPointer >= (uint32_t)((&xcpMemoryStack[0] + XCPSTATICMEMORY) - sizeof(_entry)))
		{
		XcpNegativeResponse(dataToSend,XCPERRORMEMORYOVERFLOW);
		return;
		}
	memoryStackPointer += sizeof(_entry);

	}
	#endif

	xcpMemoryUsage += (dataReceived[5] * sizeof(_entry));

	XcpPositiveResponse(dataToSend);
}

#if DYNAMICMEMORYALLOCATION == 0 && DYNAMICMEMORYALLOCATIONFREERTOS == 0
/***************************************************************************************
** \brief     Function that returns the memory locations of the DAQx structure where x stands
** 			  for the daqlist number.
** \param	  daqList The daqlist from wich the memory location is required.
** \return    Memory location of the daq structure that is required.
**
****************************************************************************************/
static _daq* XcpGetDaq(uint16_t daqList)
{
return (_daq*)&xcpMemoryStack[0] + daqList;
}

/***************************************************************************************
** \brief     Function that returns the memory locations of the ODTx structure from a specific
**			  DAQ list. x stands for the ODT number in this DAQ list
** \param	  daqList The daqlist from wich the memory location of the ODT is required.
** \param	  odtValue The ODT number from wich the memory location is required.
** \return    Memory location of the ODT structure that is required.
****************************************************************************************/
static _odt* XcpGetOdt(uint16_t daqList, uint8_t odtValue)
{
	_daq* daq = (_daq*)&xcpMemoryStack[0] + daqList;
return	(_odt*)daq->odt1Location + odtValue;
}

/***************************************************************************************
** \brief     Function that returns the memory locations of ODTentryx structure from a specific
**			  DAQ list and ODT. x stands for the entry number in the provided ODT from the
			  provided DAQ list.
** \param	  daqList The daqlist from wich the memory location of the ODTentry is required.
** \param	  odtValue The ODT number from wich the ODTentry memory location is required.
** \param	  odtEntry The ODTentry number from wich the memory location is required.
** \return    Memory location of the ODTentry structure that is required.
****************************************************************************************/
static _entry* XcpGetOdtEntry(uint16_t daqList, uint8_t odtValue, uint8_t odtEntry)
{
	_daq* daq = (_daq*)&xcpMemoryStack[0] + daqList;
	_odt* odt = (_odt*)daq->odt1Location + odtValue;
return	(_entry*)odt->entry1Location + odtEntry;
}
#endif


/***************************************************************************************
** \brief     Function that takes care of the actual data tranmission from slave to master.
**            This function needs to be called periodically. For example each 10ms. The
**			  prescaler is used in this function to increase or decrease the frequency of
**			  data upload to the master
** \param	  none.
** \return    none.
****************************************************************************************/
void XcpDataTransmission(void)
{
uint8_t data[50] = {0};

	if(xcpCommunication.status == 0x40)
	{
	xcpCommunication.active = 1;
		for(uint8_t daqListCount = 0; daqListCount < xcpCommunication.nrOfDaqLists; daqListCount++) // go through the available daqLists
		{

		#if DYNAMICMEMORYALLOCATION == 1 || DYNAMICMEMORYALLOCATIONFREERTOS == 1
		daq[daqListCount].prescalerActual ++; //increase the timer counter for one specific daqlist
			if(daq[daqListCount].listStatus == 0x01 && daq[daqListCount].prescalerActual >= daq[daqListCount].prescalerSet)
			{
				data[1] = daq[daqListCount].odtId; // use the dynamic ODT ID to identify ODT to master
				for(uint8_t odtCount = 0; odtCount < daq[daqListCount].nrOfOdts; odtCount++)
				{
					uint8_t dataPointer = 2;
					for(uint8_t odtEntryCount = 0; odtEntryCount < daq[daqListCount].odt[odtCount].nrOfOdtEntries; odtEntryCount++)
					{
						XcpReadData(&data[dataPointer],daq[daqListCount].odt[odtCount].entry[odtEntryCount].sizeOfDaqElement,(uint32_t*)(uintptr_t)daq[daqListCount].odt[odtCount].entry[odtEntryCount].adressOfDaqElement);
						dataPointer += daq[daqListCount].odt[odtCount].entry[odtEntryCount].sizeOfDaqElement;
						if(xcpCommunication.status != 0x40)
						{
						return;
						}
					}
					data[0] = dataPointer-1; // dataPointer starts @ index 2 0 = length of message , 1 = ODT  2 = actual message
					XcpCalculateChecksum(&data[0]);
					if (XcpSendData(&data[0])){
						//data failed to send
						xcpCommunication.status = 0;
						return;
					}
					data[1] ++;
				}
				daq[daqListCount].prescalerActual = 0; // reset the daqListCount for timing
			}

		#else
			_daq *daq = XcpGetDaq(daqListCount);

			daq->prescalerActual ++;

			if(daq->listStatus == 0x01 && daq->prescalerActual >= daq->prescalerSet)
			{
				data[1] = daq->odtId; // use the dynamic ODT ID to identify ODT to master

				for(uint8_t odtCount = 0; odtCount < daq->nrOfOdts; odtCount++)
				{
					_odt *odt = XcpGetOdt(daqListCount, odtCount);

					uint8_t dataPointer = 2;
					for(uint8_t odtEntryCount = 0; odtEntryCount < odt->nrOfOdtEntries; odtEntryCount++)
					{
						_entry *entry = XcpGetOdtEntry(daqListCount, odtCount, odtEntryCount);
						XcpReadData(&data[dataPointer],entry->sizeOfDaqElement,(uint32_t*)entry->adressOfDaqElement);
						dataPointer = dataPointer + entry->sizeOfDaqElement;
					}
					data[0] = dataPointer-1; // dataPointer starts @ index 2 0 = length of message , 1 = ODT  2 = actual message
					XcpCalculateChecksum(&data[0]);
					XcpSendData(&data[0]);
					data[1] ++;
				}
			daq->prescalerActual = 0; // reset the daqListCount for timing
			}
		#endif
		}
	}
	else //datatransmission disabled.
	{
	xcpCommunication.active = 0;
	}
}

/***************************************************************************************
** \brief     Function to stop communication with master and uninitialize the stack.
** \param	  none.
** \return    none.
****************************************************************************************/
static void XcpStopDataTransmission(void)
{
	xcpCommunication.status = 0x00;							//Reset the XCP stack status back to 0
}

/***************************************************************************************
** \brief     Function that calculates and adds the checksum tot the data array that is
** 			  send back to the master.
** \param	  dataToSend Pointer to the data array that is send back to the master with a reply or data
** \return    none.
****************************************************************************************/
static void XcpCalculateChecksum(uint8_t *dataToSend)
{
	if(xcpCommunication.checksum == 0)		// if no checksum is required. exit function directly.
	{
	return;
	}

	dataToSend[dataToSend[0]+1]= 0;

	for(uint8_t dataPointer = 1; dataPointer <= dataToSend[0]; dataPointer ++)
	{
	dataToSend[dataToSend[0]+1] += dataToSend[dataPointer];
	}
}


/***************************************************************************************
** \brief     Function that returns the number of bytes in use for XCP
** \param	  none.
** \return    number of bytes used by XCP
****************************************************************************************/
uint32_t XcpMonitorMemoryUsage(void)
{
return xcpMemoryUsage;
}

/* end of XcpStack.c */
