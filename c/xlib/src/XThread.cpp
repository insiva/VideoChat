/*
 * XThread.cpp
 *
 *  Created on: 2016年7月5日
 *      Author: zzh
 */

#include<xlib/XThread.h>
#include<netdb.h>

XThread::XThread(bool looperEnabled) :
		mThreadId(0), mLooperEnabled(looperEnabled), mRunnable(XNULL), mStop(
				false), mArgs(XNULL), mMsgQueue(XNULL), mLooping(looperEnabled) {
	if (this->mLooperEnabled) {
		this->enableLooper();
	}
}

XThread::~XThread() {
	//DLOG("XThread delete\n");
	mThreadId = 0;
	if (this->mLooperEnabled) {
		delete this->mMsgQueue;
	}
}

void XThread::setRunnable(FuncRunnable * runnable) {
	this->mRunnable = runnable;
}

void *XThread::run(void *args) {
	void *result = XNULL;
	XArgs *xargs = (XArgs *) args;
	XThread *thread = xargs->mThread;
	result = thread->myRun(xargs->mArgs);
	if (thread->mRunnable != XNULL) {
		result = thread->mRunnable(xargs->mArgs);
	}
	if (thread->mLooperEnabled) {
		thread->loop();
	}
	LOG("Thread(%d) Complete!\n",thread->mThreadId);
	if (thread->mArgs != XNULL) {
		delete thread->mArgs;
	}
	pthread_exit((void *) 1);
	//return result;
}

int XThread::start(void *args) {
	if (this->mArgs != XNULL) {
		delete this->mArgs;
	}
	this->mArgs = new XArgs();
	this->mArgs->mThread = this;
	this->mArgs->mArgs = args;
	int err = pthread_create(&mThreadId, XNULL, &XThread::run, this->mArgs);
	XERR(err, "Thread Start");
	return err;
}

int XThread::stop(bool waitExit) {
	void *tret;
	this->mLooping = false;
	XASSERT(this->mThreadId > 0, "Thread Not Start!\n");
	int err=0;
	if(waitExit){
		 pthread_join(this->mThreadId, &tret);
		XERR(err, "Thread Stop Failed");
	}
	return err;
}

void XThread::enableLooper() {
	this->mMsgQueue = new XMessageQueue();
}

void XThread::loop() {
	while (this->mLooping) {
		XMessage *msg = this->mMsgQueue->dequeue();
		this->handleMessage(msg);
		msg->destroy();
	}
}

void *XThread::myRun(void *args) {
	return XNULL;
}

void XThread::handleMessage(XMessage *data) {

}

void XThread::sendEmptyMessage(int what) {
	this->sendEmptyMessageDelay(what,0);
}

void XThread::sendMessage(XMessage *msg) {
	this->sendMessageDelay(msg,0);
}

void XThread::sendEmptyMessageDelay(int what, ulong delayMilliSeconds) {
	XMessage *msg = XMessage::obtain(what);
	this->sendMessageDelay(msg,delayMilliSeconds);
}

void XThread::sendMessageDelay(XMessage * msg, ulong delayMilliSeconds) {
	if (delayMilliSeconds > 0) {
		struct timeval now;
		gettimeofday(&now, XNULL);
		msg->mTimeSpec.tv_sec = now.tv_sec+delayMilliSeconds/1000;
		msg->mTimeSpec.tv_nsec = now.tv_usec * 1000
				+ (delayMilliSeconds%1000) * 1000 * 1000;
	}
	this->mMsgQueue->enqueue(msg);
}
