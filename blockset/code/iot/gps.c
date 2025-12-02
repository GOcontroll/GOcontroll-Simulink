#include "gps.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmsis_os2.h"
#include "print.h"
#include "uart_handler.h"

osEventFlagsId_t gps_events = NULL;
struct gps_data* gps_data = NULL;
osMutexId_t gps_data_lock = 0;
uint8_t gps_state = 0;

void ParseGps(char* buff, struct gps_data* gps_data,
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
		/* divide by 100 to shift the number right 2 decimal places */
		latitude = latitude + (float)((atoi(token) / 100));
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
		/* divide by 100 to shift the number right 2 decimal places */
		longitude = longitude + (float)((atoi(token) / 100));
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

void HandleGps(char* rx, uint16_t num_bytes) {
	uint8_t message_ok = 0;
	if (memcmp(rx + num_bytes - 4, "OK\r\n", 4) == 0) {
		message_ok = 1;
	}
	if (memcmp(rx + GPS_COMMAND_BASE_LEN, "INFO", 4) == 0) {
		if (message_ok) ParseGps(rx, gps_data, gps_data_lock);
		return;
	} else if (memcmp(rx + GPS_COMMAND_BASE_LEN, "=0", 2) == 0) {
		if (message_ok) gps_state = GPS_STATE_OFF;
		osEventFlagsSet(gps_events, GPS_STATE_CHANGE);
	} else if (memcmp(rx + GPS_COMMAND_BASE_LEN, "=1", 2) == 0) {
		if (message_ok) gps_state = GPS_STATE_ON;
		osEventFlagsSet(gps_events, GPS_STATE_CHANGE);
	}
}

void ReadGpsThread(void* args) {
	struct gps_thread_args* gps_thread_args = (struct gps_thread_args*)args;
	gps_data_lock = gps_thread_args->gps_data_lock;
	gps_data = gps_thread_args->gps_data;
	uint32_t tick;
	struct uart_message message;

	dbg("Gps thread start\n");

	gps_events = osEventFlagsNew(NULL);

	do {
		osEventFlagsWait(simcom_events, SIMCOM_STATE_CHANGE, osFlagsWaitAll,
						 osWaitForever);
	} while (simcom_state != SIMCOM_READY);

	message.buff = "AT+CGPS=0\r";
	message.command_len = 11;
	message.command_len2 = 0;
	do {
		tick = osKernelGetTickCount();
		osMessageQueuePut(simcom_tx, &message, 0, osWaitForever);
		osEventFlagsWait(gps_events, GPS_STATE_CHANGE, osFlagsWaitAll,
						 osWaitForever);

	} while (gps_state != GPS_STATE_OFF);

	message.buff = "AT+CGPS=1\r";
	message.command_len = 11;
	message.command_len2 = 0;
	do {
		tick = osKernelGetTickCount();
		osMessageQueuePut(simcom_tx, &message, 0, osWaitForever);
		osEventFlagsWait(gps_events, GPS_STATE_CHANGE, osFlagsWaitAll,
						 osWaitForever);
	} while (gps_state != GPS_STATE_ON);

	tick = osKernelGetTickCount();
	message.buff = "AT+CGPSINFO\r";
	message.command_len = 13;
	message.command_len2 = 0;
	while (gps_thread_args->thread_run) {
		tick += 1000;
		osMessageQueuePut(simcom_tx, &message, 0, osWaitForever);

		osDelayUntil(tick);
	}
	osThreadExit();
}