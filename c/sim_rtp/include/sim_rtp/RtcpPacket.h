/*
 * RtcpPacket.h
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#ifndef INCLUDE_SIM_RTP_RTCPPACKET_H_
#define INCLUDE_SIM_RTP_RTCPPACKET_H_

#include <sim_rtp/Config.h>
#include <string.h>
#include<xlib/XPool.h>

#define RTCP_PACKET_SIZE UDP_MTU_SIZE

typedef struct _RtcpHeader {
	uint mFromIp;
	ushort mFromPort;
	unsigned mType :6;
	unsigned bIsFeedback :1;
	unsigned mNeedFeedback :1;
	uint mSsrc;
	uint mSequence;
	uint mMilliSeconds;
	uchar mExtraDataLength;
} RtcpHeader;

class RtcpPacket {
public:
	RtcpPacket(RtcpType = RTCP_NONE, uint fromIp = 0, ushort fromPort = 0,
			bool isFeedback = false, bool needFeedback = false, uint = 0, uint =
					0, uint = 0);
	~RtcpPacket();

	RtcpType getType() const;
	bool isFeedback() const ;
	bool needFeedback() const;
	uint getSequence() const;
	uint getMilliSeconds() const;
	size_t getExtraDataLength() const;
	const uchar * getExtraData() const;
	uint getFromIp() const;
	ushort getFromPort() const;
	uint getSsrc() const;

	RtcpPacket *setFromIp(uint fromIp);
	RtcpPacket *setFromPort(ushort fromPort);
	RtcpPacket *setType(RtcpType);
	RtcpPacket *setNeedFeedback(bool);
	RtcpPacket *setSequence(uint);
	RtcpPacket *setMilliSeconds(uint);
	RtcpPacket *setExtraData(const uchar *, size_t, size_t = 0);
	RtcpPacket *setSsrc(uint ssrc);
	//RtcpPacket *setFromAddr(struct sockaddr * fromAddr);
	const uchar *getBytes() const;
	size_t getBytesLength() const;
	void recycle();

private:
	uchar* pBuffer;
	size_t mBytesLength;
	RtcpHeader *pHeader;
	//struct socketaddr *pFromAddr;
public:
	static RtcpPacket *obtain(RtcpType = RTCP_NONE, uint fromIp = 0,
			ushort fromPort = 0,bool isFeedback=false, bool needFeedback= false, uint = 0, uint = 0, uint = 0);
	static int parse(const char *, size_t, RtcpPacket *);
private:
	static XPool<RtcpPacket> *pPool;
};

#endif /* INCLUDE_SIM_RTP_RTCPPACKET_H_ */
