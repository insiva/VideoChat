/*
 * DataPacket.cpp
 *
 *  Created on: 2016年7月8日
 *      Author: zzh
 */

#include<sim_rtp/DataPacket.h>

XPool<DataPacket> *DataPacket::pPool = new XPool<DataPacket>();

DataPacket::DataPacket(RtpType rt, size_t len) :
		mType(rt), pBuffer(XNULL), mLength(0), mSequence(0), mSubCount(0) {
	this->allocBuffer(len);
}

DataPacket::~DataPacket() {
	if (this->pBuffer != XNULL) {
		delete this->pBuffer;
	}
}

uchar *DataPacket::allocBuffer(size_t len) {
	if (len > 0) {
		if (len <= this->mLength) {
			memset(this->pBuffer, 0, this->mLength);
		} else {
			if (this->pBuffer != XNULL) {
				delete this->pBuffer;
				this->pBuffer = XNULL;
			}
			this->pBuffer = new uchar[len];
			memset(this->pBuffer, 0, len);
			this->mLength = len;
		}
	} else {
		if (this->pBuffer != XNULL) {
			delete this->pBuffer;
			this->pBuffer = XNULL;
			this->mLength = 0;
		}
	}
	this->mLength = len;
	return this->pBuffer;
}

DataPacket *DataPacket::obtain(RtpType rt, size_t len, uint sequence,
		ushort subCount) {
	DataPacket *dp = DataPacket::pPool->obtain();
	dp->mType = rt;
	dp->allocBuffer(len);
	dp->mSequence = sequence;
	dp->mSubCount = subCount;
	return dp;
}

uchar *DataPacket::getBuffer() const {
	return this->pBuffer;
}

size_t DataPacket::getLength() const {
	return this->mLength;
}

int DataPacket::addRtpPacket(RtpPacket * rp) {
	int err = DP_CODE_DEFAULT;
	if (this->mSequence == 0) {
		this->mSequence = rp->getSequence();
		this->mSubCount = rp->getSubCount();
		this->allocBuffer(rp->getTotalLength());
	}
	if (this->mSequence == rp->getSequence()) {
		this->mSubCount--;
		memcpy(this->pBuffer + rp->getOffset(), rp->getData(), rp->getLength());
		if (this->mSubCount == 0) {
			err = DP_CODE_PACKET_COMPLETE;
		}
	} else {
		err = DP_CODE_PACKET_SEQUENCE_UNEQUAL;
	}
	return err;
}

void DataPacket::recycle() {
	DataPacket::pPool->recycle(this);
}
