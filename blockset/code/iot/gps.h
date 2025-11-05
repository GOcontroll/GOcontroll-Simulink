#include <stdint.h>

#include "cmsis_os2.h"

void ReadGpsThread(void* args);

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