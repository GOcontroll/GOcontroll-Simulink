#include "XcpTargetSpecific.h"

#include "SEGGER_RTT.h"
#define DEBUG 1

uint8_t XcpCanSend(uint8_t *data);

static uint8_t dataToSend[8] = {0};
uint32_t xcpDtoId;
void *XcpConnection_fd;
static uint8_t xcpTransmissionBus = 0;

// TODO Channels must be assigned during timer creation.
_eventChannel eventChannel[3] = {
	{"EvChnl1"},
	{"EvChnl2"},
	{"EvChnl3"},
};

void XcpCanHandler(CAN_HandleTypeDef *hcan) {
	struct can_frame_rx message;
	SEGGER_RTT_printf(0, "received CAN message\n");
	while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &message.header,
								message.data) == HAL_OK) {
#if DEBUG == 1
		SEGGER_RTT_printf(0, "received CAN message, dlc: %d, id: %x\ndata: [",
						  message.header.DLC, message.header.ExtId);
		for (int i = 0; i < message.header.DLC; i++) {
			SEGGER_RTT_printf(0, "%02x,", message.data[i]);
		}
		SEGGER_RTT_printf(0, "]\n");
#endif
		XcpCommunicationHandling(message.data, message.header.DLC, dataToSend);
	}
}

void InitXcpCan(CAN_HandleTypeDef *hcan, uint32_t canCtoId, uint32_t canDtoId) {
	xcpTransmissionBus = XCPCAN;
	xcpDtoId = canDtoId;
	XcpConnection_fd = (void *)hcan;
	XcpDynamicConfigurator(0, 8, 8);
	CAN_FilterTypeDef filter = {0};
	// filter.FilterIdLow = canCtoId & 0xffff;
	// filter.FilterIdHigh = (canCtoId >> 16) & 0xffff;
	filter.FilterIdLow = 0x0000;
	filter.FilterIdHigh = 0x0000;
	filter.FilterMaskIdHigh = 0x0000;
	filter.FilterMaskIdLow = 0x0000;
	// send all xcp data to fifo1, other data will go to fifo0
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	// filter.FilterMode = CAN_FILTERMODE_IDLIST;
	filter.FilterMode = CAN_FILTERMODE_IDMASK;
	filter.FilterBank = 0;	// maybe it should be the first one?
	filter.FilterScale = CAN_FILTERSCALE_32BIT;
	filter.FilterActivation = CAN_FILTER_ENABLE;
	filter.SlaveStartFilterBank =
		14;	 // this should probably be set based on can if
	if (HAL_CAN_ConfigFilter(hcan, &filter) != HAL_OK)
		SEGGER_RTT_printf(0, "Could not config filter: 0x%x\n",
						  hcan->ErrorCode);
	if (HAL_CAN_RegisterCallback(hcan, HAL_CAN_RX_FIFO1_MSG_PENDING_CB_ID,
								 XcpCanHandler) != HAL_OK)
		SEGGER_RTT_printf(0, "Could not register callback: 0x%x\n",
						  hcan->ErrorCode);
	if (HAL_CAN_ActivateNotification(
			hcan, HAL_CAN_RX_FIFO1_MSG_PENDING_CB_ID) != HAL_OK)
		SEGGER_RTT_printf(0, "Could not activate notification: 0x%x\n",
						  hcan->ErrorCode);
}

uint8_t XcpSendData(uint8_t *data) {
	switch (xcpTransmissionBus) {
		case XCPCAN:
			return XcpCanSend(data);
		default:
			return 1;
	}
}

uint8_t XcpCanSend(uint8_t *data) {
	CAN_TxHeaderTypeDef header;
	HAL_StatusTypeDef res;
	if (data[0] != 0 && data[0] <= 8) {
		header.DLC = data[0];
		header.StdId = xcpDtoId;
#if DEBUG == 1
		SEGGER_RTT_printf(0, "sending CAN message, dlc: %d, id: %x\ndata: [",
						  header.DLC, header.StdId);
		for (int i = 0; i < header.DLC; i++) {
			SEGGER_RTT_printf(0, "%02x,", data[i + 1]);
		}
		SEGGER_RTT_printf(0, "]\n");
#endif
		res = HAL_CAN_AddTxMessage(
			(CAN_HandleTypeDef *)XcpConnection_fd, &header, &data[1],
			(uint32_t *)CAN_TX_MAILBOX0);  // mailbox selection?
		if (res == HAL_OK) {
			return 0;
		}
	} else {
#if DEBUG == 1
		SEGGER_RTT_printf(0, "Could not send message, incorrect size: %d\n",
						  data[0]);
#endif
	}
	return 1;
}

void XcpReadData(void *data, uint8_t elements, void *location) {
	switch (elements) {
		case 1:
			*(uint8_t *)data = *(uint8_t *)location;
			break;
		case 2:
			*(uint16_t *)data = *(uint16_t *)location;
			break;
		case 4:
			*(uint32_t *)data = *(uint32_t *)location;
			break;
		case 8:
			*(uint64_t *)data = *(uint64_t *)location;
			break;
	}
}

void XcpWriteData(void *data, uint8_t elements, void *location) {
	// TODO check for write protected area's in memory
	switch (elements) {
		case 1:
			*(uint8_t *)location = *(uint8_t *)data;
			break;
		case 2:
			*(uint16_t *)location = *(uint16_t *)data;
			break;
		case 4:
			*(uint32_t *)location = *(uint32_t *)data;
			break;
		case 8:
			*(uint64_t *)location = *(uint64_t *)data;
			break;
	}
}

void XcpStopConnection(void) { return; }

uint8_t XcpUserCmd(uint8_t *dataReceived) { return 0; }
