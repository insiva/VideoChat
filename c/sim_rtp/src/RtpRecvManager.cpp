/*
 * RtpRecvManager.cpp
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#include <sim_rtp/RtpRecvManager.h>

RtpRecvManager::RtpRecvManager(ushort localPort, RtpManager *rtpManager) :
		pRtpManager(rtpManager) {
	this->mRtpPort = localPort - localPort % 2;
	this->mRtcpPort = this->mRtpPort + 1;
	this->pRtpListener = new UdpListener(this->mRtpPort);
	this->pRtcpListener = new UdpListener(this->mRtcpPort);
	this->pRtpThread = new XThread();
	this->pRtcpThread = new XThread();
	this->pRtpRecvedFuncHandler = new FuncHandler<FuncOnDataRecved>(this,
			RtpRecvManager::onRtpDataRecved);
	this->pRtcpRecvedFuncHandler = new FuncHandler<FuncOnDataRecved>(this,
			RtpRecvManager::onRtcpDataRecved);
	this->pRtcpFeedbackSocket = new UdpSocket();
}

RtpRecvManager::~RtpRecvManager() {
	delete this->pRtpListener;
	delete this->pRtcpListener;
	delete this->pRtpRecvedFuncHandler;
	delete this->pRtcpRecvedFuncHandler;
	delete this->pRtcpFeedbackSocket;
}

int RtpRecvManager::listenRtpAsync() {
	this->pRtpListener->setDataRecvedFunc(this->pRtpRecvedFuncHandler);
	this->pRtpThread->setRunnable(&RtpRecvManager::listen);
	int err = this->pRtpThread->start(this->pRtpListener);
	XERR(err, "Start Rtp Listener Thread");
	return err;
}

int RtpRecvManager::listenRtcpAsync() {
	this->pRtcpListener->setDataRecvedFunc(this->pRtcpRecvedFuncHandler);
	this->pRtcpThread->setRunnable(&RtpRecvManager::listen);
	int err = this->pRtcpThread->start(this->pRtcpListener);
	XERR(err, "Start Rtcp Listener Thread");
	return err;
}

void *RtpRecvManager::listen(void *ptr) {
	UdpListener *listener = (UdpListener *) ptr;
	int err = listener->listen();
	return (void *) err;
}

void RtpRecvManager::onRtpDataRecved(const char *buffer, size_t len,
		struct sockaddr *fromAddr, void *invoker) {
	if (invoker == XNULL) {
		return;
	}
	RtpPacket * rp = RtpPacket::obtain();
	int err = RtpPacket::parse(buffer, len, rp);
	if (err < 0) {
		return;
	}
	RtpRecvManager *manager = (RtpRecvManager*) invoker;
	manager->pRtpManager->onRtpPacketRecved(rp);
	rp->recycle();
}

void RtpRecvManager::onRtcpDataRecved(const char *buffer, size_t len,
		struct sockaddr * fromAddr, void *invoker) {
	if (invoker == XNULL) {
		return;
	}
	RtpRecvManager *rrManager = (RtpRecvManager *) invoker;
	RtcpPacket * rcp = RtcpPacket::obtain();
	int err = RtcpPacket::parse(buffer, len, rcp);
	if (err < 0) {
		return;
	}
	if (rcp->needFeedback()) {
		rrManager->pRtcpFeedbackSocket->set(rcp->getFromIp(),
				rcp->getFromPort(), false);
		RtcpPacket *fbRcp = RtcpPacket::obtain(rcp->getType(),
				rrManager->pRtpManager->mLocalIp,
				rrManager->pRtpManager->mLocalRtcpPort, true, false,
				rrManager->pRtpManager->mSsrc,
				++(rrManager->pRtpManager->mRtcpSequence));
		uint sequence = rcp->getSequence();
		fbRcp->setExtraData((uchar *) &sequence, sizeof(uint));
		rrManager->pRtcpFeedbackSocket->send(fbRcp->getBytes(),
				fbRcp->getBytesLength());
		fbRcp->recycle();
	}
	RtpRecvManager *manager = (RtpRecvManager*) invoker;
	manager->pRtpManager->onRtcpPacketRecved(rcp);
	//delete rcp;
	rcp->recycle();
}
