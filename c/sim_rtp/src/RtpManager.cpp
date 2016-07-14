/*
 * RtpManager.cpp
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#include<sim_rtp/RtpManager.h>
#include<utils/XUtils.h>
#include<math.h>

size_t RtpManager::iAvalableSize = RTP_DATA_MAX_SIZE;

RtpManager::RtpManager(const char *localIp, ushort localPort, uint ssrc) :
		pSendManager(XNULL), mLastRtpSeq(0), pDataPacketRecvedFuncHandler(
		XNULL), pRtcpRecvedFuncHandler(XNULL), mRtpSequence(0), mRtcpSequence(
				0), pDataPacket(XNULL) {
	this->pRecvManager = new RtpRecvManager(localPort, this);
	this->mLocalRtpPort = localPort - localPort % 2;
	this->mLocalRtcpPort = this->mLocalRtpPort + 1;
	this->mLocalIp = inet_addr(localIp);
	if (ssrc == 0) {
		ssrc = XUtils::xrand();
	}
	this->mSsrc = ssrc;
}

RtpManager::~RtpManager() {
	delete this->pRecvManager;
	if (this->pSendManager != XNULL) {
		delete this->pSendManager;
	}
}

uint RtpManager::getSsrc() const {
	return this->mSsrc;
}

void RtpManager::connectRemote(const char* remoteIp, ushort port) {
	if (this->pSendManager == 0) {
		this->pSendManager = new RtpSendManager(this);
	}
	this->pSendManager->setRemoteIPAndPort(remoteIp, port);
}

void RtpManager::disconnectRemote() {
	//this->mSendManager!=0?this->mSendManager->reset():this->mSendManager;
	if (this->pSendManager != XNULL) {
		this->pSendManager->reset();
		this->pSendManager = XNULL;
	}
	mLastRtpSeq = 0;
}

int RtpManager::sendRtp(RtpType rt, const uchar *buffer, size_t len) {
	XASSERT(this->pSendManager != XNULL, "SendManager not inited.");

	ushort packetCount = ceil((double) len / (double) RTP_DATA_MAX_SIZE);
	if (packetCount > SUB_PACKET_MAX_COUNT) {
		LOG("This RtpData (Size = %d) is Too Big!\n", len);
		return ERROR_CODE_RTP_DATA_OVERSIZE;
	}
	this->mRtpSequence++;
	uint currentMilliSeconds = XUtils::currentMilliSeconds()
			% (24 * 3600 * 1000);
	size_t offset = 0;
	int err = 0;
	RtpPacket *rp = RtpPacket::obtain(this->mSsrc, rt, currentMilliSeconds,
			this->mRtpSequence);
	for (int i = 0; i < packetCount; i++) {
		size_t myLen =
				(len - offset) > iAvalableSize ? iAvalableSize : (len - offset);
		rp->setSubCount(packetCount)->setSubSequence(i)->setTotalLength(len)->setLength(
				myLen)->setOffset(offset);
		offset = rp->setData(buffer, offset, myLen);
		err = this->pSendManager->send(rp);
		if (err < 0) {
			return err;
		} else {
			//this->mPacksSent++;
			//this->mOcetsSent += rp->getBytesLength();
		}
	}
	/*
	{
		char *hex = new char[2 * len + 1];
		XUtils::bytesToHexString(buffer, len, hex);
		LOG("SEND(%d):%s\n", len, hex);
		delete[] hex;
	}*/
	return err;
}

int RtpManager::sendRtcp(RtcpType rct, bool needFeedback, uint milliSeconds,
		const uchar *data, size_t length) {
	XASSERT(this->pSendManager != XNULL, "SendManager not inited.");
	RtcpPacket *rcp = RtcpPacket::obtain(rct, this->mLocalIp,
			this->mLocalRtcpPort, false, needFeedback, this->mSsrc,
			++mRtcpSequence, milliSeconds);
	if (length != 0) {
		rcp->setExtraData(data, length);
	}
	int err = this->pSendManager->send(rcp);
	if (err > 0) {

	}
	/*
	{
		char *hex = new char[2 * rcp->getBytesLength() + 1];
		XUtils::bytesToHexString(rcp->getBytes(), rcp->getBytesLength(), hex);
		LOG("SEND(%d):%s\n", rcp->getBytesLength(), hex);
		delete[] hex;
	}*/
	rcp->recycle();
	return err;
}

void RtpManager::onRtpPacketRecved(RtpPacket *rp) {
	if (this->pDataPacket == XNULL) {
		this->pDataPacket = DataPacket::obtain(rp->getType(),
				rp->getTotalLength(), rp->getSequence(), rp->getSubCount());
	}
	int err = DP_CODE_DEFAULT;
	if (this->mLastRtpSeq <= rp->getSequence()) {
		if (this->pDataPacket->mSequence == rp->getSequence()) { //同一帧内的包
			err = this->pDataPacket->addRtpPacket(rp);
		} else { //上一帧有丢包
			this->invokeDataPacketRecvedFunc();
			this->pDataPacket = DataPacket::obtain(rp->getType(),
					rp->getTotalLength(), rp->getSequence(), rp->getSubCount());
			err = this->pDataPacket->addRtpPacket(rp);
		}
		this->mLastRtpSeq = rp->getSequence();
	} else { //丢弃

	}
	if (err == DP_CODE_PACKET_COMPLETE) {
		{
			/*
			char *hex = new char[2 * this->pDataPacket->getLength() + 1];
			XUtils::bytesToHexString(this->pDataPacket->getBuffer(),
					this->pDataPacket->getLength(), hex);
			LOG("RECV(%d):%s\n", this->pDataPacket->getLength(), hex);
			delete[] hex;*/
		}
		this->invokeDataPacketRecvedFunc();
	}
}

inline void RtpManager::invokeDataPacketRecvedFunc() {
	if (this->pDataPacket != XNULL) {
		if (this->pDataPacketRecvedFuncHandler != XNULL) {
			this->pDataPacketRecvedFuncHandler->pFunc(this->pDataPacket,
					this->pDataPacketRecvedFuncHandler->pInvoker);
		}
		this->pDataPacket->recycle();
		this->pDataPacket = XNULL;
	}
}

void RtpManager::onRtcpPacketRecved(RtcpPacket *rcp) {
	if (this->pRtcpRecvedFuncHandler != XNULL) {
		this->pRtcpRecvedFuncHandler->pFunc(rcp,
				this->pRtcpRecvedFuncHandler->pInvoker);
	}
}

RtpManager* RtpManager::create(const char *localIp, ushort port, uint ssrc) {
	RtpManager* instance = new RtpManager(localIp, port, ssrc);
	return instance;
}

void RtpManager::destroy(RtpManager* ptr) {
	if (ptr != 0) {
		delete ptr;
	}
}

int RtpManager::listenAsync() {
	//return this->pRecvManager->listen(XNULL);
	int err = this->pRecvManager->listenRtpAsync();
	if (err != 0) {
		return err;
	}
	err = this->pRecvManager->listenRtcpAsync();
	return err;
}

void RtpManager::setOnDataPacketRecvedFunc(
		FuncHandler<FuncOnDataPacketRecved> *handler) {
	this->pDataPacketRecvedFuncHandler = handler;
}

void RtpManager::setOnRtcpRecvedFunc(FuncHandler<FuncOnRtcpRecved> * handler) {
	this->pRtcpRecvedFuncHandler = handler;
}
