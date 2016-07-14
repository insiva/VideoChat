/*
 * RtpPacket.h
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#ifndef INCLUDE_SIM_RTP_RTPPACKET_H_
#define INCLUDE_SIM_RTP_RTPPACKET_H_

#include <sim_rtp/Config.h>
#include <string.h>
#include<xlib/XPool.h>

#define RTP_PACKET_SIZE UDP_MTU_SIZE

typedef struct _RtpHeader { //24字节

	uint mSsrc; //4byte
	uchar mType; //包类型
	uint mMilliSeconds; //精确到毫秒,4byte
	uint mSequence; //4byte
	uchar mSubCount; //子包数量
	uchar mSubSequence; //子包序号
	uint mTotalLength; //总长度
	ushort mLength; //2byte,本包长度
	uint mOffset; //偏移
} RtpHeader;

#define RTP_DATA_MAX_SIZE (RTP_PACKET_SIZE-sizeof(RtpHeader))

class RtpPacket {
public:
	RtpPacket(uint = 0, RtpType = RtpType::RTP_NONE, uint = 0, uint = 0);
	~RtpPacket();
	uint getSsrc() const;
	RtpType getType() const;
	uint getMilliSeconds() const;
	uint getSequence() const;
	uchar getSubCount() const;
	uchar getSubSequence() const;
	uint getTotalLength() const;
	ushort getLength() const;
	uint getOffset() const;

	RtpPacket *setSsrc(uint);
	RtpPacket *setType(RtpType);
	RtpPacket *setMilliSeconds(uint);
	RtpPacket *setSubCount(uchar);
	RtpPacket *setSubSequence(uchar);
	RtpPacket *setTotalLength(uint);
	RtpPacket *setLength(ushort);
	RtpPacket *setOffset(uint);

	const uchar *getData() const;
	size_t getBytesLength() const;
	const uchar * getBytes() const;
	void recycle();
	size_t setData(const uchar *, size_t offset, size_t);
	static int parse(const char *, size_t, RtpPacket *);
	//static RtpPacket **divide(const char *, size_t, int *,uint = 0, PacketType = PacketType::NONE, uint = 0, uint = 0);

private:
	RtpHeader *pHeader;
	uchar *pBuffer, *pData;
	size_t mBytesLength;

public:
	static RtpPacket *obtain(uint = 0, RtpType = RtpType::RTP_NONE, uint = 0,
			uint = 0);
private:
	static XPool<RtpPacket> *pPool;
};

#endif /* INCLUDE_SIM_RTP_RTPPACKET_H_ */
