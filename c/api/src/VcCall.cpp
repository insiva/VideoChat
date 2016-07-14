/*
 * VcCall.cpp
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#include<api/VcCall.h>

int VcCall::ID = 0;

VcCall::VcCall(VcFriend * vFriend, VcCallState state, bool isOutgo) :
		bOutgo(isOutgo), mId(++VcCall::ID), pFriend(vFriend), mStartTime(0), mRingTime(
				0), mAnswerTime(0), mHangupTime(0), mState(state) {

}

VcCall::~VcCall() {

}

int VcCall::getId() const {
	return this->mId;
}

VcFriend *VcCall::getFriend() const {
	return this->pFriend;
}

bool VcCall::isOutgo() const {
	return this->bOutgo;
}

VcCallState VcCall::getState() const {
	return this->mState;
}
