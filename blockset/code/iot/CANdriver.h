#include <stdint.h>

#include "stm32f4xx_hal.h"

#define CAN125KBPS 16
#define CAN250KBPS 8
#define CAN500KBPS 4
#define CAN1MBPS 2

struct can_frame_rx {
	CAN_RxHeaderTypeDef header;	 // this thing is way to big, fix this somehow?
	uint8_t data[8];
};

int init_can(CAN_HandleTypeDef *hcan, uint32_t baudrate);