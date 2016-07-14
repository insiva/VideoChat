/*
 * FuncHandler.h
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#ifndef INCLUDE_XLIB_FUNCHANDLER_H_
#define INCLUDE_XLIB_FUNCHANDLER_H_

#include<xlib/Config.h>

template<typename F> class FuncHandler {
public:
	void *pInvoker;
	F *pFunc;
	FuncHandler(void *invoker, F * func) {
		this->pInvoker = invoker;
		this->pFunc = func;
	}
	~FuncHandler() {

	}
};

#endif /* INCLUDE_XLIB_FUNCHANDLER_H_ */
