#include <stdint.h>

#include "cmsis_os2.h"

#define SIMCOM_CTS 0b1
#define SIMCOM_RX 0b10
#define SIMCOM_STATE_CHANGE 0b100

#define SIMCOM_NOT_READY 0
#define SIMCOM_READY 1

struct uart_message {
	char* buff;
	int16_t command_len;
	int16_t command_len2;
};

extern osMessageQueueId_t simcom_tx;
extern osEventFlagsId_t simcom_events;
extern uint8_t simcom_state;

void SimcomInit(void);
void SimcomTxThread(void* args);
void SimcomRxThread(void* args);
