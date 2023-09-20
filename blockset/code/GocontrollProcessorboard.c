/************************************************************************************//**
* \file         GOcontrollProcessorboard.c
* \brief        Functions that handels the low level control of the GOcontroll hardware.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "oaes_lib.h"
#include "oaes_base64.h"
#include "oaes_common.h"

#include "GocontrollProcessorboard.h"
#include "XcpTargetSpecific.h"

/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define LOW				0
#define HIGH			1

#define ARM32			0
#define ARM64			1

#ifndef __min
  #define __min(a,b)  (((a) < (b)) ? (a) : (b))
#endif // __min

/* General includes */
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/* Used for CPU affinity*/
//#define _GNU_SOURCE /*Added using CC flag. See makefile */
#include <sched.h>


/* SPI includes */
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

/* I2C include */
#include <linux/i2c-dev.h>
#include <signal.h>

/* GOcontroll specific includes for Linux target */
#include <MemoryEmulation.h>
#include <MemoryDiagnostic.h>

#include "iio.h"


_hardwareConfig hardwareConfig;
struct iio_context *iioContext;

/****************************************************************************************
* Data declarations (SPI mapping)
****************************************************************************************/
typedef struct{
	char *channel;
}_moduleSpi;

_moduleSpi moduleSpi[8] =
{
	{"/dev/spidev1.0"},
	{"/dev/spidev1.1"},
	{"/dev/spidev2.0"},
	{"/dev/spidev2.1"},
	{"/dev/spidev2.2"},
	{"/dev/spidev2.3"},
	{"/dev/spidev0.0"},
	{"/dev/spidev0.1"},
};

/****************************************************************************************
* Data declarations (SPI settings)
****************************************************************************************/
static uint8_t 	mode = 0;
static uint8_t 	bits = 8;
static uint32_t speed = 2000000;

/****************************************************************************************
* Function prototypes
****************************************************************************************/

/****************************************************************************************
** \brief     Get the modules out of their bootloader state
** \param     module the module slot (0-7)
** \param 	  dataTx buffer for the transmit bytes
** \param 	  dataRx buffer for the receive bytes
** \return    0 if ok -1 if  failed
****************************************************************************************/
static uint8_t 	GocontrollProcessorboard_EscapeFromBootloader(uint8_t module,uint8_t* dataTx, uint8_t* dataRx);

/****************************************************************************************
** \brief     Send a dummy message to all modules over spi
** \param     none.
** \return    none
****************************************************************************************/
static void 	GocontrollProcessorboard_DummySpiSend(void);

/**************************************************************************************
** \brief     Configures and provides the SPI busses
** \param     moduleSlot the module slot for which it needs to provide an spi device
** \return    an spi number
****************************************************************************************/
static int 		GocontrollProcessorboard_SpiDevice(uint8_t moduleSlot);

/**************************************************************************************
** \brief     opens a sysfs file for the reset pin of a module
** \param     moduleSlot module number that needs to be accessed (0-7)
** \return    file descriptor if ok -1 if  failed
****************************************************************************************/
static int 		GocontrollProcessorboard_ModuleReset(uint8_t moduleSlot);

/**************************************************************************************
** \brief     Initialize the leds on the enclosure
** \param     none.
** \return    0 if ok -1 if  failed
****************************************************************************************/
static int 		GocontrollProcessorboard_LedInitialize(void);

/****************************************************************************************
** \brief     Claim cpu number 3 for this process
** \param     none.
** \return    none
****************************************************************************************/
static void 	GocontrollProcessorboard_SetCpuAffinity(void);

/**************************************************************************************
** \brief     Function that sleeps for x miliseconds
** \param     times number of miliseconds to sleep
** \return 	  none
****************************************************************************************/
static void 	GocontrollProcessorboard_Delay1ms(uint32_t times);

/****************************************************************************************
** \brief     set the state of the reset pin of a module
** \param     module int holding the position of the module (0-7)
** \param 	  state the state of the reset pin macros HIGH 1 or LOW 0
** \return    0 if ok -1 if  failed
****************************************************************************************/
static int8_t 	GocontrollProcessorboard_ResetStateModule(uint8_t module, uint8_t state);

/**************************************************************************************
** \brief     calculate the checksum of an spi message
** \param     array buffer filled with the spi message
** \param 	  length length of the spi message
** \return    the checksum
****************************************************************************************/
static uint8_t 	GocontrollProcessorboard_CheckSumCalculator(uint8_t *array,uint8_t length);

/**************************************************************************************
** \brief     Function that gets the hardware details from memory
** \param     none
** \return    none
****************************************************************************************/
static void 	GocontrollProcessorboard_GetHardwareVersion(void);

/**************************************************************************************
** \brief     Function that creates a local iio context with all iio devices on the controller
** \param     none
** \return    none
****************************************************************************************/
void 			GocontrollProcessorboard_GetIIOContext(void);

/****************************************************************************************/

void GocontrollProcessorboard_Initialize(void)
{
	/* Retrieve hardware version */
	GocontrollProcessorboard_GetHardwareVersion();

	GocontrollProcessorboard_GetIIOContext();

	/*Start with modules in reset state so we can do other stuff during reset*/
	for(uint8_t m = 0; m <hardwareConfig.moduleNumber; m++)
	{
	 GocontrollProcessorboard_ResetStateModule(m,1);
	}

	fprintf(stderr,"Processorboard initialize\n");

	GocontrollProcessorboard_SetCpuAffinity();
	/*Send a dummy byte to initialize the SPI. Only neccessary for Linux*/
	GocontrollProcessorboard_DummySpiSend();
	/*Initialize the memory dataholder for persistant data */
	MemoryEmulation_InitializeMemory();
	/*Initialize the memory dataholder for diagnostic data */
	MemoryDiagnostic_InitializeMemory();
	/* Initialize LED driver */
		if( hardwareConfig.ledControl == LED_RUKR)
		{
		GocontrollProcessorboard_LedInitialize();
		}
	/* Short timeout before getting the modules out of reset state */
	GocontrollProcessorboard_Delay1ms(5);
	/*Get the modules out of reset and give some time to startup */
	for(uint8_t m = 0; m <hardwareConfig.moduleNumber; m++)
	{
	 GocontrollProcessorboard_ResetStateModule(m,0);
	}
	/* Give modules short time to start bootloader */
	GocontrollProcessorboard_Delay1ms(10);
	/*Send command to exit the bootloader and start module application */
	/*From within Simulink, the bootloader is not used for new firmware update */
	uint8_t moduleState[8] = {0};
	uint8_t dataTx[BOOTMESSAGELENGTHCHECK] = {0};
	uint8_t dataRx[BOOTMESSAGELENGTHCHECK] = {0};

	for(uint8_t module = 1; module <=hardwareConfig.moduleNumber; module++)
	{
	dataRx[6] = 0;
	GocontrollProcessorboard_EscapeFromBootloader(module,dataTx,dataRx);
	/* Check if the received data is coming from the bootloader */
	/* Retrieve first part of module hardware code */
		if( dataRx[0] == 9) /* bootloaders first byte == 9 */
		{
		/* If valid module data is received, dataRx[6] should be 20 */
		moduleState[module-1] = dataRx[6];
		}
	}
	/* Give some time to start application program on module */
	GocontrollProcessorboard_Delay1ms(5);
	/* Check if module application is started */
	for(uint8_t module = 0; module <hardwareConfig.moduleNumber; module++)
	{
		/* At this point, a module is installed so proceed with the check */
		/* Reset dataRx[6] to 0 to prevent old data corrupting this mechanism */
		dataRx[6] = 0;
		GocontrollProcessorboard_EscapeFromBootloader(module,dataTx,dataRx);
		/* Retrieve first part of module hardware code. If it is provided, it means module application is running*/
		if(dataRx[6] == 20)
		{
			continue;
		}
		/* At this point, it seems the module is stuck. Check if during the first escape from bootloader
		a valid module hardware code was received. If so, extra reset and reinitialisation */
		else if (moduleState[module-1] == 20)
		{
			/* Set module in reset state */
			GocontrollProcessorboard_ResetStateModule(module,1);
			GocontrollProcessorboard_Delay1ms(5);
			GocontrollProcessorboard_ResetStateModule(module,0);
			/* Give modules short time to start bootloader */
			GocontrollProcessorboard_Delay1ms(5);
			dataRx[6] = 0;
			/* Escape from bootloader */
			GocontrollProcessorboard_EscapeFromBootloader(module,dataTx,dataRx);
			/* Give some time to start application program on module */
			GocontrollProcessorboard_Delay1ms(5);
			/* Check again if modules application is now running by re-execution of for loop*/
			module--;
		}
	}

	/* Give bootloader some time to start application program */
	GocontrollProcessorboard_Delay1ms(5);
}

/****************************************************************************************/

static void GocontrollProcessorboard_SetCpuAffinity(void)
{
cpu_set_t  mask;
CPU_ZERO(&mask);
CPU_SET(3, &mask);
sched_setaffinity(0, sizeof(mask), &mask);
}

/****************************************************************************************/

static int8_t GocontrollProcessorboard_ResetStateModule(uint8_t module, uint8_t state)
{
	static const char s_values_str[] = "01";

	if (1 != write(GocontrollProcessorboard_ModuleReset(module), &s_values_str[LOW == state ? 0 : 1], 1)) {
		fprintf(stderr, "Reset pin fail for module %d\n",module);
		return(-1);
	}
	return(0);
}

/****************************************************************************************/

static void GocontrollProcessorboard_DummySpiSend(void)
{
	uint8_t dummyT[6] = {1,2,3,4,5,6};

	for(uint8_t m = 0; m <=7; m++)
	{

	write(GocontrollProcessorboard_SpiDevice(m), &dummyT[0], 5);

	}
}

/****************************************************************************************/

static int GocontrollProcessorboard_LedInitialize(void)
{
	const uint8_t addr = 0x14;
	uint8_t dataTx[2];

	static int i2cDevice = 0;

	/* Open I2C device */
	if ((i2cDevice = open("/dev/i2c-2",O_RDWR)) < 0) {
	close(i2cDevice);
	printf("Error I2C open for LED's.\n");
	return -1;
	}

	/* Aquire bus acces */
	if (ioctl(i2cDevice,I2C_SLAVE,addr) < 0) {
	close(i2cDevice);
    printf("Error I2C require bus for LED's.\n");
	return -1;
	}

	/* First, reset the device */
	dataTx[0] = 0x17;
	dataTx[1] = 0xFF;

	/* Send actual data */
	if (write(i2cDevice,dataTx,2) != 2) {
	close(i2cDevice);
	printf("Error I2C write to bus for LED's.\n");
	return -1;
	}

	/* Second, enable the device (Chip_EN)*/
	dataTx[0] = 0x00;
	dataTx[1] = 0x40;

	/* Send actual data */
	if (write(i2cDevice,dataTx,2) != 2) {
	close(i2cDevice);
	printf("Error I2C write to bus for LED's.\n");
	return -1;
	}

	/* Close I2C connection */
	close(i2cDevice);
return 0;
}

/****************************************************************************************/

int8_t GocontrollProcessorboard_LedControl(uint8_t led, _ledColor color, uint8_t value)
{
	if(hardwareConfig.ledControl == LED_RUKR)
	{
	static uint8_t dataTx[3] 	= {0};
	const int addr 				= 0x14;

	static int i2cDevice = 0;

		/* Select proper LED with color address */
		if(led == 1)
		{
		dataTx[0] = 0x0A + color;
		}
		else if(led == 2)
		{
		dataTx[0] = 0x0D + color;
		}
		else if(led == 3)
		{
		dataTx[0] = 0x10 + color;
		}
		else if(led == 4)
		{
		dataTx[0] = 0x13 + color;
		}
		else
		{
		return -1;
		}

		/* Load data that needs to be send */
		dataTx[1] = value;

		/* Open I2C device */
		if ((i2cDevice = open("/dev/i2c-2",O_RDWR)) < 0) {
		close(i2cDevice);
		printf("Error I2C open for LED's.\n");
		return -1;
		}

		/* Aquire bus acces */
		if (ioctl(i2cDevice,I2C_SLAVE,addr) < 0) {
		close(i2cDevice);
		printf("Error I2C require bus for LED's.\n");
		return -1;
		}

		/* Send actual data */
		if (write(i2cDevice,dataTx,2) != 2) {
		close(i2cDevice);
		printf("Error I2C write to bus for LED's.\n");
		return -1;
		}

	/* Close I2C connection */
	close(i2cDevice);
	return 0;
	}
	else if(hardwareConfig.ledControl == LED_GPIO)
	{
	int ledControl;
	char path[40];

		/* Create path to the LED that needs to be controlled */
		switch(color){
			case LED_COLOR_RED:
			snprintf(path, 40, "/sys/class/leds/Status%d-r/brightness", led);
			break;
			case LED_COLOR_GREEN:
			snprintf(path, 40, "/sys/class/leds/Status%d-g/brightness", led);
			break;
			case LED_COLOR_BLUE:
			snprintf(path, 40, "/sys/class/leds/Status%d-b/brightness", led);
			break;
		}

		/* Construct the command for LED value control */
		static const char s_values_str[] = "01";

		/* Open the path to the LED that needs to be controlled */
		ledControl = open(path, O_WRONLY);

		/* Check if path is opened properly */
		if (ledControl == -1) {
			fprintf(stderr, "Error GPIO write led %d!\n",led);
			close(ledControl);
			return(-1);
		}

		/* Write the actual value to the LED LL file system */
		if (1 != write(ledControl, &s_values_str[LOW == value ? 0 : 1], 1)) {
			fprintf(stderr, "Error GPIO write led %d!\n", led);
			close(ledControl);
			return(-1);
		}

	/* Close the ledcontrol instance */
	close(ledControl);
	}
	else if(hardwareConfig.ledControl == NOT_INSTALLED)
	{
		return 0;
	}
	return(0);
}

/****************************************************************************************/

static uint8_t GocontrollProcessorboard_EscapeFromBootloader(uint8_t module,uint8_t* dataTx, uint8_t* dataRx)
{
	dataTx[0] = 19;
	dataTx[1] = BOOTMESSAGELENGTH-1;
	*(uint16_t*) &dataTx[2] = 19;

	dataTx[BOOTMESSAGELENGTH-1] = GocontrollProcessorboard_CheckSumCalculator(&dataTx[0],BOOTMESSAGELENGTH-1);

	struct spi_ioc_transfer tr = {
		.tx_buf = (long int)&dataTx[0],
		.rx_buf = (long int)&dataRx[0],
		.len = BOOTMESSAGELENGTHCHECK,
		.delay_usecs = 10,
		.speed_hz = 0,
		.bits_per_word = 0,
	};

	ioctl(GocontrollProcessorboard_SpiDevice(module), SPI_IOC_MESSAGE(1), &tr);

	if( GocontrollProcessorboard_CheckSumCalculator(&dataRx[0],BOOTMESSAGELENGTH-1)==dataRx[BOOTMESSAGELENGTH-1])
	{
	return 0;
	}

return -1;
}

/****************************************************************************************/

uint8_t GocontrollProcessorboard_SendSpi(uint8_t command, uint8_t dataLength,uint8_t id1,uint8_t id2,uint8_t id3,
	uint8_t id4, uint8_t module, uint8_t* dataTx, uint32_t delay)
{
	dataTx[0] = command;
	dataTx[1] = dataLength-1;
	dataTx[2] = id1;
	dataTx[3] = id2;
	dataTx[4] = id3;
	dataTx[5] = id4;

	dataTx[dataLength-1] = GocontrollProcessorboard_CheckSumCalculator(&dataTx[0],dataLength-1);

	usleep((uint32_t)delay);
	write(GocontrollProcessorboard_SpiDevice(module), &dataTx[0], dataLength+MESSAGEOVERLENGTH);

return 1;
}

/****************************************************************************************/

uint8_t GocontrollProcessorboard_SendReceiveSpi(uint8_t command, uint8_t dataLength, uint8_t id1,uint8_t id2,uint8_t id3,
	uint8_t id4, uint8_t module, uint8_t* dataTx, uint8_t* dataRx)
{
	dataTx[0] = command;
	dataTx[1] = dataLength-1;
	dataTx[2] = id1;
	dataTx[3] = id2;
	dataTx[4] = id3;
	dataTx[5] = id4;

	dataTx[dataLength-1] = GocontrollProcessorboard_CheckSumCalculator(&dataTx[0],dataLength-1);

	/* Reset some essential values to erase earlier messages */
	dataRx[0] = 0;
	dataRx[0] = 0;
	dataRx[dataLength-1] = 0;

	struct spi_ioc_transfer tr = {
		.tx_buf = (long int)&dataTx[0],
		.rx_buf = (long int)&dataRx[0],
		.len = dataLength + MESSAGEOVERLENGTH,
		.delay_usecs = 10,
		.speed_hz = 0,
		.bits_per_word = 0,
	};

	ioctl(GocontrollProcessorboard_SpiDevice(module), SPI_IOC_MESSAGE(1), &tr);

	if( GocontrollProcessorboard_CheckSumCalculator(&dataRx[0],dataLength-1)==dataRx[dataLength-1])
	{
	return 1;
	}

return 0;
}

/****************************************************************************************/

static int GocontrollProcessorboard_SpiDevice(uint8_t moduleSlot)
{
static int spiDevice[8] = {0};

	if(spiDevice[moduleSlot] == 0)
	{
	spiDevice[moduleSlot] = open(moduleSpi[moduleSlot].channel, O_RDWR);

	ioctl(spiDevice[moduleSlot], SPI_IOC_WR_MODE, &mode);
	ioctl(spiDevice[moduleSlot], SPI_IOC_WR_BITS_PER_WORD, &bits);
	ioctl(spiDevice[moduleSlot], SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	}

return spiDevice[moduleSlot];
}

/****************************************************************************************/

static int GocontrollProcessorboard_ModuleReset(uint8_t moduleSlot)
{

static int moduleReset[8] = {0};

	if(moduleReset[moduleSlot] == 0)
	{
		char path[40];

		snprintf(path, 40, "/sys/class/leds/ResetM-%d/brightness", moduleSlot+1);

		moduleReset[moduleSlot] = open(path, O_WRONLY);

		if (-1 == moduleReset[moduleSlot]) {
		fprintf(stderr, "Error GPOI write module reset!\n");
		return(-1);
		}
	}

return moduleReset[moduleSlot];
}

/****************************************************************************************/

int GocontrollProcessorboard_ControllerActive(uint8_t state)
{
static int controllerActive = 0;

	if(controllerActive == 0)
	{
		char path[45];
		snprintf(path, 41, "/sys/class/leds/power-active/brightness");
		controllerActive = open(path, O_WRONLY);

		if (-1 == controllerActive) {
		fprintf(stderr, "Error GPIO write controller active!\n");
		return(-1);
		}
	}

	static const char s_values_str[] = "01";

	if (1 != write(controllerActive, &s_values_str[LOW == state ? 0 : 1], 1)) {
		fprintf(stderr, "Error GPIO write controller active!\n");
		return(-1);
	}
	return(0);
}

/****************************************************************************************/

float GocontrollProcessorboard_ControllerTemperature(void)
{
	int fileId = 0;

	if((fileId = open("/sys/devices/virtual/thermal/thermal_zone0/temp", O_RDONLY | O_NONBLOCK)) <= 0)
	{
	/* Here means the file is not opend properly */
	/* To be sure it is closed, force a close */
	close(fileId);
	return 0;
	}

	/* If we are here, we have acces to a valid file */
	char tempValue[15] = {0};
	/* Read the content of the file */
	read(fileId, &tempValue[0], 15);
	/* Close the file descriptor */
	close(fileId);
	/* Write the content as a float and return*/
	return (strtof(tempValue, NULL))/1000;
}

/****************************************************************************************/

static uint8_t GocontrollProcessorboard_CheckSumCalculator(uint8_t *array,uint8_t length)
{
	uint8_t checkSum = 0;
	for (uint8_t pointer = 0; pointer<length; pointer++)
	{
	checkSum += array[pointer];
	}
return checkSum;
}

/****************************************************************************************/

static void GocontrollProcessorboard_Delay1ms(uint32_t times)
{
usleep(times *1000);
}

/****************************************************************************************/

static void GocontrollProcessorboard_ProgramStop(int x, siginfo_t * y, void* z)
{
	printf("shutting down\n");
	GocontrollProcessorboard_ExitProgram(NULL);
}

/****************************************************************************************/

void GocontrollProcessorboard_ExitProgram(void* Terminate)
{
static void (*TerminateFunction)(void);

if(Terminate != NULL)
{
static struct sigaction _sigact;

memset(&_sigact, 0, sizeof(_sigact));
_sigact.sa_sigaction = GocontrollProcessorboard_ProgramStop;
_sigact.sa_flags = SA_SIGINFO;

/* Add CTRL-C action to list which triggers a gentle shutdown */
sigaction(SIGINT, &_sigact, NULL);
/* Add Deamon terminate to list which triggers a gentle shutdown */
sigaction(SIGTERM, &_sigact, NULL);

TerminateFunction = Terminate;
return;
}
	XcpStopConnection();
	/* Execute the termination of the program */
	TerminateFunction();
	exit(0);
}

/****************************************************************************************/

static void GocontrollProcessorboard_GetHardwareVersion(void)
{
	int fileId = 0;
	/* Open the file that contains the hardware version of the controller */
	if((fileId = open("/sys/firmware/devicetree/base/hardware", O_RDONLY | O_NONBLOCK)) <= 0)
	{
	/* Here means the file is not opend properly */
	/* To be sure it is closed, force a close */
	close(fileId);
	/* Set default values */
	hardwareConfig.moduleNumber = 8;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_MCP3004;
	printf("Failed to detected hardware! Set default.");
	return;
	}

	/* If we are here, we have acces to a valid file */
	char tempValue[30] = {0};
	/* Read the content of the file */
	read(fileId, &tempValue[0], 30);
	/* Close the file descriptor */
	close(fileId);

	printf("Detected hardware: ");

	/*	Compare strings to see which hardware it is
		Check production hardware first since those are the most likely */
	if(strcmp (tempValue, "Moduline IV V3.06")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 8;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	else if(strcmp (tempValue, "Moduline Mini V1.11")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 4;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	/* Less common hardware versions */
	/* Minis*/
	else if(strcmp (tempValue, "Moduline Mini V1.03")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 4;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_ADS1015;
	}

	else if(strcmp (tempValue, "Moduline Mini V1.05")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 4;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	else if(strcmp (tempValue, "Moduline Mini V1.06")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 4;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	else if(strcmp (tempValue, "Moduline Mini V1.07")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 4;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	else if(strcmp (tempValue, "Moduline Mini V1.10")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 4;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	/* IVs*/
	else if(strcmp (tempValue, "Moduline IV V3.00")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 8;
	hardwareConfig.ledControl = LED_GPIO;
	hardwareConfig.adcControl = ADC_ADS1015;
	}

	else if(strcmp (tempValue, "Moduline IV V3.01")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 8;
	hardwareConfig.ledControl = LED_GPIO;
	hardwareConfig.adcControl = ADC_ADS1015;
	}

	else if(strcmp (tempValue, "Moduline IV V3.02")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 8;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_ADS1015;
	}

	else if(strcmp (tempValue, "Moduline IV V3.03")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 8;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_ADS1015;
	}

	else if(strcmp (tempValue, "Moduline IV V3.04")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 8;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_ADS1015;
	}

	else if(strcmp (tempValue, "Moduline IV V3.05")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 8;
	hardwareConfig.ledControl = LED_RUKR;
	hardwareConfig.adcControl = ADC_ADS1015;
	}

	/* Displays */
	else if(strcmp (tempValue, "Moduline Screen V1.01")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 2;
	hardwareConfig.ledControl = NOT_INSTALLED;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	else if(strcmp (tempValue, "Moduline Screen V1.02")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 2;
	hardwareConfig.ledControl = NOT_INSTALLED;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	else if(strcmp (tempValue, "Moduline Screen V1.03")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 2;
	hardwareConfig.ledControl = NOT_INSTALLED;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	else if(strcmp (tempValue, "Moduline Screen V1.04")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 2;
	hardwareConfig.ledControl = NOT_INSTALLED;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

	else if(strcmp (tempValue, "Moduline Screen V1.05")==0)
	{
	printf(tempValue);
	hardwareConfig.moduleNumber = 2;
	hardwareConfig.ledControl = NOT_INSTALLED;
	hardwareConfig.adcControl = ADC_MCP3004;
	}

printf("\n");
}

/****************************************************************************************/

void GocontrollProcessorboard_VerifyLicense(uint8_t *key, char _iv_ent[16], char *_file_in, char *_file_check, unsigned long keyLen) {

	_do_block _b;
    size_t _read_len = 4096;
	FILE *_f_in = NULL;
	FILE *_f_check = NULL;
	uint8_t _iv[OAES_BLOCK_SIZE] = "";
	size_t _buf_len = 0;
	uint8_t *_buf = NULL;
	OAES_RET _rc = OAES_RET_SUCCESS;

	//check if the entered key has a valid length
	if ((keyLen != 16UL) && (keyLen != 24UL) && (keyLen != 32UL)) {
		fprintf(stderr, "A key of incorrect length was entered: %lu bytes\nKey length can only be 16, 24 or 32 bytes\n", keyLen);
		exit(-1);
	}

	//initialize the OAES struct and allocate it some memory
	OAES_CTX *ctx = NULL;
	ctx =  oaes_alloc();
	if(NULL == ctx) {
		fprintf(stderr, "failed to initialize OAES, exiting.\n");
		exit(-1);
	}

	//get the value that the decoded license should match
	//get the file
	_f_check = fopen(_file_check, "r");
	if( NULL == _f_check ) {
		fprintf(stderr, "Error: Failed to open license check file.\n");
		oaes_free(&ctx);
		exit(-1);
	}
	//get the first line
	char *_check_buf=NULL;
	size_t len = 0;
	getline(&_check_buf, &len,_f_check);
	//close the file and cut of the newline character at the end if it is present
	fclose(_f_check);
	if( strchr(_check_buf,'\n') ) *strchr(_check_buf,'\n')=0;

	//decode the initialisation vector and set it in the oaes struct
	oaes_base64_decode(_iv_ent, strlen(_iv_ent), NULL, &_buf_len );
	_buf = (uint8_t *)calloc(_buf_len, sizeof(uint8_t));
	oaes_base64_decode(_iv_ent, strlen(_iv_ent), _buf, &_buf_len );
	memcpy(_iv, _buf, __min(OAES_BLOCK_SIZE, _buf_len));
	for( int _i = OAES_BLOCK_SIZE; _i < _buf_len; _i++ )
		_iv[_i % OAES_BLOCK_SIZE] ^= _buf[_i];
	free(_buf);

	if( OAES_RET_SUCCESS != oaes_set_option(ctx, OAES_OPTION_CBC, _iv) ){
		fprintf(stderr, "Error: Failed to set OAES options, invalid initialisation vector?.\n");
		oaes_free(&ctx);
		exit(-1);
	}

	//import the secret bit key into oaes struct
	oaes_key_import_data(ctx, key, keyLen);

	//open the license file
	_f_in = fopen(_file_in, "rb");
    if( NULL == _f_in )
    {
      	fprintf(stderr, "Error: Failed to open license file.\n");
	  	oaes_free(&ctx);
      	exit(-1);
    }

	while( (_b.in_len = fread(_b.in, sizeof(uint8_t), _read_len, _f_in)) )
  	{
		if( _b.in_len < 4096 )
			_b.pad = 1;
		else
			_b.pad = 0;
		_b.out = NULL;
		_b.out_len = 0;
		//get the length of the decrypted license
		_rc = oaes_decrypt( ctx,
			_b.in, _b.in_len,
			NULL, &(_b.out_len),
			NULL, 0 );
		if( OAES_RET_SUCCESS != _rc )
		{
			fprintf(stderr, "Error: Failed to decrypt1. return code: %d\n",_rc);
			oaes_free(&ctx);
			exit(-1);
		}
		//allocate memory for the decrypted license
		_b.out = (uint8_t *) calloc(_b.out_len, sizeof(uint8_t));
		if( NULL == _b.out )
		{
			fprintf(stderr, "Error: Failed to allocate memory.\n");
			oaes_free(&ctx);
			exit(-1);
		}
		//decrypt and store the license
		_rc = oaes_decrypt( ctx,
			_b.in, _b.in_len,
			_b.out, &(_b.out_len),
			_iv, _b.pad );
		if( OAES_RET_SUCCESS != _rc )
		{
			fprintf(stderr, "Error: Failed to decrypt2. return code: %d\n",_rc);
			oaes_free(&ctx);
			free(_b.out);
			exit(-1);
		}

	}
	//close license file
	fclose(_f_in);
	//free the oaes struct memory
	if( OAES_RET_SUCCESS !=  oaes_free(&ctx) )
		fprintf(stderr, "Error: Failed to uninitialize OAES.\n");

	//strip newline at the end
	if( strchr( (char *) _b.out,'\n') ) *strchr( (char *) _b.out,'\n')=0;

	// printf("finished decrypting.\ndecrypted: %s\ncheck file: %s\n",_b.out,_check_buf);
	//compare the decrypted license to the check file
	if (strcmp( (char *) _b.out, _check_buf)){
		fprintf(stderr, "decrypted license does not match the check file. exiting.\n");
		free(_b.out);
		exit(-1);
	}
	//success continue initialisation and the rest of the program
	fprintf(stderr, "license verified!\n");
	free(_b.out);
	return;
}

/****************************************************************************************/

void GocontrollProcessorboard_GetIIOContext(void){
    iioContext = iio_create_local_context();
}
