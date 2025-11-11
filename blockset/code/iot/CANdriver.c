#include "CANdriver.h"

#include "cmsis_os2.h"
#include "main.h"
#include "print.h"

#define CAN_PACKED_DLC 0b001111
#define CAN_PACKED_EXTID 0b010000
#define CAN_PACKED_RTR 0b100000

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

void can_pack_header(struct can_frame* frame, CAN_RxHeaderTypeDef* header) {
	frame->id = header->IDE ? header->ExtId : header->StdId;
	frame->flags = header->DLC & CAN_PACKED_DLC;
	frame->flags |= header->IDE ? CAN_PACKED_EXTID : 0;
	frame->flags |= header->RTR ? CAN_PACKED_RTR : 0;
}

uint8_t can_packed_dlc(struct can_frame* frame) {
	return frame->flags & CAN_PACKED_DLC;
}

bool can_packed_is_ExtId(struct can_frame* frame) {
	return (frame->flags & CAN_PACKED_EXTID) > 0;
}

bool can_packed_is_RTR(struct can_frame* frame) {
	return (frame->flags & CAN_PACKED_RTR) > 0;
}

int init_can(CAN_HandleTypeDef* hcan, uint32_t baudrate) {
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
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
	if (HAL_CAN_Init(hcan) != HAL_OK) {
		err("Could not init CAN %d\n", hcan->Instance);
		return -1;
	}
	dbg("CAN %d init: %x\n", hcan->Instance, hcan->ErrorCode);
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