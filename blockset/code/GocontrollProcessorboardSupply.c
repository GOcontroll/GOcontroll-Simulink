/************************************************************************************//**
* \file         GocontrollProcessorboardSupply.c
* \brief        Functions to read the ADC from the GOcontroll Moduline.
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
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/* I2C include */
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <signal.h>

#include "GocontrollProcessorboardSupply.h"
#include "GocontrollProcessorboard.h"

#include "iio.h"

/****************************************************************************************
* Data declarations to store supply voltages
****************************************************************************************/

_controllerSupply controllerSupply;

extern _hardwareConfig hardwareConfig;

struct iio_device *iioMCP;
struct iio_channel *adcChannels[4];

/****************************************************************************************
* Function prototypes
****************************************************************************************/

int GocontrollProcessorboardSupply_Voltage(uint8_t supply, uint16_t* value)
{
	switch(supply)
	{
		case 1: { *value = controllerSupply.batteryVoltage;	break; }
		case 2: { *value = controllerSupply.k15aVoltage;	break; }
		case 3: { *value = controllerSupply.k15bVoltage;	break; }
		case 4: { *value = controllerSupply.k15cVoltage;	break; }
		default: {return -1;}
	}

	return 0;
}

/****************************************************************************************/

int GocontrollProcessorboardSupply_ReadAdc(uint8_t supply, uint16_t* value) {
	if (hardwareConfig.adcControl==ADC_MCP3004) {
		char buf[100];
		ssize_t res;
		size_t bufSize = sizeof buf;
		switch (supply) {
			//read raw (voltage) attributes
			case 1:
			res = iio_channel_attr_read(adcChannels[2], "raw", buf, bufSize);
			break;
			case 2:
			res = iio_channel_attr_read(adcChannels[3], "raw", buf, bufSize);
			break;
			case 3:
			res = iio_channel_attr_read(adcChannels[0], "raw", buf, bufSize);
			break;
			case 4:
			res = iio_channel_attr_read(adcChannels[1], "raw", buf, bufSize);
			break;
		}
		// printf("Read adc result: %ld\n", res);
		//convert to milivolts and store it
		*value = (uint16_t)((float)(strtof(buf, NULL) * 25.54)); //25.54 = ((3.35/1023)/1.5)*11700
		return 0;
	}

	if(hardwareConfig.adcControl == ADC_ADS1015)
	{
	static uint8_t dataTxConfig[3] 	= {0};
	static uint8_t dataTxConvert[1] = {0};
	static uint8_t dataRx[2] 		= {0};

	static int i2cDevice = 0;
	static float decimalFactor = 4.095 / 2047;

    const int addr 					= 0x48; /* Device address */

	const int convertContact1 		= 0xC3; /* Battery voltage */
	const int convertContact2 		= 0xD3; /* Active 1 voltage */
	const int convertContact3 		= 0xE3; /* Active 2 voltage */
	const int convertBattery 		= 0xF3; /* Active 3 voltage */


	dataTxConfig[0] 	= 0x01; 	/* Select Configuration register */
	dataTxConfig[1] 	= 0xF3; 	/* MSB config register */
	dataTxConfig[2] 	= 0xE3; 	/* LSB config register */

	dataTxConvert[0] 	= 0x00;		/* Select Conversion register */


		/* Configure device depending on values to be read*/
		if(supply == 1)
		{
			dataTxConfig[1] = convertBattery;
		}
		else if(supply == 2)
		{
			dataTxConfig[1] = convertContact1;
		}
		else if(supply == 3)
		{
			dataTxConfig[1] = convertContact2;
		}
		else if(supply == 4)
		{
			dataTxConfig[1] = convertContact3;
		}
		else
		{
			return -1;
		}


		/* Open I2C device */
		if ((i2cDevice = open("/dev/i2c-2",O_RDWR)) < 0) {
			printf("Failed to open the bus for voltage measurement.\n");
			close(i2cDevice);
			return -1;
		}

		/* Aquire bus acces */
		if (ioctl(i2cDevice,I2C_SLAVE,addr) < 0) {
			printf("Failed to acquire bus access and/or talk to slave.\n");
			close(i2cDevice);
			return -1;
		}

		/* Send configuration data */
		if (write(i2cDevice,dataTxConfig,3) != 3) {
			printf("Failed to write to the i2c bus.\n");
			close(i2cDevice);
			return -1;
		}

		/* Set device in mode to read out the conversion value */
		if (write(i2cDevice,dataTxConvert,1)!= 1){
			printf("Failed to write to the i2c bus.\n");
			close(i2cDevice);
			return -1;
		}

		/* Read data from device */
		if (read(i2cDevice,dataRx,2) != 2) {
			printf("Failed to read from the i2c bus.\n");
			close(i2cDevice);
			return -1;
		}

		/* Close I2C bus */
		close(i2cDevice);

		/* Calculate the value that is retrieved from the bus */
		int16_t valueTemp = (dataRx[0]<<4) | ((dataRx[1] & 0xf0)>>4);

		/* Check if signbit is set. If set, Provide 0 als value*/
		if(((dataRx[0] & 0x80)>> 7)==1)
		{
		valueTemp = 0;
		}

		/* Send back the retrieved value */
		*value = (uint16_t)((float)(((valueTemp * decimalFactor)/1.5)*11700));
	}

return 0;
}

/****************************************************************************************/

void *GocontrollProcessorboardSupply_ReadAdcThread(void *arg)
{
	struct ControllerSupplyThreadArgs *args = (struct ControllerSupplyThreadArgs *)arg;
	struct iio_context *iioContext;
	__useconds_t sample_time = (__useconds_t) 1000000.0 * args->sample_time;
	/* set up the mcp3004 iio channels*/
	if(hardwareConfig.adcControl==ADC_MCP3004) {
		iioContext = iio_create_local_context();
		uint8_t channel_count = 0;
		iioMCP = iio_context_find_device(iioContext, "mcp3004");
		//loop through the channels available to this device
		for (uint8_t i = 0; i < iio_device_get_channels_count(iioMCP); ++i) {
			struct iio_channel *chn = iio_device_get_channel(iioMCP, i);
			//filter out the channels that have 2 attributes, these are the voltage channels
			if (iio_channel_get_attrs_count(chn) == 2) {
				adcChannels[channel_count] = chn;
				channel_count++;
			}
		}
	}
	while(args->thread_run) {
		/* Execute actual conversions of the adc */
		GocontrollProcessorboardSupply_ReadAdc(1,&controllerSupply.batteryVoltage);
		GocontrollProcessorboardSupply_ReadAdc(2,&controllerSupply.k15aVoltage);
		GocontrollProcessorboardSupply_ReadAdc(3,&controllerSupply.k15bVoltage);
		GocontrollProcessorboardSupply_ReadAdc(4,&controllerSupply.k15cVoltage);
		usleep(sample_time);
	}
	iio_context_destroy(iioContext);
	return 0;
}