/*
 * UdpSocket.cpp
 *
 *  Created on: 2016年6月30日
 *      Author: zzh
 */
#include <xlib/UdpSocket.h>

#include<string.h>
#include <unistd.h>

UdpSocket::UdpSocket(const char *ip, ushort port, bool broadcast) :
		mSocketFd(0), iIp(0), iPort(0) {
	this->mDestAddr = new sockaddr_in();
	if (ip != XNULL) {
		this->set(ip, port, broadcast);
	}
}

UdpSocket::~UdpSocket() {
	this->closeSocket();
}

void UdpSocket::set(const char *ip, ushort port, bool broadcast) {
	this->iIp = inet_addr(ip);
	this->iPort = port;
	this->set(this->iIp, port, broadcast);
}

void UdpSocket::set(uint intIp, ushort port, bool broadcast) {
	memset(this->mDestAddr, 0, sizeof(struct sockaddr_in));
	if (this->mSocketFd > 0) {
		close(this->mSocketFd);
	}
	if (intIp >= 0) {
		this->mDestAddr->sin_family = AF_INET;
		this->mDestAddr->sin_addr.s_addr = intIp; //这里不一样
		this->mDestAddr->sin_port = htons(port);
		this->mSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
		if (broadcast) {
			int i = 1;
			socklen_t len = sizeof(i);
			int err = setsockopt(mSocketFd, SOL_SOCKET, SO_BROADCAST, &i, len);
			if (err < 0) {
				LOG("set broadcast mode failed:%d,%s\n", err,
						gai_strerror(err));
			}
		}
	}
}

int UdpSocket::send(const uchar *buff, size_t len) {
	int err = RESULT_OK;
	if (this->mSocketFd < 0) {
		err = RESULT_ERROR;
		LOG("Socket inited failed!");
	} else {
		err = sendto(this->mSocketFd, buff, len, 0,
				(struct sockaddr *) mDestAddr, sizeof(*mDestAddr));
	}
	return err;
}

void UdpSocket::closeSocket() {
	if (this->mSocketFd >= 0) {
		close(this->mSocketFd);
		this->mSocketFd = -1;
		LOG("UDP socket(%d:%d) closed!\n",this->iIp,this->iPort);
	}
}
