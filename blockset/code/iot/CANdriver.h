#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"

#define CAN125KBPS 16
#define CAN250KBPS 8
#define CAN500KBPS 4
#define CAN1MBPS 2

#define CAN_PACKED_DLC 0b001111
#define CAN_PACKED_EXTID 0b010000
#define CAN_PACKED_RTR 0b100000

struct can_frame {
	uint8_t data[8];
	uint32_t id;
	uint8_t flags;
};

int init_can(CAN_HandleTypeDef* hcan, uint32_t baudrate);

void can_pack_header(struct can_frame* frame, CAN_RxHeaderTypeDef* header);

uint8_t can_packed_dlc(struct can_frame* frame);
bool can_packed_is_ExtId(struct can_frame* frame);
bool can_packed_is_RTR(struct can_frame* frame);