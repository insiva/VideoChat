/*
 * CallManager.h
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#ifndef INCLUDE_API_CALLMANAGER_H_
#define INCLUDE_API_CALLMANAGER_H_

#include<api/Config.h>
#include<api/VcFriend.h>
#include<api/VcCall.h>
#include<api/VcCallback.h>
#include<sim_rtp/RtpManager.h>
#include<api/VcManager.h>

class VcManager;

class CallManager {
	friend class VcManager;
public:
	CallManager(RtpManager *rtpManager);
	~CallManager();
	int makeCall(VcFriend * vf, int width, int height, int fps);
private:
	//void setCallback(VcCallback *);
	VcCall *pCurrentCall;
	RtpManager *pRtpManager;
	void releaseCall();
	void onRtcpFeedback(RtcpPacket *rcp);
	void onCallInvite(RtcpPacket * rcp);
	void onCallAnswer(RtcpPacket * rcp);
	void onCallHangup(RtcpPacket * rcp);
	void onCallReject(RtcpPacket * rcp);

	void answerCall();
	void rejectCall();
	void hangupCall();

	FuncHandler<FuncOnDataPacketRecved> *pDataPacketRecvedFuncHandler;
	FuncHandler<FuncOnRtcpRecved> *pRtcpRecvedFuncHandler;
	static void onDataPacketRecved(DataPacket *, void *);
	static void onRtcpPacketRecved(RtcpPacket *, void *);
};

#endif /* INCLUDE_API_CALLMANAGER_H_ */
