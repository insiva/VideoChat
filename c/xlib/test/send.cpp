/*
 * main.cpp
 *
 *  Created on: 2016年6月30日
 *      Author: zzh
 */

#include <string.h>

#include <xlib/UdpSocket.h>

int main(int argc, char** argv) {
	int aa[4];
	LOG("a=%d,SIZEOF(a)=%d\n", aa, sizeof(aa));
	UdpSocket* us = new UdpSocket("255.255.255.255", 5678, true);
	const char* a = "matteo";
	LOG("argc len:%d\n", argc);
	if (argc > 1) {
		a = argv[1];
	}
	int err = us->send(a, strlen(a));
	if (err < 0) {
		LOG("send:%s,err=%s\n", a, gai_strerror(err));

	} else {
		LOG("send:%s,length=%d,err=%d\n", a, strlen(a), err);
	}
	delete us;
	return 0;
}
