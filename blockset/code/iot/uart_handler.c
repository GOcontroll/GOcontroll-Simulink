#include "uart_handler.h"

#include <string.h>

#include "cmsis_os2.h"
#include "print.h"

/* NOTE: Simcom cellular modem (USART3 + DMA) is not present on STM32H5 IoT board.
 * GPS and cellular functionality is handled by the ESP32 via EspInterface (USART2).
 * These stubs exist to satisfy the GPS block TLC code generation.
 * Use EspInterface_EnableGps() to enable GPS data from the ESP32. */

osMessageQueueId_t simcom_tx = NULL;
osEventFlagsId_t simcom_events = NULL;
uint8_t simcom_state = SIMCOM_NOT_READY;

void SimcomInit(void) {
	err("Simcom not supported on H5 IoT. Use EspInterface for GPS/LTE.\n");
}

void SimcomTxThread(void* args) {
	osThreadExit();
}

void SimcomRxThread(void* args) {
	osThreadExit();
}
