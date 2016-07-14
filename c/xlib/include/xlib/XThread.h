/*
 * XThread.h
 *
 *  Created on: 2016年7月5日
 *      Author: zzh
 */

#ifndef SRC_XTHREAD_H_
#define SRC_XTHREAD_H_

#include<xlib/Config.h>
#include<xlib/XMessage.h>
#include<pthread.h>

typedef void *(FuncRunnable)(void *);

class XThread;

typedef struct {
	XThread *mThread;
	void *mArgs;
} XArgs;

class XThread {
public:
	XThread(bool = false);
	virtual ~XThread();
	int start(void * args= XNULL);
	int stop(bool=true);
	pthread_t getThreadId() const;
	void setRunnable(FuncRunnable *);
	void sendEmptyMessage(int);
	void sendMessage(XMessage *);
	void sendEmptyMessageDelay(int,ulong);
	void sendMessageDelay(XMessage *,ulong);
private:
	pthread_t mThreadId;
	static void *run(void *);
	bool mLooperEnabled, mLooping;
	FuncRunnable *mRunnable;
	XArgs *mArgs;
	bool mStop;
	void enableLooper();
	void loop();
	XMessageQueue *mMsgQueue;
protected:
	virtual void handleMessage(XMessage *);
	virtual void *myRun(void * a);
};

#endif /* SRC_XTHREAD_H_ */
