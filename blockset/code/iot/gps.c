#include "gps.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.h"
#include "print.h"
#include "uart_handler.h"

#define GPS_BUFF_SIZE 128

osMessageQueueId_t uart_rx;

void parse_gps(char* buff, struct gps_data* gps_data,
			   osMutexId_t gps_data_lock) {
	char* first_split = NULL;
	char* newline_split = NULL;
	char* comma_split = NULL;
	char *data_end, *data_comma, *token;
	float longitude, latitude, altitude, velocity;
	int temp_int;
	uint16_t year;
	uint8_t month, day, hour, minute, second;
	data_end = strtok_r(buff, ": ", &first_split);
	if (data_end == NULL) goto no_msg;
	data_end = strtok_r(NULL, ": ", &first_split);
	if (data_end == NULL) goto no_msg;

	data_comma = strtok_r(data_end, "\r", &newline_split);
	if (data_comma == NULL) goto no_msg;

	token = strtok_r(data_comma, ",", &comma_split);
	if (token == NULL) goto no_msg;
	if (strnlen(token, 11) >= 4) {
		latitude = strtof(token + 2, NULL) / 60;
		latitude = latitude + (float)(atoi(token) / 100);
	} else {
		/* incomplete message */
		goto no_msg;
	}
	token = strtok_r(NULL, ",", &comma_split);
	if (token == NULL) goto no_msg;
	if (!strcmp(token, "S")) latitude = -latitude;

	token = strtok_r(NULL, ",", &comma_split);
	if (token == NULL) goto no_msg;
	if (strnlen(token, 12) >= 5) {
		longitude = strtof(token + 3, NULL) / 60;
		longitude = longitude + (float)(atoi(token) / 100);
	}
	token = strtok_r(NULL, ",", &comma_split);
	if (token == NULL) goto no_msg;
	if (!strcmp(token, "W")) longitude = -longitude;

	token = strtok_r(NULL, ",", &comma_split);
	if (token == NULL) goto no_msg;
	if (strnlen(token, 6) == 6) {
		temp_int = atoi(token);
		year = (temp_int % 100) + 2000;
		month = (temp_int % 10000) / 100;
		day = temp_int / 10000;
	}

	token = strtok_r(NULL, ",", &comma_split);
	if (token == NULL) goto no_msg;
	if (strnlen(token, 6) == 6) {
		temp_int = atoi(token);
		second = (temp_int % 100);
		minute = (temp_int % 10000) / 100;
		hour = temp_int / 10000;
	}

	token = strtok_r(NULL, ",", &comma_split);
	if (token == NULL) goto no_msg;
	altitude = strtof(token, NULL);

	/* velocity is in knots for some reason */
	token = strtok_r(NULL, ",", &comma_split);
	if (token == NULL) goto no_msg;
	velocity = strtof(token, NULL) * 1.852;

	dbg("GpsThread stack space left: %d\n",
		osThreadGetStackSpace(osThreadGetId()));

	if (!osMutexAcquire(gps_data_lock, 1)) {
		gps_data->longitude = longitude;
		gps_data->latitude = latitude;
		gps_data->altitude = altitude;
		gps_data->velocity = velocity;
		gps_data->year = year;
		gps_data->month = month;
		gps_data->day = day;
		gps_data->hour = hour;
		gps_data->minute = minute;
		gps_data->second = second;
		osMutexRelease(gps_data_lock);
	}
	return;

no_msg:
	if (!osMutexAcquire(gps_data_lock, 1)) {
		gps_data->longitude = 0;
		gps_data->latitude = 0;
		gps_data->altitude = 0;
		gps_data->velocity = 0;
		gps_data->year = 0;
		gps_data->month = 0;
		gps_data->day = 0;
		gps_data->hour = 0;
		gps_data->minute = 0;
		gps_data->second = 0;
		osMutexRelease(gps_data_lock);
	}
}

void ReadGpsThread(void* args) {
	struct gps_thread_args* gps_thread_args = (struct gps_thread_args*)args;
	osMutexId_t gps_data_lock = gps_thread_args->gps_data_lock;
	struct gps_data* gps_data = gps_thread_args->gps_data;
	int ret;
	uint32_t tick;
	char buff[GPS_BUFF_SIZE];

	dbg("Gps thread start\n");

	uart_rx = osMessageQueueNew(1, sizeof(int), NULL);

	do {
		tick = osKernelGetTickCount();
		ret = at_command("AT\r", 4, buff, GPS_BUFF_SIZE, 500, uart_rx);
		if (ret) err("test command failed\n");

	} while (ret);
	osDelayUntil(tick += 500);

	do {
		tick = osKernelGetTickCount();
		ret = at_command("AT+CGPS=0\r", 11, buff, GPS_BUFF_SIZE, 500, uart_rx);
		if (ret) err("could not disable gps\n");
	} while (ret);
	osDelayUntil(tick += 500);

	do {
		tick = osKernelGetTickCount();
		ret = at_command("AT+CGPS=1\r", 11, buff, GPS_BUFF_SIZE, 500, uart_rx);
		if (ret) err("could not enable gps\n");
	} while (ret);
	osDelayUntil(tick += 500);

	tick = osKernelGetTickCount();
	while (gps_thread_args->thread_run) {
		tick += 1000;
		if (!at_command("AT+CGPSINFO\r", 13, buff, GPS_BUFF_SIZE, 900,
						uart_rx)) {
			parse_gps(buff, gps_data, gps_data_lock);
		}
		osDelayUntil(tick);
	}
	osThreadExit();
}
