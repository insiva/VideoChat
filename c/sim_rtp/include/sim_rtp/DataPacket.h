/*
 * DataPacket.h
 *
 *  Created on: 2016年7月8日
 *      Author: zzh
 */

#ifndef INCLUDE_SIM_RTP_DATAPACKET_H_
#define INCLUDE_SIM_RTP_DATAPACKET_H_

#include<sim_rtp/Config.h>
#include<xlib/XPool.h>
#include<sim_rtp/RtpManager.h>
#include<sim_rtp/RtpPacket.h>

#define DP_CODE_PACKET_SEQUENCE_UNEQUAL -2
#define DP_CODE_DEFAULT 0
#define DP_CODE_PACKET_COMPLETE 1
class DataPacket {
	friend class RtpManager;
public:
	DataPacket(RtpType = RtpType::RTP_NONE, size_t = 0);
	~DataPacket();
	uchar *allocBuffer(size_t);
	uchar *getBuffer() const;
	size_t getLength() const;
private:
	RtpType mType;
	uchar *pBuffer;
	size_t mLength;
	uint mSequence;
	ushort mSubCount;
	int addRtpPacket(RtpPacket *);
public:
	static DataPacket *obtain(RtpType = RtpType::RTP_NONE, size_t = 0,
			uint sequence = 0, ushort subCount = 0);
	void recycle();
private:
	static XPool<DataPacket> *pPool;
};

#endif /* INCLUDE_SIM_RTP_DATAPACKET_H_ */
