/*
 * RtpSendManager.cpp
 *
 *  Created on: 2016年7月4日
 *      Author: zzh
 */

#include <sim_rtp/RtpSendManager.h>
#include <string.h>

RtpSendManager::RtpSendManager(RtpManager *rtpManager) :
		mRtpPort(0), mRtcpPort(0), pRtpSocket(XNULL), pRtcpSocket(0), pRemoteIp(
		XNULL), pRtpManager(rtpManager) {
}

RtpSendManager::~RtpSendManager() {
	this->reset();
}

void RtpSendManager::reset() {
	this->mRtcpPort = this->mRtpPort = 0;
	if (this->pRemoteIp != 0) {
		delete[] this->pRemoteIp;
		this->pRemoteIp = XNULL;
	}
	if (this->pRtpSocket != XNULL) {
		delete this->pRtpSocket;
		this->pRtpSocket = XNULL;
	}
	if (this->pRtcpSocket != XNULL) {
		delete this->pRtcpSocket;
		this->pRtcpSocket = XNULL;
	}
}

RtpSendManager* RtpSendManager::setRemoteIPAndPort(const char *remoteIp,
		ushort port) {
	this->pRemoteIp = new char[strlen(remoteIp) + 1];
	strcpy(this->pRemoteIp, remoteIp);
	this->mRtpPort = port - port % 2;
	this->mRtcpPort = this->mRtpPort + 1;
	this->pRtpSocket = new UdpSocket(this->pRemoteIp, this->mRtpPort);
	this->pRtcpSocket = new UdpSocket(this->pRemoteIp, this->mRtcpPort);
	return this;
}

int RtpSendManager::send(RtpPacket *rp) const {
	XASSERT(pRtpSocket != XNULL, "RtpSocket not inited!");
	return this->pRtpSocket->send(rp->getBytes(), rp->getBytesLength());
}

int RtpSendManager::send(RtcpPacket *rcp) const {
	XASSERT(pRtcpSocket != XNULL, "RtcpSocket not inited!");
	int err = this->pRtcpSocket->send(rcp->getBytes(), rcp->getBytesLength());
	err = err > 0 ? 0 : err;
	return err;
}
