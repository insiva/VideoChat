/*
 * RtpManager.h
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#ifndef INCLUDE_SIM_RTP_RTPMANAGER_H_
#define INCLUDE_SIM_RTP_RTPMANAGER_H_

#include <sim_rtp/Config.h>
#include <sim_rtp/RtpRecvManager.h>
#include <sim_rtp/RtpSendManager.h>
#include <sim_rtp/DataPacket.h>

#define SUB_PACKET_MAX_COUNT 128

class RtpSendManager;
class RtpRecvManager;
class DataPacket;

typedef void (FuncOnRtpRecved)(RtpPacket *, void *);
typedef void (FuncOnRtcpRecved)(RtcpPacket *, void *);

typedef void (FuncOnDataPacketRecved)(DataPacket *, void *);

class RtpManager {
	friend class RtpSendManager;
	friend class RtpRecvManager;
public:
	static RtpManager* create(const char *localIp, ushort port, uint ssrc = 0);
	static void destroy(RtpManager*);
	//void setRemoteIp(const char*);
	void connectRemote(const char *, ushort);
	int listenAsync();
	int sendRtp(RtpType, const uchar *, size_t);
	int sendRtcp(RtcpType rct, bool needFeedback, uint milliSeconds = 0,
			const uchar *data = XNULL, size_t length = 0);
	void disconnectRemote();
	void setOnDataPacketRecvedFunc(FuncHandler<FuncOnDataPacketRecved> *);
	void setOnRtcpRecvedFunc(FuncHandler<FuncOnRtcpRecved> *);
	uint getSsrc() const;
private:
	RtpManager(const char *localIp, ushort localPort, uint = 0);
	~RtpManager();
	RtpRecvManager* pRecvManager;
	RtpSendManager* pSendManager;
	int mLastRtpSeq;
	void onRtpPacketRecved(RtpPacket*);
	void onRtcpPacketRecved(RtcpPacket*);
	FuncHandler<FuncOnDataPacketRecved> *pDataPacketRecvedFuncHandler;
	FuncHandler<FuncOnRtcpRecved> *pRtcpRecvedFuncHandler;
	inline void invokeDataPacketRecvedFunc();
	DataPacket *pDataPacket;
	uint mSsrc, mRtpSequence, mRtcpSequence;
	uint mLocalIp;
	ushort mLocalRtpPort, mLocalRtcpPort;

	static size_t iAvalableSize;
};

#endif /* INCLUDE_SIM_RTP_RTPMANAGER_H_ */
