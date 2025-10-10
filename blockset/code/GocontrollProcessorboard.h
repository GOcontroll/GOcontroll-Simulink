/**************************************************************************************
 * \file         GOcontrollProcessorboard.h
 * \brief        Functions that handels the low level control of the GOcontroll
 *hardware.
 * \internal
 *----------------------------------------------------------------------------------------
 *                          C O P Y R I G H T
 *----------------------------------------------------------------------------------------
 *  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com   All rights
 *reserved
 *
 *----------------------------------------------------------------------------------------
 *                            L I C E N S E
 *----------------------------------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 * \endinternal
 ****************************************************************************************/

#ifndef _GOCONTROLL_PROCESSORBOARD_H
#define _GOCONTROLL_PROCESSORBOARD_H

/****************************************************************************************
 * Include files
 ****************************************************************************************/
#include <stdint.h> /* ANSI C types                  */

/****************************************************************************************
 * Macro definitions
 ****************************************************************************************/
#define BOOTMESSAGELENGTH 46
#define BOOTMESSAGELENGTHCHECK 61

#define INPUTMODULE6CHMESSAGELENGTH 55
#define INPUTMODULE10CHMESSAGELENGTH 50

#define INPUTMODULE420MAMESSAGELENGTH 33

#define OUTPUTMODULE6CHMESSAGELENGTH 44
#define OUTPUTMODULE10CHMESSAGELENGTH 49

#define BRIDGEMODULEMESSAGELENGTH 44

#define IRMODULEMESSAGELENGTH 31
#define RTCMODULEMESSAGELENGTH 44

#define MESSAGEOVERLENGTH 1

/****************************************************************************************
 * Macro definitions (Hardware configuration)
 ****************************************************************************************/
#define NOT_INSTALLED 0
#define ADC_MCP3004 1
#define ADC_ADS1015 2
#define ADC_INTEGRATED 3

#define LED_GPIO 1
#define LED_RUKR 2

#define MODULESLOT1 0
#define MODULESLOT2 1
#define MODULESLOT3 2
#define MODULESLOT4 3
#define MODULESLOT5 4
#define MODULESLOT6 5
#define MODULESLOT7 6
#define MODULESLOT8 7

#define SLOTOCCUPIED 1
#define SLOTFREE 0

/****************************************************************************************
 * Data declarations (LED control)
 ****************************************************************************************/
typedef enum {
	LED_COLOR_RED = 0x01,
	LED_COLOR_GREEN = 0x02,
	LED_COLOR_BLUE = 0x03
} _ledColor;

/****************************************************************************************
 * Data declarations (Hardware configuration)
 ****************************************************************************************/
typedef struct {
	uint8_t ledControl;
	uint8_t adcControl;
	uint8_t moduleNumber;
	uint8_t moduleOccupancy[8][7];
} _hardwareConfig;

typedef struct {
	unsigned long in_len;
	uint8_t in[4096];
	unsigned long out_len;
	uint8_t *out;
	uint8_t pad;
} _do_block;

/****************************************************************************************
 * Function prototypes
 ****************************************************************************************/

/* external interface to be implemented by board specific files */

/****************************************************************************************
** \brief     Control the GOcontroll enclosure LEDs
** \param     led which led should be controlled (1-4)
** \param     color _ledColor enum
** \param     value brightness of the colour on the specified led
** \return    0 if ok -1 if  failed
****************************************************************************************/
int GocontrollProcessorboard_LedControl(uint8_t led, _ledColor color,
										uint8_t value);

/**************************************************************************************
** \brief     Function that controls the controller supply active state. This
*function is *			  needed to switch of the controller when
*required by the system
** \param     state The state the supply needs to be in. 1=on 0=off
** \return    0 if ok -1 if  failed
****************************************************************************************/
int GocontrollProcessorboard_ControllerActive(uint8_t state);

/***************************************************************************************
** \brief	function that gets the cpu temperature
** \param	none
** \return	cpu termperature
****************************************************************************************/
float GocontrollProcessorboard_ControllerTemperature(void);

/****************************************************************************************
** \brief     Send data over spi to a module
** \param     command the spi command
** \param     dataLength the length of the data to be sent in bytes
** \param     id1 data byte 1
** \param     id2 data byte 2
** \param     id3 data byte 3
** \param     id4 data byte 4
** \param     module the module number to which it must be sent
** \param     dataTx buffer for the bytes to transmit
** \param     delay delay in microseconds before sending the message
** \return    0 if ok -1 if  failed
****************************************************************************************/
int GocontrollProcessorboard_SendSpi(uint8_t command, uint8_t dataLength,
									 uint8_t id1, uint8_t id2, uint8_t id3,
									 uint8_t id4, uint8_t module,
									 uint8_t *dataTx, uint32_t delay);

/****************************************************************************************
** \brief     Send spi message to a module and receive one back
** \param     command the spi command
** \param     dataLength the length of the data to be sent in bytes
** \param     id1 data byte 1
** \param     id2 data byte 2
** \param     id3 data byte 3
** \param     id4 data byte 4
** \param     module the module number to which it must be sent
** \param     dataTx buffer for the bytes to transmit
** \param     dataRx buffer for the bytes to receive
** \return    0 if ok -1 if  failed
****************************************************************************************/
int GocontrollProcessorboard_SendReceiveSpi(uint8_t command, uint8_t dataLength,
											uint8_t id1, uint8_t id2,
											uint8_t id3, uint8_t id4,
											uint8_t module, uint8_t *dataTx,
											uint8_t *dataRx);

/**************************************************************************************
** \brief     Function that verifies the application license key \
** \brief     check OpenAES: https://github.com/jhjin/OpenAES
** \param     key 16/24/32 byte encryption key
** \param 	  _iv_ent initialisation vector for the encryption
** \param 	  _file_in pointer to a string describing the path to the
*encrypted license file
** \param 	  _file_check pointer to a string describing te path to the file
*containing the decrypted license
** \param     keyLen the length of the key
** \return    none or exits if it fails
****************************************************************************************/
void GocontrollProcessorboard_VerifyLicense(uint8_t *key, char _iv_ent[16],
											char *_file_in, char *_file_check,
											unsigned long keyLen);

/**************************************************************************************
** \brief     Function that sets the backlight brightness
** \param     brightness brightness to set the screen to
** \param 	  call_type 0 == init, 1 == control, 2 == terminate
** \return    0 on success, error code on failure
****************************************************************************************/
int GocontrollProcessorboard_SetScreenBrightness(uint8_t brightness,
												 uint8_t call_type);

/**************************************************************************************
** \brief     Function that gets the hardware details from memory
** \param     none
** \return    none
****************************************************************************************/
void GocontrollProcessorboard_GetHardwareVersion(void);

/**************************************************************************************
** \brief     Initialize the leds on the enclosure
** \param     none.
** \return    0 if ok -1 if  failed
****************************************************************************************/
int GocontrollProcessorboard_LedInitialize(void);

/****************************************************************************************
** \brief     set the state of the reset pin of a module
** \param     module int holding the position of the module (0-7)
** \param     state the state of the reset pin macros HIGH 1 or LOW 0
** \return    0 if ok -1 if  failed
****************************************************************************************/
int8_t GocontrollProcessorboard_ResetStateModule(uint8_t module, uint8_t state);

/**************************************************************************************
** \brief     Function that sleeps for x miliseconds
** \param     times number of miliseconds to sleep
** \return     none
****************************************************************************************/
void GocontrollProcessorboard_Delay1ms(uint32_t times);

/****************************************************************************************
** \brief     Get the modules out of their bootloader state
** \param     module the module slot (0-7)
** \param     dataTx buffer for the transmit bytes
** \param     dataRx buffer for the receive bytes
** \return    0 if ok -1 if  failed
****************************************************************************************/
int GocontrollProcessorboard_EscapeFromBootloader(uint8_t module,
												  uint8_t *dataTx,
												  uint8_t *dataRx);

/* shared interface */

/**************************************************************************************
** \brief     calculate the checksum of an spi message
** \param     array buffer filled with the spi message
** \param     length length of the spi message
** \return    the checksum
****************************************************************************************/
uint8_t GocontrollProcessorboard_CheckSumCalculator(uint8_t *array,
													uint8_t length);

/**************************************************************************************
** \brief     initialize a module in a specific slot
** \param     moduleslot (0 based index)
** \return    0 on success errno on failure
****************************************************************************************/
int GocontrollProcessorboard_ModuleInitialize(uint8_t moduleslot);

/**************************************************************************************
** \brief     register a module in a specific slot
** \param     slot module slot (0-7)
** \param     rx the bootloader rx buffer
** \return    none
****************************************************************************************/
void GocontrollProcessorboard_RegisterModule(uint8_t slot, uint8_t *rx);

#endif	// _GOCONTROLL_PROCESSORBOARD_H

/* end of GocontrollProcessorboard.h */