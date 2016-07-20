/*
 * XManager.h
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#ifndef INCLUDE_API_VCMANAGER_H_
#define INCLUDE_API_VCMANAGER_H_

#include <api/Config.h>
#include<api/VcCallback.h>
#include<api/VcCall.h>
#include<api/VcFriend.h>
#include<api/CallManager.h>
#include<api/FriendManager.h>
#include<xlib/XThread.h>
#include<api/VideoManager.h>

class CallManager;

enum VcCallAction {
	CALL_ACTION_ANSWER = 1, CALL_ACTION_REJECT, CALL_ACTION_HANGUP
};

class VcManager: public XThread {
	friend class CallManager;
private:
	VcManager(const char * localIp, unsigned short port, unsigned int ssrc = 0);
	~VcManager();

public:
	int makeCall(unsigned int friendSsrc);
	void addFriend(unsigned int ssrc, const char * ip, unsigned short port);
	void clearFriend();
	VcCall *getCurrentCall() const;
	void setCallback(VcCallback *);
	void handleCall(VcCall *call, VcCallAction action);
	int pushYv12Frame(char *buffer, size_t length);
	void setMyCameraParameters(int height, int width, int fps);
#ifdef __ANDROID__
	void initGl(int viewWidth,int viewHeight,const char *buildModel);
	void deinitGl();
	void render();
#endif
private:
	RtpManager *pRtpManager;

	CallManager *pCallManager;
	FriendManager *pFriendManager;
	VideoManager *pVideoManager;
	VcCallback *pCallback;
	void handleMessage(XMessage *);
	void onDataPacketRecved(DataPacket *);
	void onCallFinished();
	void onRemoteCameraParametersRecved(int width, int height, int fps);
	void setRemoteCameraParameters(int width, int height, int fps);
	//void onRtcpPacketRecved(RtcpPacket *);

	void asyncCallTimeout();
	void asyncHandleCall(XMessage *msg);
	int getEncodeWidth() const;
	int getEncodeHeight() const;
	int getEncodeFps() const;
public:
	static VcManager *init(const char *localIp, unsigned short port,
			unsigned int ssrc = 0);
	static void deinit();
	static VcManager* get();
private:
	static VcManager* pInstance;
};

#endif /* INCLUDE_API_VCMANAGER_H_ */
