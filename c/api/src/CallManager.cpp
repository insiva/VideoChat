/*
 * CallManager.cpp
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#include<api/CallManager.h>
#include<utils/XUtils.h>

CallManager::CallManager(RtpManager *rtpManager) :
		pCurrentCall(XNULL), pRtpManager(rtpManager) {
	//this->pRtpManager = RtpManager::create(localPort);
	int err = this->pRtpManager->listenAsync();
	XASSERT(err == 0, "Listen Failed!\n");
	this->pDataPacketRecvedFuncHandler =
			new FuncHandler<FuncOnDataPacketRecved>(this,
					&CallManager::onDataPacketRecved);
	this->pRtcpRecvedFuncHandler = new FuncHandler<FuncOnRtcpRecved>(this,
			&CallManager::onRtcpPacketRecved);
	this->pRtpManager->setOnDataPacketRecvedFunc(pDataPacketRecvedFuncHandler);
	this->pRtpManager->setOnRtcpRecvedFunc(pRtcpRecvedFuncHandler);
}

CallManager::~CallManager() {
	RtpManager::destroy(this->pRtpManager);
	delete pDataPacketRecvedFuncHandler;
	delete pRtcpRecvedFuncHandler;
}

int CallManager::makeCall(VcFriend * vf) {
	this->pRtpManager->connectRemote(vf->getIp(), vf->getPort());
	//const char * cc=cameraParameters;
	int err = this->pRtpManager->sendRtcp(RtcpType::CALL_INVITE, true, 0);
	if (err == 0) {
		DLOG("Make Call(%s:%d) Success!\n", vf->getIp(), vf->getPort());
		this->pCurrentCall = new VcCall(vf, VcCallState::ESTABLISHING);
	} else {
		VcManager::pInstance->sendEmptyMessage(WHAT_CALL_INVITE_FAIL);
		if (VcManager::pInstance->pCallback != XNULL) {
			VcManager::pInstance->pCallback->onOutgoFail(XNULL);
		}
	}
	return err;
}

void CallManager::releaseCall() {
	if (this->pCurrentCall != XNULL) {
		delete this->pCurrentCall;
	}
	this->pCurrentCall = XNULL;
	this->pRtpManager->disconnectRemote();

	VcManager::pInstance->onCallFinished();
}

void CallManager::onDataPacketRecved(DataPacket *dp, void *invoker) {
	//VcManager::pInstance->onDataPacketRecved(dp);
	CallManager *callManager = (CallManager *) invoker;
	if (callManager->pCurrentCall != XNULL
			&& callManager->pCurrentCall->getState()
					== VcCallState::CONFIRMED) {
		VcManager::pInstance->onDataPacketRecved(dp);
	}
}

void CallManager::onRtcpPacketRecved(RtcpPacket *rcp, void *invoker) {
	CallManager *callManager = (CallManager *) invoker;
	if (rcp->isFeedback()) {
		callManager->onRtcpFeedback(rcp);
	} else {
		switch (rcp->getType()) {
		case RtcpType::CALL_INVITE:
			callManager->onCallInvite(rcp);
			break;
		case RtcpType::CALL_ANSWER:
			callManager->onCallAnswer(rcp);
			break;
		case RtcpType::CALL_HANGUP:
			callManager->onCallHangup(rcp);
			break;
		case RtcpType::CALL_REJECT:
			callManager->onCallReject(rcp);
			break;
		case RtcpType::CALL_CAMERA_PARAMETERS:
			callManager->onCallCameraParameters(rcp);
			break;
		}
	}
}

void CallManager::onRtcpFeedback(RtcpPacket *rcp) {
	switch (rcp->getType()) {
	case RtcpType::CALL_INVITE:
		if (this->pCurrentCall != XNULL
				&& this->pCurrentCall->mState == VcCallState::ESTABLISHING) {
			this->pCurrentCall->mState = VcCallState::ESTABLISHED;
			if (VcManager::pInstance->pCallback != XNULL) {
				VcManager::pInstance->pCallback->onEstablished(
						this->pCurrentCall);
			}
		}
		break;
	case RtcpType::CALL_ANSWER:
		if (this->pCurrentCall != XNULL
				&& this->pCurrentCall->mState < VcCallState::CONFIRMED) {
			this->pCurrentCall->mState = VcCallState::CONFIRMED;
			if (VcManager::pInstance->pCallback != XNULL) {
				VcManager::pInstance->pCallback->onConfirmed(
						this->pCurrentCall);
			}
		}
		break;
	}
}

void CallManager::onCallInvite(RtcpPacket *rcp) {
	if (this->pCurrentCall != XNULL) {
		this->pRtpManager->sendRtcp(RtcpType::CALL_REJECT, false);
	} else {
		VcFriend *vf = VcManager::pInstance->pFriendManager->findFriend(
				rcp->getSsrc());
		if (vf == XNULL) {
			this->pRtpManager->sendRtcp(RtcpType::CALL_REJECT, false);
		} else {
			this->pCurrentCall = new VcCall(vf, VcCallState::ESTABLISHED,
					false);
			if (VcManager::pInstance->pCallback != XNULL) {
				VcManager::pInstance->pCallback->onIncoming(this->pCurrentCall);
			}
		}
	}
}

void CallManager::onCallAnswer(RtcpPacket *rcp) {
	/*
	 {
	 char *hex = new char[2 * rcp->getBytesLength() + 1];
	 XUtils::bytesToHexString(rcp->getBytes(), rcp->getBytesLength(), hex);
	 LOG("SEND(%d):%s\n", rcp->getBytesLength(), hex);
	 delete[] hex;
	 }
	 */
	if (this->pCurrentCall != XNULL
			&& this->pCurrentCall->mState < VcCallState::CONFIRMED) {
		this->pCurrentCall->mState = VcCallState::CONFIRMED;
		const int *cameraParameters = (const int *) rcp->getExtraData();
		VcManager::pInstance->setRemoteCameraParameters(cameraParameters[0],
				cameraParameters[1], cameraParameters[2]);
		if (VcManager::pInstance->pCallback != XNULL) {
			VcManager::pInstance->pCallback->onConfirmed(this->pCurrentCall);
		}
	}
}

void CallManager::onCallHangup(RtcpPacket *rcp) {
	if (this->pCurrentCall != XNULL
			&& this->pCurrentCall->mState < VcCallState::DISCONNECT) {
		this->pCurrentCall->mState = VcCallState::DISCONNECT;
		if (VcManager::pInstance->pCallback != XNULL) {
			VcManager::pInstance->pCallback->onDisconnect(this->pCurrentCall);
		}
		this->releaseCall();
	}
}

void CallManager::onCallReject(RtcpPacket *rcp) {
	if (this->pCurrentCall != XNULL
			&& this->pCurrentCall->mState < VcCallState::DISCONNECT) {
		this->pCurrentCall->mState = VcCallState::DISCONNECT;
		if (VcManager::pInstance->pCallback != XNULL) {
			VcManager::pInstance->pCallback->onDisconnect(this->pCurrentCall);
		}
		this->releaseCall();
	}
}

void CallManager::answerCall() {
	if (this->pCurrentCall != XNULL && (!this->pCurrentCall->isOutgo())
			&& this->pCurrentCall->mState < VcCallState::CONFIRMED) {
		this->pRtpManager->connectRemote(
				this->pCurrentCall->getFriend()->getIp(),
				this->pCurrentCall->getFriend()->getPort());

		int cameraParameters[3];
		cameraParameters[0] = VcManager::pInstance->getEncodeWidth();
		cameraParameters[1] = VcManager::pInstance->getEncodeHeight();
		cameraParameters[2] = VcManager::pInstance->getEncodeFps();

		this->pRtpManager->sendRtcp(RtcpType::CALL_ANSWER, true, 0,
				(const uchar *) cameraParameters, sizeof(cameraParameters));
	}
}

void CallManager::rejectCall() {
	if (this->pCurrentCall != XNULL
			&& this->pCurrentCall->mState < VcCallState::DISCONNECT) {
		int err = this->pRtpManager->sendRtcp(RtcpType::CALL_REJECT, false);
		if (err == 0) {
			this->pCurrentCall->mState = VcCallState::DISCONNECT;
			if (VcManager::pInstance->pCallback != XNULL) {
				VcManager::pInstance->pCallback->onDisconnect(
						this->pCurrentCall);
			}
			this->releaseCall();
		}
	}
}

void CallManager::hangupCall() {
	if (this->pCurrentCall != XNULL
			&& this->pCurrentCall->mState < VcCallState::DISCONNECT) {
		int err = this->pRtpManager->sendRtcp(RtcpType::CALL_HANGUP, false);
		if (err == 0) {
			this->pCurrentCall->mState = VcCallState::DISCONNECT;
			if (VcManager::pInstance->pCallback != XNULL) {
				VcManager::pInstance->pCallback->onDisconnect(
						this->pCurrentCall);
			}
			this->releaseCall();
		}
	}
}

int CallManager::sendMyCameraParameters(int width, int height, int fps) const {
	int parameters[3];
	parameters[0] = width;
	parameters[1] = height;
	parameters[2] = fps;
	int err = this->pRtpManager->sendRtcp(RtcpType::CALL_CAMERA_PARAMETERS,
			false, 0, (const unsigned char *) parameters, sizeof(parameters));
	return err;
}

void CallManager::onCallCameraParameters(RtcpPacket *rcp){
	const int *cameraParameters = (const int *) rcp->getExtraData();
	VcManager::pInstance->onRemoteCameraParametersRecved(cameraParameters[0],
			cameraParameters[1], cameraParameters[2]);
}
