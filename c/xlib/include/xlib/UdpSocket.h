/*
 * UdpSocket.h
 *
 *  Created on: 2016年6月30日
 *      Author: zzh
 */

#ifndef XLIB_INCLUDE_SIM_RTP_UDPSOCKET_H_
#define XLIB_INCLUDE_SIM_RTP_UDPSOCKET_H_

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <xlib/Config.h>

class UdpSocket {
public:
	UdpSocket(const char* = XNULL,ushort=0,bool = false);
	~UdpSocket();
	void set(const char *ip,ushort port,bool broadcast);
	void set(uint ip,ushort port,bool broadcast);
	int send(const uchar*, size_t);
	void closeSocket();
private:
	struct sockaddr_in *mDestAddr;
	int mSocketFd;
	uint iIp;
	ushort iPort;
};

#endif /* SIM_RTP_INCLUDE_SIM_RTP_UDPSOCKET_H_ */
