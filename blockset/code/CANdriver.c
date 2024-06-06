/************************************************************************************//**
* \file			CANdriver.c
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


/****************************************************************************************
* Include files
****************************************************************************************/
#include "CANdriver.h"

#define NOTNEEDED			0
#define INITIALIZEFAILED	1
#define INITIALIZED			2
#define ERRORSTATE			3

_canConnection canConnection;

/** Global variables **/
int can_socket; /* File descriptor for CAN socket */
struct can_filter CANfilter[4][CANBUFSIZE]; /* CANfilter to filter receive ID's */
struct CANbuffer_t CANbuffer[4][CANBUFSIZE]; /* CANbuffer to hold received messages */
const int test = sizeof(struct CANbuffer_t);


/****************************************************************************************/
void CANsocket(uint8_t canInterface){
	struct sockaddr_can addr;
	struct ifreq ifr;
	//printf("CAN initialize %d\n",canInterface);
	/* Check parameter plausibility*/
	if(canInterface > 3 || canInterface < 0){return;}

	fprintf(stderr, "About to enter CANsocket()\n");
	/*First check if the CAN socket has already been created */
	if (canConnection.socketCreated[canInterface] == NOTNEEDED || canConnection.socketCreated[canInterface] == INITIALIZEFAILED){
		fprintf(stderr, "Entered CANsocket()\n");
		/* Create the CAN socket */
		canConnection.socket[canInterface]=socket(PF_CAN, SOCK_RAW, CAN_RAW);
		/* Check if the socket was created successfully */
		if(canConnection.socket[canInterface]==-1){
			perror("CANsocket");
		}else{
			if(canInterface == 0){strcpy(ifr.ifr_name, "can0" );}
			if(canInterface == 1){strcpy(ifr.ifr_name, "can1" );}
			if(canInterface == 2){strcpy(ifr.ifr_name, "can2" );}
			if(canInterface == 3){strcpy(ifr.ifr_name, "can3" );}

			if(ioctl(canConnection.socket[canInterface], SIOCGIFINDEX, &ifr)<0)
			{
			canConnection.socketCreated[canInterface] = INITIALIZEFAILED;
			return;
			}

			addr.can_family = AF_CAN;
			addr.can_ifindex = ifr.ifr_ifindex;

			if(bind(canConnection.socket[canInterface], (struct sockaddr *)&addr, sizeof(addr)) < 0){
			//	perror("Error in socket bind");
			//	printf("Error %d\n",canInterface);

				canConnection.socketCreated[canInterface] = INITIALIZEFAILED;
				return;
			}else{
			//	printf("SUCCES %d\n",canInterface);
				canConnection.socketCreated[canInterface] = INITIALIZED;
			}
		}
	}else{
		/* CANsocket was already created, nothing to be done here */
	}
}

/****************************************************************************************/
int serveCANconnection(void){
	struct can_frame frame;
	uint16_t i;
	for(uint8_t canInterface = 0; canInterface < 4; canInterface++)
	{
		/* Check if CANsocket exists */
		if (canConnection.socketCreated[canInterface] == INITIALIZED){
			//printf("CAN %d\n",canInterface);
			while(recv(canConnection.socket[canInterface], &frame, sizeof(struct can_frame), MSG_DONTWAIT) > 0){ /* Add error check? */
			//printf("new message\n");
				/* Check if identifier is in HANcoder buffer, it should be there because the */
				/* receive filter should only allow messages which are defined in Simulink */
				for(i=0;i<CANBUFSIZE;i++){
					/* Check CAN_id */
					if(frame.can_id == CANbuffer[canInterface][i].can_id){
						/* Copy all the frame fields */
						memcpy(&CANbuffer[canInterface][i].frame, &frame, sizeof(struct can_frame));
						/* Set the newFlag to true */
						CANbuffer[canInterface][i].newFlag = true;
						break; /* Buffer location found move to next message */
					}
				}
			}
		}
		else if(canConnection.socketCreated[canInterface] == INITIALIZEFAILED)
		{
			if((canConnection.initErrorCounter[canInterface]++)>15)
			{
				canConnection.socketCreated[canInterface]  = ERRORSTATE;
				printf("Interface: CAN%d not found!\n",canInterface);
			}

		CANsocket(canInterface);
		}
	}
	return 0; /* return 0 when no error has occurred */
}

/****************************************************************************************/
int getCANbuffer(canid_t can_id,uint8_t canInterface, struct can_frame **frame, bool *newFlag){

	/* Check parameter plausibility*/
	if(canInterface > 3 || canInterface < 0){return -1;}

	uint16_t i;
	/* Check where in the buffer the same identifier is used */
	for(i=0;i<CANBUFSIZE;i++){
		if(CANbuffer[canInterface][i].can_id == can_id){
			*frame = &CANbuffer[canInterface][i].frame; /* Copy data to the frame buffer */
			*newFlag = CANbuffer[canInterface][i].newFlag; /* Update value pointed at by newFlag */
			/* Update the new flag in the buffer */
			CANbuffer[canInterface][i].newFlag = false;
			return 0;
		}
	}
	return -1; /* Should not reach this part of the code (only if the ID is not in buffer */
}

/****************************************************************************************/
int updateCANreceiveFilter(canid_t oldCANid,uint8_t canInterface, canid_t newCANid){

	/* Check parameter plausibility*/
	if(canInterface > 3 || canInterface < 0){return -1;}

	uint16_t i;
	int returnValue;
	/* Find old CANid in the filter */
	for(i=0;i<=CANBUFSIZE;i++){
		if(CANfilter[canInterface][i].can_id == oldCANid){
			/* Add canid to filter */
			CANfilter[canInterface][i].can_id = newCANid;
			CANfilter[canInterface][i].can_mask = CAN_EFF_MASK; /* Mask is always set to 29bit ID's */
			/* A filter matches, when: <received_can_id> & mask == can_id & mask */
			break;
		}
	}
	if(i >= CANBUFSIZE){
		returnValue = -1; /* Set error when the oldCANid wasn't found */
	}else{
		/* Set new filter to socket */
		returnValue = setsockopt(can_socket, SOL_CAN_RAW, CAN_RAW_FILTER, &CANfilter[canInterface], sizeof(CANfilter[canInterface]));
	}
	return returnValue;
}

/****************************************************************************************/
int updateCANreceiveBuffer(canid_t oldCANid,uint8_t canInterface, canid_t newCANid){

	/* Check parameter plausibility*/
	if(canInterface > 3 || canInterface < 0){return-1;}

	uint16_t i;
	int returnValue;
	/* Find old CANid in the filter */
	for(i=0;i<=CANBUFSIZE;i++){
		fprintf(stdout,"CANid of buffer %x and CAN id of block 0x%x\n",CANbuffer[canInterface][i].can_id,oldCANid);
		if(CANbuffer[canInterface][i].can_id == oldCANid){
			/* Add canid to buffer */
			CANbuffer[canInterface][i].can_id = newCANid;
			fprintf(stdout,"Just added the newCANid to the buffer: 0x%x\n",CANbuffer[canInterface][i].can_id);
			break;
		}
	}
	if(i >= CANBUFSIZE){
		returnValue = -1; /* Set error when the oldCANid wasn't found */
	}else{
		returnValue = 0;
	}
	return returnValue;
}