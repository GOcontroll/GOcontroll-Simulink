#include <stdint.h>

#include "cmsis_os2.h"

struct uart_message {
	char* buff;
	int num_bytes;
	int command_len;
	uint32_t timeout;
	osMessageQueueId_t rx;
};

void SimcomThread(void* args);

int at_command(char* command, int command_len, char* buff, int buff_size,
			   uint32_t timeout, osMessageQueueId_t rx);