/*
 * VcCall.h
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#ifndef INCLUDE_API_VCCALL_H_
#define INCLUDE_API_VCCALL_H_

#include<api/Config.h>
#include<api/VcFriend.h>

enum VcCallState {
	UNKNOW = 0, ESTABLISHING = 1, ESTABLISHED = 2, CONFIRMED = 3, DISCONNECT = 4
};

class VcCall {
	friend class VcManager;
	friend class CallManager;
public:
	VcCall(VcFriend *, VcCallState state = VcCallState::UNKNOW, bool isOutgo =
			true);
	~VcCall();

	int getId() const;
	VcFriend *getFriend() const;
	VcCallState getState() const;
	bool isOutgo() const;
private:
	bool bOutgo;
	VcCallState mState;
	int mId;
	static int ID;
	VcFriend *pFriend;
	uint mStartTime, mRingTime, mAnswerTime, mHangupTime;
};

#endif /* INCLUDE_API_VCCALL_H_ */
