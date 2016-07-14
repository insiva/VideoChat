/*
 * XMessage.h
 *
 *  Created on: 2016年7月5日
 *      Author: zzh
 */

#ifndef INCLUDE_XLIB_XMESSAGE_H_
#define INCLUDE_XLIB_XMESSAGE_H_

#include<xlib/Config.h>
#include<xlib/XPool.h>
#include<pthread.h>
#include<time.h>
#include <sys/time.h>

class XMessage {
	friend class XMessageQueue;
	friend class XThread;
public:
	XMessage(int = 0, void * = XNULL);
	~XMessage();
	void destroy();
	static XMessage * obtain(int = 0, void * = XNULL);
	void set(int, void *);
	void *getData() const;
	int getWhat() const;
private:
	static XPool<XMessage> *mPool;
	int mWhat;
	void *mData;
	XMessage *mPrev, *mNext;
	struct timespec mTimeSpec;
	//struct time_val mTimeVal;
};

class XMessageQueue {
public:
	XMessageQueue();
	~XMessageQueue();
	void enqueue(XMessage *);
	XMessage *dequeue();
	uint size();
private:
	uint mSize;
	XMessage *mHead, *mTail;
	pthread_cond_t mReady;// = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t mLock;// = PTHREAD_MUTEX_INITIALIZER;
	XMessage *dequeueHead(struct timespec **);
	static int compareTimeSpec(const struct timespec *,const struct timespec *);
};

#endif /* INCLUDE_XLIB_XMESSAGE_H_ */
