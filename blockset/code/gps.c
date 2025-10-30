#include "gps.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

void parse_gps(char* buff, struct gps_data* gps_data,
			   pthread_mutex_t* gps_data_lock) {
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

	pthread_mutex_lock(gps_data_lock);
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
	pthread_mutex_unlock(gps_data_lock);
	return;

no_msg:
	pthread_mutex_lock(gps_data_lock);
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
	pthread_mutex_unlock(gps_data_lock);
}

void printarr(char* arr, ssize_t count) {
	printf("%s", arr);

	printf("[");
	for (int i = 0; i < count; i++) {
		printf("0x%x,", arr[i]);
	}
	printf("]\n");
}

int at_command(int fd, char* command, ssize_t command_len, char* buff) {
	ssize_t num_bytes;
	num_bytes = write(fd, command, command_len);
	usleep(1000000);
	num_bytes = read(fd, buff, 256);
	if (num_bytes >= 256) {
		return -ENOMEM;
	}
	buff[num_bytes] = 0;

#if DEBUG == 1
	printarr(buff, num_bytes + 1);
#endif

	return strstr(buff, "OK\r") == NULL;
}

void* readGpsThread(void* args) {
	struct gps_thread_args* gps_thread_args = (struct gps_thread_args*)args;
	pthread_mutex_t* gps_data_lock = gps_thread_args->gps_data_lock;
	struct gps_data* gps_data = gps_thread_args->gps_data;
	int tty_fd, ret;
	char buff[256];
	struct termios tty;

	tty_fd = open("/dev/ttymxc1", O_RDWR);
	if (tty_fd < 0) {
		fprintf(stderr, "Could not open tty: %s\n", strerror(tty_fd));
		return 0;
	}

	tcgetattr(tty_fd, &tty);
	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag |= CREAD | CLOCAL;
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;
	tty.c_lflag &= ~ECHOE;
	tty.c_lflag &= ~ECHONL;
	tty.c_lflag &= ~ISIG;
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
	tty.c_oflag &= ~OPOST;
	tty.c_oflag &= ~ONLCR;
	tty.c_cc[VTIME] = 10;
	tty.c_cc[VMIN] = 2;

	cfsetspeed(&tty, B115200);

	tcsetattr(tty_fd, TCSANOW, &tty);

	do {
		ret = at_command(tty_fd, "AT\r", 4, buff);
		if (ret) fprintf(stderr, "test command failed\n");
	} while (ret);

	do {
		ret = at_command(tty_fd, "AT+CGPS=0\r", 11, buff);
		if (ret) fprintf(stderr, "could not disable gps\n");
	} while (ret);

	do {
		ret = at_command(tty_fd, "AT+CGPS=1\r", 11, buff);
		if (ret) fprintf(stderr, "could not enable gps\n");
	} while (ret);

	while (gps_thread_args->thread_run) {
		if (at_command(tty_fd, "AT+CGPSINFO\r", 13, buff)) {
			continue;
		}

		parse_gps(buff, gps_data, gps_data_lock);
	}

	close(tty_fd);
	return 0;
}
