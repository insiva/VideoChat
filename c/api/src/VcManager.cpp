/*
 * VcManager.cpp
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#include<api/VcManager.h>

using namespace std;

VcManager *VcManager::pInstance = XNULL;

VcManager::VcManager(const char *localIp, ushort localPort, uint ssrc) :
		XThread(true), pCallback(XNULL) {
	this->pRtpManager = RtpManager::create(localIp, localPort, ssrc); // new RtpManager(localPort);
	this->pFriendManager = new FriendManager();
	this->pCallManager = new CallManager(this->pRtpManager);
	this->pVideoManager = new VideoManager(this->pRtpManager);
}

VcManager::~VcManager() {
	delete this->pCallManager;
}

VcManager *VcManager::init(const char *localIp, ushort localPort, uint ssrc) {
	VcManager::pInstance = new VcManager(localIp, localPort, ssrc);
	VcManager::pInstance->start();
	return VcManager::pInstance;
}

void VcManager::deinit() {
	VcManager::pInstance->stop(false);
	delete VcManager::pInstance;
	VcManager::pInstance = XNULL;
}

VcManager *VcManager::get() {
	return VcManager::pInstance;
}

void VcManager::addFriend(uint ssrc, const char *ip, ushort port) {
	this->pFriendManager->addFriend(ssrc, ip, port);
}

VcCall *VcManager::getCurrentCall() const {
	return this->pCallManager->pCurrentCall;
}

void VcManager::setCallback(VcCallback *callback) {
	this->pCallback = callback;
}

int VcManager::makeCall(uint friendSsrc, int width, int height, int fps) {
	VcFriend *vf = this->pFriendManager->findFriend(friendSsrc);
	if (vf == XNULL) {
		LOG("No friend Ssrc=%d\n", friendSsrc);
		return ERROR_NO_FRIEND_FOUND;
	}
	int err = this->pCallManager->makeCall(vf, width, height, fps);
	if (err >= 0) {
		this->setMyCameraParameters(width, height, fps);
	}
	return err;
}

void VcManager::handleCall(VcCall *call, VcCallAction action) {
	if (this->pCallManager->pCurrentCall == XNULL
			|| this->pCallManager->pCurrentCall->mId != call->mId) {
		return;
	}
	int *data = new int();
	*data = action;
	XMessage *msg = XMessage::obtain(WHAT_CALL_ACTION, data);
	this->sendMessage(msg);
}

void VcManager::handleMessage(XMessage *msg) {
	switch (msg->getWhat()) {
	case WHAT_CALL_NO_RESPONSE_TIMEOUT:
		this->asyncCallTimeout();
		break;
	case WHAT_CALL_ACTION:
		this->asyncHandleCall(msg);
		break;
	}
}

void VcManager::asyncCallTimeout() {
	if (this->pCallManager->pCurrentCall == XNULL) {
		if (this->pCallback != XNULL) {
			this->pCallback->onOutgoFail(XNULL);
		}
	} else {
		if (this->pCallback != XNULL) {
			this->pCallback->onTimeout(this->pCallManager->pCurrentCall);
		}
		this->pCallManager->releaseCall();
	}
	this->onCallFinished();
}

void VcManager::asyncHandleCall(XMessage *msg) {
	int *action = (int *) msg->getData();
	switch ((VcCallAction) *action) {
	case VcCallAction::CALL_ACTION_ANSWER:
		this->pCallManager->answerCall();
		break;
	case VcCallAction::CALL_ACTION_REJECT:
		this->pCallManager->rejectCall();
		break;
	case VcCallAction::CALL_ACTION_HANGUP:
		this->pCallManager->hangupCall();
		break;
	}
	delete action;
}

void VcManager::onCallFinished() {
	this->pVideoManager->deinitEncoder();
	this->pVideoManager->deinitDecoder();
}
void VcManager::onCallIncoming(int width, int height, int fps) {
	this->setRemoteCameraParameters(width, height, fps);
	DLOG(
			"Call Incoming, Camera Parameters: Width = %d, Height =%d, FPS = %d.\n",
			width, height, fps);
}

void VcManager::pushYv12Frame(char *buffer, size_t length) {
	if (this->pCallManager->pCurrentCall != XNULL
			&& this->pCallManager->pCurrentCall->mState
					== VcCallState::CONFIRMED) {
		this->pVideoManager->onYv12FramePushed((uchar *) buffer);
	}
}

void VcManager::onDataPacketRecved(DataPacket *dp) {
	this->pVideoManager->onH264FrameRecved((uchar *) dp->getBuffer(),
			dp->getLength());
}

void VcManager::setMyCameraParameters(int width, int height, int fps) {
	this->pVideoManager->initEncoder(width, height, fps);
}

void VcManager::setRemoteCameraParameters(int width, int height, int fps) {
	this->pVideoManager->initDecoder(width, height, fps);
	//this->pVideoManager->setGlVideoSize(width,height);
}

int VcManager::getEncodeWidth() const {
	return this->pVideoManager->nEncodeWidth;
}

int VcManager::getEncodeHeight() const {
	return this->pVideoManager->nEncodeHeight;
}

int VcManager::getEncodeFps() const {
	return this->pVideoManager->nEncodeFps;
}

#ifdef __ANDROID__
void VcManager::initGl(int viewWidth, int viewHeight) {
	this->pVideoManager->initGlHepler(viewWidth, viewHeight);
}

void VcManager::deinitGl() {
	this->pVideoManager->deinitGlHelper();
}

void VcManager::render() {
	this->pVideoManager->render();
}
#endif
