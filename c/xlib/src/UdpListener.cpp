/*
 * UdpListener.cpp
 *
 *  Created on: 2016年6月30日
 *      Author: zzh
 */

#include <xlib/UdpListener.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>

UdpListener::UdpListener(const char *ip, ushort port, size_t bufferSize) :
		mBufferSize(bufferSize), mSocketFd(-1), pDataRecvedFuncHandler(0), mEnabled(
				true), mRecving(false) {
	this->pLocalAddr = new sockaddr_in();
	memset(this->pLocalAddr, 0, sizeof(struct sockaddr_in));
	this->pLocalAddr->sin_family = AF_INET;
	this->pLocalAddr->sin_addr.s_addr = inet_addr(ip); //这里不一样
	//this->mLocalAddr->sin_addr.s_addr =  htonl (INADDR_ANY);
	this->pLocalAddr->sin_port = htons(port);
	if (this->pLocalAddr->sin_addr.s_addr == INADDR_NONE) {
		LOG("Incorrect ip address!");
	}
	this->mSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
	this->mBuffer = new char[this->mBufferSize];

}

UdpListener::UdpListener(ushort port, size_t bufferSize) :
		mBufferSize(bufferSize), mSocketFd(-1), pDataRecvedFuncHandler(0), mEnabled(
				true), mRecving(false) {
	this->pLocalAddr = new sockaddr_in();
	memset(this->pLocalAddr, 0, sizeof(struct sockaddr_in));
	this->pLocalAddr->sin_family = AF_INET;
	this->pLocalAddr->sin_addr.s_addr = htonl(INADDR_ANY); //这里不一样
	this->pLocalAddr->sin_port = htons(port);
	if (this->pLocalAddr->sin_addr.s_addr == INADDR_NONE) {
		LOG("Incorrect ip address!");
	}
	this->mSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
	this->mBuffer = new char[this->mBufferSize];
}

UdpListener::~UdpListener() {
	if (this->mRecving) {
		if (this->mEnabled) {
			this->mEnabled = false;
		}
	} else {
		this->free();
	}
}

void UdpListener::setDataRecvedFunc(FuncHandler<FuncOnDataRecved> *handler) {
	this->pDataRecvedFuncHandler = handler;
}

int UdpListener::listen() {
	int err = this->mSocketFd;
	if (err >= 0) {
		err = bind(mSocketFd, (struct sockaddr *) pLocalAddr,
				sizeof(struct sockaddr_in));
		if (err < 0) {
			XERR(err,"Bind failed")
			return err;
		}
		struct sockaddr fromAddr;
		socklen_t len = sizeof(struct sockaddr);
		this->mRecving = true;
		while (this->mEnabled) {
			err = recvfrom(this->mSocketFd, this->mBuffer, this->mBufferSize, 0,
					&fromAddr, &len);
			//LOG("Data Recved::%d.\n", this->mBufferSize);
			if (err < 0) {
				LOG("Data recved error:%d,%s\n", err, gai_strerror(err));
				break;
			} else {
				if (this->pDataRecvedFuncHandler != 0) {
					this->pDataRecvedFuncHandler->pFunc(this->mBuffer, err,
							&fromAddr, this->pDataRecvedFuncHandler->pInvoker);
				}
			}
		}
	}
	this->free();
	return err;
}

void UdpListener::free() {
	this->closeSocket();
	delete[] mBuffer;
	delete this->pLocalAddr;
}

void UdpListener::closeSocket() {
	this->mEnabled = false;
	if (this->mSocketFd >= 0) {
		close(this->mSocketFd);
		this->mSocketFd = -1;
	}
}
