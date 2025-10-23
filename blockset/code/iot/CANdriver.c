#include "CANdriver.h"

#include "SEGGER_RTT.h"
#include "cmsis_os2.h"
#include "main.h"

uint32_t prescalers[4] = {CAN125KBPS, CAN250KBPS, CAN500KBPS, CAN1MBPS};

/* move to tlc*/
// void can_rx_pending_callback_0(CAN_HandleTypeDef *hcan) {
// 	struct can_frame_rx message;
// 	// HAL_StatusTypeDef res;
// 	while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &message.header,
// 								message.data) == HAL_OK) {
// 		osMessageQueuePut(mid_MsgQueue, &message, 0, 0);
// 	}
// }

int init_can(CAN_HandleTypeDef *hcan, uint32_t baudrate) {
	// make some lookup to get proper clock setting for the desired baudrate
	hcan->Init.Prescaler = prescalers[baudrate];
	hcan->Init.Mode = CAN_MODE_NORMAL;
	hcan->Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan->Init.TimeSeg1 = CAN_BS1_14TQ;
	hcan->Init.TimeSeg2 = CAN_BS2_6TQ;
	hcan->Init.TimeTriggeredMode = DISABLE;
	hcan->Init.AutoBusOff = DISABLE;
	hcan->Init.AutoWakeUp = DISABLE;
	hcan->Init.AutoRetransmission = DISABLE;
	hcan->Init.ReceiveFifoLocked = DISABLE;
	hcan->Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(hcan) != HAL_OK) {
		SEGGER_RTT_printf(0, "Could not init\n");
		return -1;
	}
	SEGGER_RTT_printf(0, "init: %x\n", hcan->ErrorCode);
	HAL_GPIO_WritePin(CAN1_SILENT_UCO_GPIO_Port, CAN1_SILENT_UCO_Pin,
					  GPIO_PIN_RESET);
	HAL_GPIO_WritePin(CAN2_SILENT_UCO_GPIO_Port, CAN2_SILENT_UCO_Pin,
					  GPIO_PIN_RESET);
	return 0;
}

// int init_can_queue(CAN_HandleTypeDef *hcan, osMessageQueueId_t mid_MsgQueue)
// { 	mid_MsgQueue = osMessageQueueNew(20, sizeof(struct can_frame_rx), NULL);
// 	HAL_CAN_RegisterCallback(hcan, HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID,
// 							 can_rx_pending_callback_0);
// }

// int receive_can_frame(CAN_HandleTypeDef *hcan, struct can_frame_rx *frame) {
// 	return (int)osMessageQueueGet(mid_MsgQueue, &frame, 0, 0);
// }