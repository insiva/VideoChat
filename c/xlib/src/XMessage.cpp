/*
 * XMessage.cpp
 *
 *  Created on: 2016年7月5日
 *      Author: zzh
 */

#include<xlib/XMessage.h>

XPool<XMessage> * XMessage::mPool = new XPool<XMessage>();

XMessage::XMessage(int what, void *data) :
		mWhat(0), mData(data), mPrev(XNULL), mNext(XNULL) {
	this->mTimeSpec.tv_sec = 0;
	this->mTimeSpec.tv_nsec = 0;
}

XMessage::~XMessage() {
	DLOG("XMessage Delete!\n");
}

XMessage * XMessage::obtain(int what, void *data) {
	XMessage *msg = mPool->obtain();
	msg->set(what, data);
	msg->mTimeSpec.tv_sec = 0;
	msg->mTimeSpec.tv_nsec = 0;
	msg->mPrev = msg->mNext = XNULL;
	return msg;
}

void XMessage::destroy() {
	XMessage::mPool->recycle(this);
}

void XMessage::set(int what, void *data) {
	this->mWhat = what;
	this->mData = data;
}

XMessageQueue::XMessageQueue() :
		mHead(XNULL), mTail(XNULL), mSize(0) {
	pthread_mutex_init(&mLock, XNULL);
	pthread_cond_init(&mReady, XNULL);
}

XMessageQueue::~XMessageQueue() {
	pthread_cond_destroy(&mReady);
	pthread_mutex_destroy(&mLock);
}

uint XMessageQueue::size() {
	return this->mSize;
}

void XMessageQueue::enqueue(XMessage *msg) {
	//MsgNode *node=new MsgNode();
	//node->mValue=msg;
	bool sendSingal = false;
	pthread_mutex_lock(&mLock);
	if (this->mTail == XNULL) {
		this->mHead = this->mTail = msg;
		sendSingal = true;
	} else {
		XMessage *current = this->mTail;
		while (current != XNULL
				&& XMessageQueue::compareTimeSpec(&(msg->mTimeSpec),
						&(current->mTimeSpec)) < 0) {
			current = current->mPrev;
		}
		if (current == XNULL) {
			this->mHead->mPrev = msg;
			msg->mNext = this->mHead;
			this->mHead = msg;
			sendSingal = true;
		} else if (current == this->mTail) {
			this->mTail->mNext = msg;
			msg->mPrev = this->mTail;
			this->mTail = msg;
		} else {
			current->mNext->mPrev = msg;
			msg->mNext = current->mNext;
			current->mNext = msg;
			msg->mPrev = current;
		}
	}
	//DLOG("W=%d,S=%d,NS=%d\n",msg->getWhat(),msg->mTimeSpec.tv_sec,msg->mTimeSpec.tv_nsec);
	//DLOG("Enqueue a Message!\n");
	pthread_mutex_unlock(&mLock);
	if (sendSingal) {
		//DLOG("Send a Singal!\n");
		pthread_cond_signal(&mReady);
	}
}

XMessage *XMessageQueue::dequeue() {
	struct timespec *pTs;
	XMessage *msg = XNULL;
	pthread_mutex_lock(&mLock);
	while ((msg = this->dequeueHead(&pTs)) == XNULL) {
		if (pTs == XNULL) {
			//DLOG("Wait for Message!\n");
			pthread_cond_wait(&mReady, &mLock);
		} else {
			//struct timeval now;
			//gettimeofday(&now,XNULL);
			//ulong m=(pTs->tv_sec-now.tv_sec)*1000+(pTs->tv_nsec/1000000-now.tv_usec/1000);
			//DLOG("Wait for Message, waitTime=%d!\n",m);
			pthread_cond_timedwait(&mReady, &mLock, pTs);
		}
	}
	//DLOG("Get a Message!\n");
	pthread_mutex_unlock(&mLock);
	return msg;
}

XMessage * XMessageQueue::dequeueHead(struct timespec ** pts) {
	XMessage *msg = XNULL;
	*pts = XNULL;
	if(this->mHead!=XNULL){
		struct timespec tsNow;
		struct timeval now;
		gettimeofday(&now, XNULL);
		tsNow.tv_sec = now.tv_sec;
		tsNow.tv_nsec = now.tv_usec * 1000;
		if(XMessageQueue::compareTimeSpec(&tsNow,&(this->mHead->mTimeSpec))>=0){
			msg=this->mHead;
			this->mHead=this->mHead->mNext;
			if(this->mHead==XNULL){
				this->mTail=XNULL;
			}else{
				this->mHead->mPrev=XNULL;
			}
		}else{
			//DLOG("No Avaliv Head!\n");
			*pts = &(this->mHead->mTimeSpec);
		}
	}
	return msg;
}

void *XMessage::getData() const {
	return this->mData;
}

int XMessage::getWhat() const {
	return this->mWhat;
}

int XMessageQueue::compareTimeSpec(const struct timespec *ts1,
		const struct timespec *ts2) {
	int compareResult = 0;
	if (ts1->tv_sec > ts2->tv_sec) {
		compareResult = 1;
	} else if (ts1->tv_sec < ts2->tv_sec) {
		compareResult = -1;
	} else {
		if (ts1->tv_nsec > ts2->tv_nsec) {
			compareResult = 1;
		} else if (ts1->tv_nsec < ts2->tv_nsec) {
			compareResult = -1;
		} else {
			compareResult = 0;
		}
	}
	return compareResult;
}
