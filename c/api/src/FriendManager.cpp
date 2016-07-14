/*
 * FriendManager.cpp
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#include<api/FriendManager.h>

FriendManager::FriendManager() {
	this->pFriends = new vector<VcFriend *>();
}

FriendManager::~FriendManager() {
	delete this->pFriends;
}

void FriendManager::addFriend(uint ssrc, const char *ip, ushort port) {
	VcFriend * vf = new VcFriend(ssrc, ip, port);
	this->pFriends->push_back(vf);
}

VcFriend *FriendManager::findFriend(uint ssrc) const {
	for (vector<VcFriend *>::iterator it = pFriends->begin();
			it != pFriends->end(); it++) {
		if ((*it)->getSsrc() == ssrc) {
			return *it;
		}
	}
	return XNULL;
}
