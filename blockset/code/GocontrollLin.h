/************************************************************************************//**
* \file         Gocontrolllin.h
* \brief        Utility functions to interface the GOcontroll LIN interface.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2023 (c)  by GOcontroll   http://www.gocontroll.com      All rights reserved
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
#ifndef _GOCONTROLLLIN_H
#define _GOCONTROLLLIN_H

#include <stdint.h>


/* maximum buffer len to store whole LIN message*/
#define SLLIN_DATA_MAX	 8
#define SLLIN_BUFF_LEN	(1 /*break*/ + 1 /*sync*/ + 1 /*ID*/ + \
                         SLLIN_DATA_MAX + 1 /*checksum*/)

struct sllin_tty {
	int tty_fd;
	struct termios2 tattr_orig;
	struct termios2 tattr;
};

struct sllin {
	/* Various fields. */
	struct sllin_tty	*tty;		/* ptr to TTY structure	     */

	/* LIN message buffer and actual processed data counts */
	unsigned char		rx_buff[SLLIN_BUFF_LEN]; 	/* LIN Rx buffer */
	unsigned char		tx_buff[SLLIN_BUFF_LEN]; 	/* LIN Tx buffer */
	int					rx_expect;      			/* expected number of Rx chars */
	int					rx_lim;         			/* maximum Rx chars for ID  */
	int					rx_cnt;         			/* message buffer Rx fill level  */
	int					tx_lim;         			/* actual limit of bytes to Tx */
	int					tx_cnt;         			/* number of already Tx bytes */
	char				lin_master;					/* node is a master node */
	int					lin_baud;					/* LIN baudrate */
	int					lin_break_baud;				/* Baudrate used for break send */
	int 				lin_state;					/* state */
	int 				id_to_send;					/* there is ID to be sent */

	unsigned long		flags;						/* Flag values/ mode etc     */
};

int GocontrollLin_InitializeInterface(void);
int GocontrollLin_DeInitializeInterface(void);
uint8_t GocontrollLin_LinMessageScheduler(uint8_t id, uint8_t action);
int GocontrollLin_MasterRetrieveData(uint8_t id, uint8_t dataLength, uint8_t data[], uint8_t checksum);
int GocontrollLin_MasterSendData(uint8_t id, uint8_t dataLength, uint8_t data[], uint8_t checksum);


#endif /*_GOCONTROLLLIN_H*/