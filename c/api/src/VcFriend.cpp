/*
 * VcFriend.cpp
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#include<api/VcFriend.h>
#include<string.h>

VcFriend::VcFriend(uint ssrc, const char * ip, ushort port) :
		mSsrc(ssrc), mPort(port) {
	this->pIp = new char[strlen(ip) + 1];
	strcpy(this->pIp, ip);
}

VcFriend::~VcFriend() {
	delete[] this->pIp;
}

int VcFriend::getSsrc() const {
	return this->mSsrc;
}

const char *VcFriend::getIp() const {
	return this->pIp;
}

ushort VcFriend::getPort() const {
	return this->mPort;
}

VcFriend *VcFriend::setIp(const char *ip) {
	delete[] pIp;
	this->pIp = new char[strlen(ip) + 1];
	strcpy(this->pIp, ip);
	return this;
}

VcFriend *VcFriend::setPort(unsigned short port) {
	this->mPort = port;
	return this;
}
