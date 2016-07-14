/*
 * RtpRecvManager.h
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#ifndef INCLUDE_SIM_RTP_RTPRECVMANAGER_H_
#define INCLUDE_SIM_RTP_RTPRECVMANAGER_H_

#include <sim_rtp/Config.h>
#include <sim_rtp/RtpPacket.h>
#include <sim_rtp/RtcpPacket.h>
#include <xlib/UdpListener.h>
#include <xlib/UdpSocket.h>
#include <xlib/XThread.h>
#include<sim_rtp/RtpManager.h>

class RtpManager;

class RtpRecvManager {
	friend class RtpManager;
private:
	RtpRecvManager(ushort, RtpManager *);
	~RtpRecvManager();
	ushort mRtpPort, mRtcpPort;
	UdpListener *pRtpListener, *pRtcpListener;
	UdpSocket *pRtcpFeedbackSocket;
	int listenRtpAsync();
	int listenRtcpAsync();
	//FuncOnRtpRecved *mOnRtpRecvedFunc=0;
	//FuncOnRtcpRecved *mOnRtcpRecvedFunc=0;
	XThread *pRtpThread;
	XThread *pRtcpThread;
	FuncHandler<FuncOnDataRecved> *pRtpRecvedFuncHandler;
	FuncHandler<FuncOnDataRecved> *pRtcpRecvedFuncHandler;
	static void onRtpDataRecved(const char *, size_t, struct sockaddr *,
			void *);
	static void onRtcpDataRecved(const char *, size_t, struct sockaddr *,
			void *);
	static void *listen(void *);

	RtpManager *pRtpManager;
};

#endif /* INCLUDE_SIM_RTP_RTPRECVMANAGER_H_ */
