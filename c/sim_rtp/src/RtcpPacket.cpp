/*
 * RtcpPacket.cpp
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#include <sim_rtp/RtcpPacket.h>
#include<utils/XUtils.h>

XPool<RtcpPacket> *RtcpPacket::pPool = new XPool<RtcpPacket>();

RtcpPacket::RtcpPacket(RtcpType rt, uint fromIp, ushort fromPort,
		bool isFeedback, bool needFeedback, uint ssrc, uint sequence,
		uint milliSeconds) {
	this->pBuffer = new uchar[RTCP_PACKET_SIZE];
	memset(this->pBuffer, 0, RTCP_PACKET_SIZE);
	this->pHeader = (RtcpHeader *) this->pBuffer;
	this->mBytesLength = sizeof(RtcpHeader);
	this->pHeader->mFromIp = fromIp;
	this->pHeader->mFromPort = fromPort;
	this->pHeader->mType = rt;
	this->pHeader->bIsFeedback = isFeedback ? 1 : 0;
	this->pHeader->mNeedFeedback = needFeedback ? 1 : 0;
	this->pHeader->mSsrc = ssrc;
	this->pHeader->mSequence = sequence;
	this->pHeader->mMilliSeconds =
			milliSeconds == 0 ?
					XUtils::currentMilliSeconds(24 * 3600 * 1000) :
					milliSeconds;
}

RtcpPacket::~RtcpPacket() {
	delete[] this->pBuffer;
	DLOG("RtcpPacket Delete!\n");
}

RtcpType RtcpPacket::getType() const {
	return (RtcpType) this->pHeader->mType;
}

bool RtcpPacket::needFeedback() const {
	return this->pHeader->mNeedFeedback == 0 ? false : true;
}

bool RtcpPacket::isFeedback() const {
	return this->pHeader->bIsFeedback == 0 ? false : true;
}

uint RtcpPacket::getSequence() const {
	return this->pHeader->mSequence;
}

uint RtcpPacket::getMilliSeconds() const {
	return this->pHeader->mMilliSeconds;
}

size_t RtcpPacket::getExtraDataLength() const {
	return this->pHeader->mExtraDataLength;
}

const uchar *RtcpPacket::getExtraData() const {
	return this->pBuffer + sizeof(RtcpHeader); //this->pHeader + sizeof(RtcpHeader);
}

uint RtcpPacket::getFromIp() const {
	return this->pHeader->mFromIp;
}

ushort RtcpPacket::getFromPort() const {
	return this->pHeader->mFromPort;
}

uint RtcpPacket::getSsrc() const {
	return this->pHeader->mSsrc;
}

RtcpPacket *RtcpPacket::setFromIp(uint fromIp) {
	this->pHeader->mFromIp = fromIp;
	return this;
}

RtcpPacket *RtcpPacket::setType(RtcpType rt) {
	this->pHeader->mType = rt;
	return this;
}

RtcpPacket *RtcpPacket::setNeedFeedback(bool needFeedback) {
	this->pHeader->mNeedFeedback = needFeedback ? 1 : 0;
	return this;
}

RtcpPacket *RtcpPacket::setSequence(uint sequence) {
	this->pHeader->mSequence = sequence;
	return this;
}

RtcpPacket *RtcpPacket::setMilliSeconds(uint milliSeconds) {
	this->pHeader->mMilliSeconds = milliSeconds;
	return this;
}

RtcpPacket *RtcpPacket::setExtraData(const uchar *buffer, size_t len,
		size_t offset) {
	memcpy(this->pBuffer + sizeof(RtcpHeader), buffer + offset, len);
	this->mBytesLength = len + sizeof(RtcpHeader);
	return this;
}

RtcpPacket *RtcpPacket::setSsrc(uint ssrc) {
	this->pHeader->mSsrc = ssrc;
	return this;
}

int RtcpPacket::parse(const char *buffer, size_t len, RtcpPacket * rcp) {
	if(len>RTCP_PACKET_SIZE){
		return RESULT_ERROR;
	}
	memcpy(rcp->pBuffer, buffer, len);
	rcp->mBytesLength = len;
	return RESULT_OK;
}

void RtcpPacket::recycle() {
	RtcpPacket::pPool->recycle(this);
}

RtcpPacket *RtcpPacket::obtain(RtcpType rct, uint fromIp, ushort fromPort,
		bool isFeedback, bool needFeedback, uint ssrc, uint sequence,
		uint milliSeconds) {
	RtcpPacket *rcp = RtcpPacket::pPool->obtain();
	memset(rcp->pBuffer, 0, RTCP_PACKET_SIZE);
	rcp->mBytesLength = sizeof(RtcpHeader);
	rcp->pHeader->mFromIp = fromIp;
	rcp->pHeader->mFromPort = fromPort;
	rcp->pHeader->mType = rct;
	rcp->pHeader->bIsFeedback = isFeedback ? 1 : 0;
	rcp->pHeader->mNeedFeedback = needFeedback ? 1 : 0;
	rcp->pHeader->mSsrc = ssrc;
	rcp->pHeader->mSequence = sequence;
	rcp->pHeader->mMilliSeconds =
			milliSeconds == 0 ?
					XUtils::currentMilliSeconds(24 * 3600 * 1000) :
					milliSeconds;
	return rcp;
}

RtcpPacket *RtcpPacket::setFromPort(ushort fromPort) {
	this->pHeader->mFromPort = fromPort;
	return this;
}

const uchar *RtcpPacket::getBytes() const {
	return this->pBuffer;
}

size_t RtcpPacket::getBytesLength() const {
	return this->mBytesLength;
}
