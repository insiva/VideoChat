/*
 * VcFriend.h
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#ifndef INCLUDE_API_VCFRIEND_H_
#define INCLUDE_API_VCFRIEND_H_

#include<api/Config.h>

class VcFriend {
public:
	VcFriend(unsigned int ssrc, const char *ip, unsigned short port);
	~VcFriend();
	int getSsrc() const;
	const char *getIp() const;
	unsigned short getPort() const;
	VcFriend *setIp(const char *ip);
	VcFriend *setPort(unsigned short port);
private:
	unsigned int mSsrc;
	char * pIp;
	unsigned short mPort;
};

#endif /* INCLUDE_API_VCFRIEND_H_ */
