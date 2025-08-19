#include <stdint.h>

#include "stm32f4xx_hal_can.h"

struct can_frame_rx {
	CAN_RxHeaderTypeDef header;	 // this thing is way to big, fix this somehow?
	uint8_t data[8];
};