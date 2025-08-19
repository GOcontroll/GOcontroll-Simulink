#include "CANdriver.h"

#include <stdint.h>

#include "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.h"
#include "cmsis_os2.h"
#include "stm32f4xx_hal_can.h"

osMessageQueueId_t mid_MsgQueue;

void can_rx_pending_callback_0(AN_HandleTypeDef *hcan) {
	struct can_frame message;
	HAL_StatusTypeDef res;
	while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &message.header,
								message.data) == HAL_OK) {
		osMessageQueuePut(mid_MsgQueue, &message, 0, NULL);
	}
}

int init_can(CAN_HandleTypeDef *hcan, uint32_t baudrate) {
	// hcan->Instance = CAN1; this should probably be in the tlc
	// make some lookup to get proper clock setting for the desired baudrate
	hcan->Init.Prescaler = 16;
	hcan->Init.Mode = CAN_MODE_NORMAL;
	hcan->Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan->Init.TimeSeg1 = CAN_BS1_1TQ;
	hcan->Init.TimeSeg2 = CAN_BS2_1TQ;
	hcan->Init.TimeTriggeredMode = DISABLE;
	hcan->Init.AutoBusOff = DISABLE;
	hcan->Init.AutoWakeUp = DISABLE;
	hcan->Init.AutoRetransmission = DISABLE;
	hcan->Init.ReceiveFifoLocked = DISABLE;
	hcan->Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(hcan) != HAL_OK) {
		return -1;
	}
	mid_MsgQueue = osMessageQueueNew(20, sizeof(can_frame), NULL);
	HAL_CAN_RegisterCallback(hcan, HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID,
							 can_rx_pending_callback_0);
	HAL_CAN_Start(hcan);
}

int receive_can_frame(CAN_HandleTypeDef *hcan, can_frame *frame) {
	return (int)osMessageQueueGet(mid_MsgQueue, &frame, 0, NULL);
}