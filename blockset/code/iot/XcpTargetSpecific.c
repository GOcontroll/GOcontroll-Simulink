#include "XcpTargetSpecific.h"

#include <string.h>

#include "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.h"
#include "cmsis_os2.h"
#include "print.h"

uint8_t XcpCanSend(uint8_t* data);

static uint8_t dataToSend[16] = {0};
uint32_t xcpDtoId;
uint8_t xcpDtoIdExt;
void* XcpConnection_fd;
osMessageQueueId_t can_tx_queue;
static uint8_t xcpTransmissionBus = 0;

osMessageQueueId_t xcp_received;

// TODO Channels must be assigned during timer creation.
_eventChannel eventChannel[3] = {
	{"EvChnl1"},
	{"EvChnl2"},
	{"EvChnl3"},
};

void XcpCanHandler(CAN_HandleTypeDef* hcan) {
	CAN_RxHeaderTypeDef header;
	struct can_frame message;
	while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &header, message.data) ==
		   HAL_OK) {
		can_pack_header(&message, &header);
		osMessageQueuePut(xcp_received, &message, 0, 0);
	}
	return;
}

void XcpInit_can(CAN_HandleTypeDef* can_channel, osMessageQueueId_t tx_queue,
				 uint32_t xcp_send_id, uint32_t xcp_receive_id,
				 uint8_t xcp_send_id_extended,
				 uint8_t xcp_receive_id_extended) {
	CAN_FilterTypeDef filter = {0};
	xcpTransmissionBus = XCPCAN;
	xcpDtoId = xcp_send_id;
	xcpDtoIdExt = xcp_send_id_extended;
	XcpConnection_fd = can_channel;
	can_tx_queue = tx_queue;
	XcpDynamicConfigurator(0, 8, 8);

	if (xcp_receive_id_extended) {
		filter.FilterIdHigh = (xcp_receive_id >> 13) & 0xffff;
		filter.FilterIdLow = ((xcp_receive_id << 3) & 0xffff) | 0b100;
	} else {
		filter.FilterIdHigh = (xcp_receive_id << 5) & 0xffff;
		filter.FilterIdLow = 0x0000;
	}

	filter.FilterMaskIdHigh = 0x0000;
	filter.FilterMaskIdLow = 0x0000;
	// send all xcp data to fifo1, other data will go to fifo0
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	filter.FilterMode = CAN_FILTERMODE_IDLIST;
	if (can_channel->Instance == CAN1) {
		filter.FilterBank = 0;
		HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	} else {
		filter.FilterBank = 14;
		HAL_NVIC_SetPriority(CAN2_RX1_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
	}
	filter.FilterScale = CAN_FILTERSCALE_32BIT;
	filter.FilterActivation = CAN_FILTER_ENABLE;
	filter.SlaveStartFilterBank = 14;
	if (HAL_CAN_ConfigFilter(can_channel, &filter) != HAL_OK)
		err("Could not config filter: 0x%x\n", can_channel->ErrorCode);
	if (HAL_CAN_RegisterCallback(can_channel,
								 HAL_CAN_RX_FIFO1_MSG_PENDING_CB_ID,
								 XcpCanHandler) != HAL_OK)
		err("Could not register callback0: 0x%x\n", can_channel->ErrorCode);
	if (HAL_CAN_ActivateNotification(can_channel,
									 CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
		err("Could not activate notification: 0x%x\n", can_channel->ErrorCode);

	xcp_received = osMessageQueueNew(1, sizeof(struct can_frame), NULL);
}

void XcpThread_can(void* args) {
	struct can_frame message;

	while (1) {
		if (!osMessageQueueGet(xcp_received, &message, 0, osWaitForever)) {
			dbg("received CAN message, dlc: %d, id: %x\ndata: [",
				can_packed_dlc(&message), message.id);
			for (int i = 0; i < can_packed_dlc(&message); i++) {
				dbg("%02x,", message.data[i]);
			}
			dbg("]\n");
			dbg("xcp stack free: %d\n", osThreadGetStackSpace(osThreadGetId()));
			XcpCommunicationHandling(message.data, can_packed_dlc(&message),
									 dataToSend);
		}
	}
}

uint8_t XcpSendData(uint8_t* data) {
	switch (xcpTransmissionBus) {
		case XCPCAN:
			return XcpCanSend(data);
		default:
			return 1;
	}
}

uint8_t XcpCanSend(uint8_t* data) {
	struct can_frame message;
	int res;
	if (data[0] != 0 && data[0] <= 8) {
		message.flags = data[0] & CAN_PACKED_DLC;
		message.flags |= xcpDtoIdExt ? CAN_PACKED_EXTID : 0;
		message.id = xcpDtoId;
		memcpy(message.data, &data[1], data[0]);
		res = osMessageQueuePut(can_tx_queue, &message, 0, 0);
		if (!res) {
			return 0;
		}
	} else {
		err("Could not send message, incorrect size: %d\n", data[0]);
	}
	return 1;
}

void XcpReadData(void* data, uint8_t elements, void* location) {
	switch (elements) {
		case 1:
			*(uint8_t*)data = *(uint8_t*)location;
			break;
		case 2:
			*(uint16_t*)data = *(uint16_t*)location;
			break;
		case 4:
			*(uint32_t*)data = *(uint32_t*)location;
			break;
		case 8:
			*(uint64_t*)data = *(uint64_t*)location;
			break;
	}
}

void XcpWriteData(void* data, uint8_t elements, void* location) {
	// TODO check for write protected area's in memory
	switch (elements) {
		case 1:
			*(uint8_t*)location = *(uint8_t*)data;
			break;
		case 2:
			*(uint16_t*)location = *(uint16_t*)data;
			break;
		case 4:
			*(uint32_t*)location = *(uint32_t*)data;
			break;
		case 8:
			*(uint64_t*)location = *(uint64_t*)data;
			break;
	}
}

void XcpStopConnection(void) { return; }

uint8_t XcpUserCmd(uint8_t* dataReceived) { return 0; }
