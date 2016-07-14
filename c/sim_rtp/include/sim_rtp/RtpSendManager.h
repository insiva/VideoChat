/*
 * RtpSendManager.h
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#ifndef INCLUDE_SIM_RTP_RTPSENDMANAGER_H_
#define INCLUDE_SIM_RTP_RTPSENDMANAGER_H_

#include <sim_rtp/Config.h>
#include <xlib/UdpSocket.h>
#include <sim_rtp/RtpManager.h>

class RtpManager;

class RtpSendManager{
	friend class RtpManager;
private:
	RtpSendManager(RtpManager *);
	~RtpSendManager();
	RtpSendManager * setRemoteIPAndPort(const char *,ushort);
	int send(RtpPacket *) const;
	int send(RtcpPacket *) const;
	char *pRemoteIp;
	ushort mRtpPort,mRtcpPort;
	UdpSocket *pRtpSocket,*pRtcpSocket;
	RtpManager *pRtpManager;
	void reset();
};

#endif /* INCLUDE_SIM_RTP_RTPSENDMANAGER_H_ */
