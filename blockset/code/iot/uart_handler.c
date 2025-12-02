#include "uart_handler.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.h"
#include "cmsis_os2.h"
#include "gpio.h"
#include "gps.h"
#include "print.h"
#include "usart.h"

#define RECEIVE_BUFF_SIZE 512

osMessageQueueId_t simcom_tx = NULL;
osMessageQueueId_t simcom_rx = NULL;

osEventFlagsId_t simcom_events = NULL;
char* current_buff = NULL;
uint16_t current_buff_size = 0;
uint16_t num_bytes_acc = 0;
char receive_buff[RECEIVE_BUFF_SIZE];

uint8_t simcom_state = SIMCOM_NOT_READY;

void SimcomRxReady(uint16_t offset) {
	num_bytes_acc = offset;
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t*)receive_buff + offset,
								 RECEIVE_BUFF_SIZE - 1 - offset);
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
}

void SimcomTxReady(void) { osEventFlagsSet(simcom_events, SIMCOM_CTS); }

void SimcomInit(void) {
	simcom_tx = osMessageQueueNew(1, sizeof(struct uart_message), NULL);
	simcom_events = osEventFlagsNew(NULL);
	HAL_GPIO_WritePin(POWER_EN_PCIE_GPIO_Port, POWER_EN_PCIE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PCIE_RESET_GPIO_Port, PCIE_RESET_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PCIE_ENABLE_GPIO_Port, PCIE_ENABLE_Pin, GPIO_PIN_RESET);
}

void SimcomTxThread(void* args) {
	struct uart_message msg;
	// osEventFlagsWait(simcom_events, SIMCOM_CTS, osFlagsWaitAll,
	// osWaitForever);
	/* disable echo, it is nice for manual control but not automated */
	// HAL_UART_Transmit_DMA(&huart3, (uint8_t*)"ATE0\r", 5);
	while (1) {
		osEventFlagsWait(simcom_events, SIMCOM_CTS, osFlagsWaitAll,
						 osWaitForever);
		osMessageQueueGet(simcom_tx, &msg, 0, osWaitForever);
		dbg("simcom send: %s\n", msg.buff);
		HAL_UART_Transmit_DMA(&huart3, (uint8_t*)msg.buff, msg.command_len);
		if (msg.command_len2) {
			osEventFlagsWait(simcom_events, SIMCOM_CTS, osFlagsWaitAll,
							 osWaitForever);
			HAL_UART_Transmit_DMA(&huart3, (uint8_t*)msg.buff + msg.command_len,
								  msg.command_len2);
		}
	}
	osThreadExit();
}

void SimcomRxThread(void* args) {
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t*)receive_buff,
								 RECEIVE_BUFF_SIZE - 1);
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);

	while (osEventFlagsWait(simcom_events, SIMCOM_RX, osFlagsWaitAll,
							osWaitForever) >= 0) {
		dbg("received from simcom: %s\n", receive_buff);
		/* first message from simcom is unprovoked indicating readyness */
		if (simcom_state == SIMCOM_NOT_READY) {
			if (strstr(receive_buff, "RDY") != NULL) {
				simcom_state = SIMCOM_READY;
				osEventFlagsSet(simcom_events, SIMCOM_STATE_CHANGE);
				SimcomRxReady(0);
				SimcomTxReady();
				continue;
			}
		}
		/* message not complete, listen again at offset */
		if (memcmp(receive_buff + num_bytes_acc - 2, "\r\n", 2) != 0) {
			SimcomRxReady(num_bytes_acc);
			continue;
		}

		if (memcmp(receive_buff, GPS_COMMAND_BASE, GPS_COMMAND_BASE_LEN) == 0)
			HandleGps(receive_buff, num_bytes_acc);

		SimcomRxReady(0);
		SimcomTxReady();
	}
	osThreadExit();
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size) {
	if (huart->Instance == USART3) {
		if (Size >= RECEIVE_BUFF_SIZE) {
			HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t*)receive_buff,
										 RECEIVE_BUFF_SIZE - 1);
			__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
			return;
		}
		num_bytes_acc += Size;
		receive_buff[num_bytes_acc] = 0;

		osEventFlagsSet(simcom_events, SIMCOM_RX);
	}
}