/************************************************************************************//**
* \file         GocontrollProcessorboardSupply.h
* \brief        Functions to read the ADC from the GOcontroll Moduline.
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
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#ifndef _GOCONTROLLPROCESSORBOARDSUPPLY_H
#define _GOCONTROLLPROCESSORBOARDSUPPLY_H

#include <stdint.h>

// extern struct iio_context *iioContext;

// extern struct iio_device *iioMCP;
// extern struct iio_channel *adcChannels[4];

/***************************************************************************************
** \brief     Initialize the structs for the ADC, automatically gets called by the ReadAdcThread. \
** \brief     !!! Only manually call this function if this thread is not used. !!!
** \return    None
****************************************************************************************/
// void GocontrollProcessorboardSupply_InitAdc(void);

/***************************************************************************************
** \brief     Utility function to serve ADC data to function blocks, reads from the ADC Thread.
** \param     supply supply source (1=K30 2=K15-A 3=K15-B 4=K15-C)
** \param	  value pointer to variable to store the calculated voltage in.
** \return    0 upon succes, -1 when an incorrect supply is supplied.
****************************************************************************************/
int GocontrollProcessorboardSupply_Voltage(uint8_t supply, uint16_t *value);

/***************************************************************************************
** \brief     Dedicated thread to read ADC values from ADC convertor. \
** \brief     This thread needs to be running to use GocontrollProcessorboardSupply_Voltage()
** \param     arg Thread ID
** \return    None
**
****************************************************************************************/
void *GocontrollProcessorboardSupply_ReadAdcThread(void *arg);

/***************************************************************************************
** \brief     Function that actually reads the ADC value from the onboard ADC convertor and convert
**			  it to the right voltage, gets called by the ADC thread, or on demand. \
** \brief     Don't manually call this function when the thread is active, just use the Voltage() function then.
** \param     supply supply source (1=K30 2=K15-A 3=K15-B 4=K15-C)
** \param	  value Pointer to varaiable to store the calculated voltage in.
** \return    0 if ok -1 if  failed
****************************************************************************************/
int GocontrollProcessorboardSupply_ReadAdc(uint8_t supply, uint16_t* value);

#endif /*_GOCONTROLLPROCESSORBOARD_H*/