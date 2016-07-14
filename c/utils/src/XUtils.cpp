/*
 * XUtils.cpp
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#include <utils/XUtils.h>
#include <time.h>
#include <sys/time.h>

uint XUtils::xrand(uint max) {
	struct timeval tpstart;
	srand(tpstart.tv_usec);
	uint r = rand() % max;
	return r;
}

lint XUtils::currentSeconds() {
	time_t t;
	lint s = time(&t);
	return s;
}

lint XUtils::currentMilliSeconds(lint mod) {
	struct timeval t_time;
	gettimeofday(&t_time, XNULL);
	lint ms = t_time.tv_sec * 1000 + t_time.tv_usec / 1000;
	if (mod > 0) {
		ms = ms % mod;
	}
	return ms;
}

int XUtils::byteToHexString(uchar n, char *hex) {
	switch (n) {
	case 0:
		*hex = '0';
		break;
	case 1:
		*hex = '1';
		break;
	case 2:
		*hex = '2';
		break;
	case 3:
		*hex = '3';
		break;
	case 4:
		*hex = '4';
		break;
	case 5:
		*hex = '5';
		break;
	case 6:
		*hex = '6';
		break;
	case 7:
		*hex = '7';
		break;
	case 8:
		*hex = '8';
		break;
	case 9:
		*hex = '9';
		break;
	case 10:
		*hex = 'A';
		break;
	case 11:
		*hex = 'B';
		break;
	case 12:
		*hex = 'C';
		break;
	case 13:
		*hex = 'D';
		break;
	case 14:
		*hex = 'E';
		break;
	case 15:
		*hex = 'F';
		break;
	}
	return RESULT_OK;
}

int XUtils::bytesToHexString(const uchar *buffer, size_t len, char *hex) {
	for (int i = 0; i < len; i++) {
		char n = buffer[i];
		XUtils::byteToHexString(0x0f & (n >> 4), &hex[2 * i]);
		XUtils::byteToHexString(0x0f & n, &hex[2 * i + 1]);
	}
	hex[2 * len] = 0;
	return RESULT_OK;
}
