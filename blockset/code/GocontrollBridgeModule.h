/************************************************************************************//**
* \file         GOcontrollBridgeModule.h
* \brief        Utility functions to interface the GOcontroll bridge module.
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
* \endinternal
****************************************************************************************/
#ifndef GOCONTROLL_BRIDGE_MODULE_H_
#define GOCONTROLL_BRIDGE_MODULE_H_

#define BRIDGEFUNC_DISABLED             1       //channel is unused
#define BRIDGEFUNC_HALFBRIDGE           2       //half bridge duty cycle controlled
#define BRIDGEFUNC_LOWSIDEDUTY          3       //low side switching duty cycle controlled
#define BRIDGEFUNC_HIGHSIDEDUTY         4       //high side switching duty cycle controlled
#define BRIDGEFUNC_LOWSIDEBOOL          5       //low side switching on or off (0-1)
#define BRIDGEFUNC_HIGHSIDEBOOL         6       //high side switching on or off (0-1)

#define BRIDGEFREQ_100HZ                1       //duty cycle frequency
#define BRIDGEFREQ_200HZ                2       //duty cycle frequency
#define BRIDGEFREQ_500HZ                3       //duty cycle frequency
#define BRIDGEFREQ_1KHZ                 4       //duty cycle frequency
#define BRIDGEFREQ_6CH_2KHZ             5       //duty cycle frequency
#define BRIDGEFREQ_6CH_5KHZ             6       //duty cycle frequency
#define BRIDGEFREQ_6CH_10KHZ            7       //duty cycle frequency

#define BRIDGECHANNEL1                  0
#define BRIDGECHANNEL2                  1

/****************************************************************************************
* Include files
****************************************************************************************/
#include "GocontrollProcessorboard.h"
#include "stdio.h"

/****************************************************************************************
* Data declarations
****************************************************************************************/
typedef struct
{
uint8_t configuration[2];
uint16_t maxCurrent[2];
uint16_t value[2];
uint32_t syncCounter[2];
int16_t current[2];
int16_t temperature;
uint16_t ground;
uint32_t moduleIdentifier;
uint8_t moduleSlot;
}_bridgeModule;

/****************************************************************************************
* Function prototypes
****************************************************************************************/

/***************************************************************************************
** \brief	Function that sends the configuration data to the bridge module
** \param	bridgeModule pointer to a _bridgeModule structure that holds the data for the module configuration
** \return	None
****************************************************************************************/
void BridgeModule_Configuration(_bridgeModule *bridgeModule);

/***************************************************************************************
** \brief	Function that sends the output data to the bridge module
** \param	bridgeModule pointer to a _bridgeModule structure that holds the data for the module configuration
** \return	None
****************************************************************************************/
void BridgeModule_SendValues(_bridgeModule *bridgeModule);

/***************************************************************************************
** \brief	Function that sets the module slot for a bridge module
** \param	bridgeModule pointer to a _bridgeModule structure that holds the data for the module configuration
** \param	moduleSlot the slot that the module is inserted in
** \return	None
****************************************************************************************/
void BridgeModule_SetModuleSlot(_bridgeModule *bridgeModule, uint8_t moduleSlot);

/***************************************************************************************
** \brief	Function that configures an output channel for a bridge module
** \param	bridgeModule pointer to a _bridgeModule structure that holds the data for the module configuration
** \param	channel the channel to be configured
** \param	func the function of the bridge channel
** \param 	freq the frequency at which the bridge channel sends out its pwm frequency
** \return	None
****************************************************************************************/
void BridgeModule_ConfigureChannel(_bridgeModule *bridgeModule, uint8_t channel, uint8_t func, uint8_t freq);

#endif  /* GOCONTROLL_BRIDGE_MODULE_H_ */
