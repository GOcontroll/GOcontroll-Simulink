#include <stdbool.h>
#include <stdint.h>

#include "stm32h5xx_hal.h"

/* Baudrate index for init_can() - maps to timing_table entry */
#define CAN125KBPS 0
#define CAN250KBPS 1
#define CAN500KBPS 2
#define CAN1MBPS   3

#define CAN_PACKED_DLC    0b001111
#define CAN_PACKED_EXTID  0b010000
#define CAN_PACKED_RTR    0b100000

struct can_frame {
	uint8_t data[8];
	uint32_t id;
	uint8_t flags;
};

int init_can(FDCAN_HandleTypeDef* hfdcan, uint32_t baudrate,
			 FunctionalState autort);

void can_pack_header(struct can_frame* frame, FDCAN_RxHeaderTypeDef* header);

uint8_t can_packed_dlc(struct can_frame* frame);
bool can_packed_is_ExtId(struct can_frame* frame);
bool can_packed_is_RTR(struct can_frame* frame);
