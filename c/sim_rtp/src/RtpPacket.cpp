/*
 * RtpPacket.cpp
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#include <sim_rtp/RtpPacket.h>
#include <utils/XUtils.h>
#include<math.h>

XPool<RtpPacket> *RtpPacket::pPool = new XPool<RtpPacket>();

RtpPacket *RtpPacket::obtain(uint ssrc, RtpType rt, uint milliSeconds,
		uint sequence) {
	RtpPacket *rp = RtpPacket::pPool->obtain();

	memset(rp->pBuffer, 0, RTP_PACKET_SIZE);
	rp->pHeader->mSsrc = ssrc;
	rp->pHeader->mType = rt;
	rp->pHeader->mMilliSeconds = milliSeconds;
	rp->pHeader->mSequence = sequence;
	return rp;
}

RtpPacket::RtpPacket(uint ssrc, RtpType rt, uint milliSeconds, uint sequence) :
		mBytesLength(0){
	this->pBuffer = new uchar[RTP_PACKET_SIZE];

	memset(this->pBuffer, 0, RTP_PACKET_SIZE);
	this->pHeader = (RtpHeader *) this->pBuffer;
	this->pData = this->pBuffer + sizeof(RtpHeader);
	this->pHeader->mSsrc = ssrc;
	this->pHeader->mType = rt;
	this->pHeader->mMilliSeconds = milliSeconds;
	this->pHeader->mSequence = sequence;
}

RtpPacket::~RtpPacket() {
	delete[] this->pBuffer;
	DLOG("RtcpPacket Delete!\n");
}

void RtpPacket::recycle() {
	RtpPacket::pPool->recycle(this);
}

uint RtpPacket::getSsrc() const {
	return this->pHeader->mSsrc;
}

RtpType RtpPacket::getType() const {
	return (RtpType)this->pHeader->mType;
}

uint RtpPacket::getMilliSeconds() const {
	return this->pHeader->mMilliSeconds;
}

uint RtpPacket::getSequence() const {
	return this->pHeader->mSequence;
}

uchar RtpPacket::getSubCount() const {
	return this->pHeader->mSubCount;
}

uchar RtpPacket::getSubSequence() const {
	return this->pHeader->mSubSequence;
}

uint RtpPacket::getTotalLength() const {
	return this->pHeader->mTotalLength;
}

ushort RtpPacket::getLength() const {
	return this->pHeader->mLength;
}

uint RtpPacket::getOffset() const {
	return this->pHeader->mOffset;
}

RtpPacket *RtpPacket::setSsrc(uint ssrc) {
	this->pHeader->mSsrc = ssrc;
	return this;
}

RtpPacket *RtpPacket::setType(RtpType rt) {
	this->pHeader->mType = rt;
	return this;
}

RtpPacket *RtpPacket::setMilliSeconds(uint milliSeconds) {
	this->pHeader->mMilliSeconds = milliSeconds;
	return this;
}

RtpPacket *RtpPacket::setSubCount(uchar subCount) {
	this->pHeader->mSubCount = subCount;
	return this;
}

RtpPacket *RtpPacket::setSubSequence(uchar subSequence) {
	this->pHeader->mSubSequence = subSequence;
	return this;
}

RtpPacket *RtpPacket::setTotalLength(uint totalLength) {
	this->pHeader->mTotalLength = totalLength;
	return this;
}

RtpPacket *RtpPacket::setLength(ushort length) {
	this->pHeader->mLength = length;
	return this;
}

RtpPacket *RtpPacket::setOffset(uint offset) {
	this->pHeader->mOffset = offset;
	return this;
}

const uchar *RtpPacket::getData() const {
	return this->pData;
}

size_t RtpPacket::getBytesLength() const {
	return this->mBytesLength;
}

const uchar *RtpPacket::getBytes() const {
	return this->pBuffer;
}

size_t RtpPacket::setData(const uchar *buffer, size_t offset, size_t length) {
	//XASSERT((length+sizeof(RtpHeader))<=RTP_PACKET_SIZE,"Data Size Over PACKET_SIZE:%d!\n", RTP_PACKET_SIZE);
	memcpy(this->pData,buffer+offset,length);
	this->mBytesLength = sizeof(RtpHeader) + length;
	return offset + length;
}

int RtpPacket::parse(const char *buffer, size_t len,RtpPacket * rp) {
	if (len > RTP_PACKET_SIZE) {
		return RESULT_ERROR;
	}
	memcpy(rp->pBuffer, buffer, len);
	return RESULT_OK;
}

