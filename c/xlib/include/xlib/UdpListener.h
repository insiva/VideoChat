/*
 * UdpListener.h
 *
 *  Created on: 2016年6月30日
 *      Author: zzh
 */

#ifndef XLIB_INCLUDE_SIM_RTP_UDPLISTENER_H_
#define XLIB_INCLUDE_SIM_RTP_UDPLISTENER_H_

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <xlib/Config.h>
#include <xlib/FuncHandler.h>
#define BUFFER_SIZE UDP_MTU_SIZE
typedef void (FuncOnDataRecved)(const char*,size_t,struct sockaddr *,void *ptr);

class UdpListener{

public:
	UdpListener(const char*,ushort,size_t = BUFFER_SIZE);
	UdpListener(ushort,size_t = BUFFER_SIZE);
	~UdpListener();
	int listen();
	void setDataRecvedFunc(FuncHandler<FuncOnDataRecved> *);
private:
	struct sockaddr_in *pLocalAddr;
	int mSocketFd;
	size_t mBufferSize;
	//FuncOnDataRecved *mDataRecvedFunc;
	//void *mInvoker;
	FuncHandler<FuncOnDataRecved> *pDataRecvedFuncHandler;
	char *mBuffer;
	bool mEnabled;
	void free();
	bool mRecving;
	void closeSocket();
};



#endif /* SIM_RTP_INCLUDE_SIM_RTP_UDPLISTENER_H_ */
