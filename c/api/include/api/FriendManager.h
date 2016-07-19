/*
 * FriendManager.h
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#ifndef INCLUDE_API_FRIENDMANAGER_H_
#define INCLUDE_API_FRIENDMANAGER_H_

#include<api/Config.h>
#include<api/VcFriend.h>
#include<vector>

using namespace std;
class FriendManager {
	friend class VcManager;
public:
	FriendManager();
	~FriendManager();
	void addFriend(uint ssrc, const char *ip, ushort port);
	VcFriend *findFriend(uint ssrc) const;
private:
	vector<VcFriend *> *pFriends;
};

#endif /* INCLUDE_API_FRIENDMANAGER_H_ */
