/************************************************************************************//**
* \file         Gocontrolllin.c
* \brief        Utility functions to interface the GOcontroll LIN interface.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2024 (c)  by GOcontroll   http://www.gocontroll.com      All rights reserved
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
* endinternal
*
* This code is heavily inspired by slLIN prototype code developped by:
* Pavel Pisa <pisa@cmp.felk.cvut.cz>
* Rostislav Lisovy <lisovy@kormus.cz>
*
* Source:
* https://github.com/lin-bus/linux-lin/
*
* With their permission this code is released under an MIT style license instead of a GPL license
*
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h> /* clock_nanosleep */
#include <getopt.h>

#include <asm/ioctls.h>
#include <asm/termbits.h>

#include "GocontrollLin.h"

/*Thread references */
#include "pthread.h"


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define LIN_HDR_SIZE		2


/****************************************************************************************
* Local data declarations
****************************************************************************************/

struct sllin_tty sllin_tty_data;

struct sllin sllin_data = {
	.tty = &sllin_tty_data,
};

struct sllin *sl;

int lin_descriptor = 0;

/****************************************************************************************
* Local data definitions
****************************************************************************************/

const unsigned char sllin_id_parity_table[64] = {
 /*0*/  0x80,0xc0,0x40,0x00,0xc0,0x80,0x00,0x40, /*7*/
        0x00,0x40,0xc0,0x80,0x40,0x00,0x80,0xc0, /*15*/
        0x40,0x00,0x80,0xc0,0x00,0x40,0xc0,0x80, /*23*/
        0xc0,0x80,0x00,0x40,0x80,0xc0,0x40,0x00,
        0x00,0x40,0xc0,0x80,0x40,0x00,0x80,0xc0,
        0x80,0xc0,0x40,0x00,0xc0,0x80,0x00,0x40,
        0xc0,0x80,0x00,0x40,0x80,0xc0,0x40,0x00,
        0x40,0x00,0x80,0xc0,0x00,0x40,0xc0,0x80
};

static int tty_set_mode(struct sllin_tty *tty, int baudrate);
static int tty_set_baudrate(struct sllin_tty *tty, int baudrate);

/************************************************************************************//**
** \brief   Function that initialized the LIN Serial interface
** \param   None.
** \return  None.
**
****************************************************************************************/
int GocontrollLin_InitializeInterface(void)
{
	static uint8_t firsttime = 1;


	if(firsttime == 1)
	{
	int fd;

	sl = &sllin_data;

	sl->lin_baud = 19200;

	/* Calculate baudrate for sending LIN break */
	sl->lin_break_baud = (sl->lin_baud * 2) / 3;
	/* Check if this is the first time the interface is opened */

		/* Open serial interface */
		fd = open("/dev/ttymxc3", O_RDWR);
		if (fd < 0) {
			perror("open()");
			return -1;
		}
		/* Prevent interface from reinitializing */


	/* Store LIN file descriptor */
	sl->tty->tty_fd = fd;

	/* Set serial interface to right mode */
	tty_set_mode(sl->tty, sl->lin_baud);

	firsttime = 0;
	}
	/* Exit function in right state */
	return 0;
}


/************************************************************************************//**
** \brief   Function that initialized the LIN Serial interface
** \param   None.
** \return  None.
**
****************************************************************************************/
int GocontrollLin_DeInitializeInterface(void)
{
	ioctl(sl->tty->tty_fd, TCSETS2,sl->tty->tattr_orig);
    close(sl->tty->tty_fd);
	return 0;
}


/************************************************************************************//**
** \brief   Function that schedules the transmit and request messages
** \param   id of the message
** \param   action (1) construct the array of messages (2) request acknowledge for message
** \return  (1) if message is allowed to be send (0) if not
**
****************************************************************************************/
uint8_t GocontrollLin_LinMessageScheduler(uint8_t id, uint8_t action)
{
	static uint8_t storedIds 			= 0;
	static uint8_t storedId[10] 		= {0};
	static uint8_t cnt 					= 0;

	/* Function called if message is allowed to be send */
	if(action == 2)
	{
		/* Do we have the first message in a row? */
		if(id == storedId[0])
		{
		cnt ++;
		}
		/* If we have counted al the available messages */
		if(cnt >= storedIds)
		{
		cnt = 0;
		}
		/* If there is an id match, allow the message to be send */
		if(storedId[cnt] == id)
		{
			return 1;
		}
	}
	/* Initialization call. Store all ID, used in the model */
	else if(action == 1)
	{
	storedId[storedIds++] = id;
	}

	return 0;
}

/************************************************************************************//**
** \brief   Funtion that only sends a LIN header to the bus. Slaves can respond to these
**			messages
** \param   id of the header that is send
** \param 	dataLength
** \return  None.
**
****************************************************************************************/
int GocontrollLin_MasterRetrieveData(uint8_t id, uint8_t dataLength ,uint8_t data[], uint8_t checksum)
{
	uint8_t buff[6];

	buff[0] = 0x00; /* Fake break */
	buff[1] = 0x55; /* Sync byte */

	/* Construct the identifier */
	id &= 0x3f;
	id |= sllin_id_parity_table[id];
	buff[2] = id;

	/* Flush interface */
	ioctl(sl->tty->tty_fd, TCFLSH, TCIOFLUSH);

	/* Decrease speed to send BREAK*/
	tty_set_baudrate(sl->tty, sl->lin_break_baud);

	/* Write "break part of the LIN message" */
	write(sl->tty->tty_fd, &buff[0], 1);
	{
		struct timespec sleep_time;
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = ((1000000000ll * 11) / sl->lin_break_baud);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, NULL);
	}

	/* Restore "normal" speed */

	tty_set_baudrate(sl->tty, sl->lin_baud);

	/* Write sync byte field */
	write(sl->tty->tty_fd, &buff[1], 1);
	/* Write PID -- Protected Identifier Field */
	write(sl->tty->tty_fd, &buff[2], 1);

	/* calculate LIN data bytes based on id */
	uint8_t dataBytes = 0;
	if (dataLength == 3)		{dataBytes = 8;}
	else if (dataLength == 2)	{dataBytes = 4;}
	else if (dataLength == 1)	{dataBytes = 2;}

	fcntl(sl->tty->tty_fd, F_SETFL, fcntl(sl->tty->tty_fd, F_GETFL)|O_NONBLOCK);

	/* TODO Design other method for delay */
	/* Wait 2 ms for slave response */
	usleep(5000);

	/* At this point, a slave should respons with data */
	read(sl->tty->tty_fd, &buff[0], 1); /* Break frame */
	read(sl->tty->tty_fd, &buff[1], 1); /* Sync Byte */
	read(sl->tty->tty_fd, &buff[2], 1); /* Identifier */

	/* Start checksum calculation */
	uint8_t calculatedChecksum = 0;
	/* In case enhanced is chosen, ID is part of checksum */
	if(checksum == 2)
	{
	calculatedChecksum = buff[2];
	}

	for(uint8_t dataPointer = 0; dataPointer < dataBytes; dataPointer++)
	{
	read(sl->tty->tty_fd, &data[dataPointer], 1);
	calculatedChecksum += data[dataPointer];
		if(calculatedChecksum > 255)
		{
			calculatedChecksum -= 255;
		}
	}
	return 0;
}


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
int GocontrollLin_MasterSendData(uint8_t id, uint8_t dataLength, uint8_t data[], uint8_t checksum)
{
	/* calculate LIN data bytes based on id */
	uint8_t dataBytes = 0;
	if (dataLength == 3)		{dataBytes = 8;}
	else if (dataLength == 2)	{dataBytes = 4;}
	else if (dataLength == 1)	{dataBytes = 2;}

	/* Construct LIN message */
	uint8_t buff[6];

	buff[0] = 0x00; /* Fake break */
	buff[1] = 0x55; /* Sync byte */

	id &= 0x3f;
	id |= sllin_id_parity_table[id];
	buff[2] = id; /* LIN ID: 1 */

	ioctl(sl->tty->tty_fd, TCFLSH, TCIOFLUSH);

	/* Decrease speed to send BREAK */
	tty_set_baudrate(sl->tty, sl->lin_break_baud);

	write(sl->tty->tty_fd, &buff[0], 1); /* Write "break" */
	{
		struct timespec sleep_time;
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = ((1000000000ll * 11) / sl->lin_break_baud);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, NULL);
	}

	/* Restore "normal" speed*/
	tty_set_baudrate(sl->tty, sl->lin_baud);

	/* Send Sync Byte Field */
	write(sl->tty->tty_fd, &buff[1], 1);
	/* Send PID -- Protected Identifier Field */
	write(sl->tty->tty_fd, &buff[2], 1);

	/* Start checksum calculation */
	uint16_t calculatedChecksum = 0;
	/* In case enhanced is chosen, ID is part of checksum */
	if(checksum == 2)
	{
	calculatedChecksum = buff[2];
	}

	/* Send the number of data bytes depending on ID */
	for(uint8_t dataPointer = 0; dataPointer < dataBytes; dataPointer++)
	{
	write(sl->tty->tty_fd, &data[dataPointer], 1);
	calculatedChecksum += data[dataPointer];
		if(calculatedChecksum > 255)
		{
			calculatedChecksum -= 255;
		}
	}

	/* 2nd Complement checksum */
	calculatedChecksum = (uint8_t)~calculatedChecksum;

	/* Send checksum */
	write(sl->tty->tty_fd, &calculatedChecksum, 1);
return 0;
}

/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
static int tty_set_baudrate(struct sllin_tty *tty, int baudrate)
{
	tty->tattr.c_ospeed = baudrate;
	tty->tattr.c_ispeed = baudrate;
	tty->tattr.c_cflag &= ~CBAUD;
	tty->tattr.c_cflag |= BOTHER;

	if(ioctl(tty->tty_fd, TCSETS2, &tty->tattr)) {
		perror("ioctl TIOCSSERIAL");
		return -1;
	}

	return 0;
}


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
static int tty_set_mode(struct sllin_tty *tty, int baudrate)
{
	if(!isatty(tty->tty_fd)) {
		fprintf(stderr, "Not a terminal.\n");
		return -1;
	}

	/* Flush input and output queues. */
	if (ioctl(sl->tty->tty_fd, TCFLSH, TCIOFLUSH) != 0) {
		perror("tcflush");
		return -1;;
	}

	//ioctl(sl->tty->tty_fd, TCFLSH, TCIOFLUSH);

	/* Save settings for later restoring */
	if (ioctl(tty->tty_fd, TCGETS2, &tty->tattr_orig) < 0) {
		perror("ioctl TCGETS2");
		return -1;
	}

	/* Save settings into global variables for later use */
	if (ioctl(tty->tty_fd, TCGETS2, &tty->tattr) < 0) {
		perror("ioctl TCGETS2");
		return -1;
	}


	/* 8 data bits                  */
	/* Enable receiver              */
	/* Ignore CD (local connection) */
	tty->tattr.c_cflag = CS8 | CREAD | CLOCAL;
	tty->tattr.c_iflag = 0;
	tty->tattr.c_oflag = NL0 | CR0 | TAB0 | BS0 | VT0 | FF0;
	tty->tattr.c_lflag = 0;

	tty->tattr.c_cc[VINTR]    = '\0';
	tty->tattr.c_cc[VQUIT]    = '\0';
	tty->tattr.c_cc[VERASE]   = '\0';
	tty->tattr.c_cc[VKILL]    = '\0';
	tty->tattr.c_cc[VEOF]     = '\0';
	tty->tattr.c_cc[VTIME]    = '\0';
	tty->tattr.c_cc[VMIN]     = 1;
	tty->tattr.c_cc[VSWTC]    = '\0';
	tty->tattr.c_cc[VSTART]   = '\0';
	tty->tattr.c_cc[VSTOP]    = '\0';
	tty->tattr.c_cc[VSUSP]    = '\0';
	tty->tattr.c_cc[VEOL]     = '\0';
	tty->tattr.c_cc[VREPRINT] = '\0';
	tty->tattr.c_cc[VDISCARD] = '\0';
	tty->tattr.c_cc[VWERASE]  = '\0';
	tty->tattr.c_cc[VLNEXT]   = '\0';
	tty->tattr.c_cc[VEOL2]    = '\0';


	/* Set new parameters with previous speed and left */
	/* tty_set_baudrate() to do the rest  */
	if(ioctl(tty->tty_fd, TCSETS2, &tty->tattr)) {
		perror("ioctl TIOCSSERIAL");
		return -1;
	}

	/* Set real speed */
	tty_set_baudrate(tty, baudrate);

	return 0;
}
