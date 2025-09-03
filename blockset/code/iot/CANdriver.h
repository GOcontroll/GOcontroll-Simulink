#include <stdint.h>

#include "stm32f4xx_hal.h"

struct can_frame_rx {
	CAN_RxHeaderTypeDef header;	 // this thing is way to big, fix this somehow?
	uint8_t data[8];
};

int init_can(CAN_HandleTypeDef *hcan, uint32_t baudrate);