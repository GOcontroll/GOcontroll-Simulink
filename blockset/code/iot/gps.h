#include <stdint.h>

#include "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.h"
#include "cmsis_os2.h"

#define GPS_STATE_UNKNOWN 0
#define GPS_STATE_OFF 1
#define GPS_STATE_ON 2

#define GPS_STATE_CHANGE 0b1

#define GPS_COMMAND_BASE "AT+CGPS"
#define GPS_COMMAND_BASE_LEN sizeof(GPS_COMMAND_BASE) - 1

struct gps_data {
	float longitude;
	float latitude;
	float altitude;
	float velocity;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
};

struct gps_thread_args {
	struct gps_data* gps_data;
	osMutexId_t gps_data_lock;
	uint8_t thread_run;
};

extern osMessageQueueId_t gps_rx;

void ReadGpsThread(void* args);

void HandleGps(char* rx, uint16_t num_bytes);