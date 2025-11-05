#include "uart_handler.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "cmsis_os2.h"
#include "gpio.h"
#include "print.h"
#include "usart.h"

osMessageQueueId_t uart_tx = NULL;
osMessageQueueId_t received = NULL;
char* current_buff = NULL;
uint16_t current_buff_size = 0;
int16_t num_bytes_acc = 0;

void SimcomThread(void* args) {
	int res;
	uint16_t num_bytes = 0;

	struct uart_message msg;
	uart_tx = osMessageQueueNew(1, sizeof(struct uart_message), NULL);
	received = osMessageQueueNew(1, sizeof(int16_t), NULL);

	dbg("Simcom thread start\n");

	HAL_GPIO_WritePin(POWER_EN_PCIE_GPIO_Port, POWER_EN_PCIE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PCIE_RESET_GPIO_Port, PCIE_RESET_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PCIE_ENABLE_GPIO_Port, PCIE_ENABLE_Pin, GPIO_PIN_RESET);

	while (1) {
		dbg("SimcomThread stack space left: %d\n",
			osThreadGetStackSpace(osThreadGetId()));
		osMessageQueueGet(uart_tx, &msg, 0, osWaitForever);
		current_buff = msg.buff;
		current_buff_size = msg.num_bytes;

		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t*)msg.buff,
									 msg.num_bytes - 1);
		__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
		HAL_UART_Transmit_DMA(&huart3, (uint8_t*)msg.buff, msg.command_len);

		res = osMessageQueueGet(received, &num_bytes, 0, msg.timeout);
		dbg("received res: %d, num_bytes: %d, time: %d\n", res, num_bytes,
			osKernelGetTickCount());
		if (res | (num_bytes < 0)) {
			if (num_bytes < 0) res = num_bytes;
			if (res > 0) res = -res;
		}
		HAL_UART_DMAStop(&huart3);
		current_buff = NULL;
		current_buff_size = 0;
		num_bytes_acc = 0;

		osMessageQueuePut(msg.rx, &res, 0, osWaitForever);
	}
	osThreadExit();
}

int at_command(char* command, int command_len, char* buff, int buff_size,
			   uint32_t timeout, osMessageQueueId_t rx) {
	strncpy(buff, command, buff_size);
	int res;
	struct uart_message msg = {
		buff, buff_size, command_len, timeout, rx,
	};

	dbg("at_command: %s\n", command);

	if (osMessageQueuePut(uart_tx, &msg, 0, osWaitForever)) return -EIO;
	if (osMessageQueueGet(rx, &res, 0, osWaitForever)) return -EIO;
	if (res < 0) return res;
	dbg("%s", msg.buff);
	return strstr(buff, "OK\r") == NULL;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size) {
	if (huart->Instance == USART3) {
		num_bytes_acc += Size;

		if (!current_buff | (num_bytes_acc < 2) |
			(num_bytes_acc >= current_buff_size)) {
			num_bytes_acc = -EINVAL;
			osMessageQueuePut(received, &num_bytes_acc, 0, 0);
			return;
		}

		if (!memcmp(&current_buff[num_bytes_acc - 2], "\r\n", 2)) {
			/* intermediate stops only have CR, so CRLF means end of message */
			current_buff[num_bytes_acc] = 0;
			osMessageQueuePut(received, &num_bytes_acc, 0, 0);
		} else {
			/* incomplete message received, restart the DMA to get the rest */
			HAL_UARTEx_ReceiveToIdle_DMA(&huart3,
										 (uint8_t*)current_buff + num_bytes_acc,
										 current_buff_size - num_bytes_acc - 1);
			__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
		}
	}
}