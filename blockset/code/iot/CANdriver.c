#include "CANdriver.h"

#include "cmsis_os2.h"
#include "main.h"
#include "print.h"

/*
 * FDCAN nominal bit timing for FDCAN clock = 12 MHz (HSE).
 * Formula: bit_rate = FDCAN_clk / (prescaler * (1 + TimeSeg1 + TimeSeg2))
 *
 * Using TimeSeg1=14, TimeSeg2=9, SJW=1 → total 24 TQ for 125/250/500 kbps.
 * Using TimeSeg1=10, TimeSeg2=1, SJW=1 → total 12 TQ for 1 Mbps.
 */
struct fdcan_timing {
	uint32_t prescaler;
	uint32_t seg1;
	uint32_t seg2;
};

static const struct fdcan_timing timing_table[4] = {
	{4, 14, 9},   /* CAN125KBPS: 12MHz / (4 * 24) = 125 kbps */
	{2, 14, 9},   /* CAN250KBPS: 12MHz / (2 * 24) = 250 kbps */
	{1, 14, 9},   /* CAN500KBPS: 12MHz / (1 * 24) = 500 kbps */
	{1, 10, 1},   /* CAN1MBPS:   12MHz / (1 * 12) = 1 Mbps   */
};

void can_pack_header(struct can_frame* frame, FDCAN_RxHeaderTypeDef* header) {
	frame->id = header->Identifier;
	/* DLC: FDCAN uses FDCAN_DLC_BYTES_x macros; lower nibble holds byte count */
	frame->flags = (uint8_t)(header->DataLength >> 16) & CAN_PACKED_DLC;
	frame->flags |= (header->IdType == FDCAN_EXTENDED_ID) ? CAN_PACKED_EXTID : 0;
	frame->flags |= (header->RxFrameType == FDCAN_REMOTE_FRAME) ? CAN_PACKED_RTR : 0;
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

int init_can(FDCAN_HandleTypeDef* hfdcan, uint32_t baudrate,
			 FunctionalState autort) {
	if (baudrate >= 4) {
		err("Invalid baudrate index %d\n", baudrate);
		return -1;
	}

	const struct fdcan_timing* t = &timing_table[baudrate];

	hfdcan->Init.ClockDivider          = FDCAN_CLOCK_DIV1;
	hfdcan->Init.FrameFormat           = FDCAN_FRAME_CLASSIC;
	hfdcan->Init.Mode                  = FDCAN_MODE_NORMAL;
	hfdcan->Init.AutoRetransmission    = autort;
	hfdcan->Init.TransmitPause         = DISABLE;
	hfdcan->Init.ProtocolException     = DISABLE;
	hfdcan->Init.NominalPrescaler      = t->prescaler;
	hfdcan->Init.NominalSyncJumpWidth  = 1;
	hfdcan->Init.NominalTimeSeg1       = t->seg1;
	hfdcan->Init.NominalTimeSeg2       = t->seg2;
	hfdcan->Init.DataPrescaler         = 1;
	hfdcan->Init.DataSyncJumpWidth     = 1;
	hfdcan->Init.DataTimeSeg1          = 1;
	hfdcan->Init.DataTimeSeg2          = 1;
	hfdcan->Init.StdFiltersNbr         = 2;
	hfdcan->Init.ExtFiltersNbr         = 2;
	hfdcan->Init.TxFifoQueueMode       = FDCAN_TX_FIFO_OPERATION;

	if (HAL_FDCAN_Init(hfdcan) != HAL_OK) {
		err("Could not init FDCAN\n");
		return -1;
	}

	if (HAL_FDCAN_Start(hfdcan) != HAL_OK) {
		err("Could not start FDCAN\n");
		return -1;
	}

	dbg("FDCAN init OK, prescaler=%d\n", t->prescaler);

	if (hfdcan->Instance == FDCAN1)
		HAL_GPIO_WritePin(CAN1_SILENT_UCO_GPIO_Port, CAN1_SILENT_UCO_Pin,
						  GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(CAN2_SILENT_UCO_GPIO_Port, CAN2_SILENT_UCO_Pin,
						  GPIO_PIN_RESET);
	return 0;
}
