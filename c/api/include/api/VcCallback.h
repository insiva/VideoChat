/*
 * VcCallBack.h
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#ifndef INCLUDE_API_VCCALLBACK_H_
#define INCLUDE_API_VCCALLBACK_H_

#include<api/Config.h>
#include<api/VcCall.h>

class VcCallback {
public:
	virtual void onIncoming(VcCall *){};
	//virtual void onHangup(VcCall *) = 0;
	//virtual void onReject(VcCall *) = 0;
	//virtual void onAnswer(VcCall *) = 0;
	virtual void onConfirmed(VcCall *){};
	virtual void onEstablished(VcCall *){};
	virtual void onOutgoFail(VcCall *){};
	virtual void onTimeout(VcCall *){};
	virtual void onDisconnect(VcCall *){};
	virtual void onRemoteCameraParametersRecved(int width,int height,int fps){};
};

#endif /* INCLUDE_API_VCCALLBACK_H_ */
