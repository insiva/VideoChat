/*
 * thread.cpp
 *
 *  Created on: 2016年7月5日
 *      Author: zzh
 */
#include<xlib/XThread.h>
#include<string.h>
#include<unistd.h>
#include<math.h>
#include<stack>

class SThread:public XThread{

public:
	SThread(bool enabledLooper):XThread(enabledLooper){

	}
	~SThread(){
		DLOG("Sthread delete\n");
	}
	void handleMessage(XMessage *msg){
		LOG("handle message, what=%d\n",msg->getWhat());
	}

	void * myRun(void *){
		DLOG("SThread Start!\n");
	}
};

SThread *st;

void * f(void * args){
	LOG("Msg2: %s\n",(char *)args);
	int i=0;
	while(true){
		sleep(1);
		st->sendEmptyMessage(i++);
	}
	return XNULL;
}

class CA{
public:
	CA(){

	}
	~CA(){
		LOG("delete ca\n");
	}
};


int main(int argc, char** argv) {
	int a=4,b=3;
	int c=ceil((double)a/b);
	LOG("c=%d\n",c);

	CA *a1=new CA();
	CA *a2=new CA();
	std::stack<CA> *s=new std::stack<CA>();
	s->push(*a1);
	s->push(*a2);

	//delete a1;
	//delete a2;
	delete s;
	sleep(5);
	st=new SThread(true);
	st->start();
	sleep(1);
	st->sendEmptyMessage(1);
	st->sendEmptyMessage(2);
	st->sendEmptyMessage(3);

	st->sendEmptyMessageDelay(4,3000);
	st->sendEmptyMessageDelay(5,3000);
	st->sendEmptyMessageDelay(6,3000);
	st->sendEmptyMessageDelay(7,6000);
	st->sendEmptyMessageDelay(8,5000);
	st->sendEmptyMessageDelay(9,4000);
	st->sendEmptyMessageDelay(10,3000);
	XThread  *xt=new XThread();
	const char *t="test thread";
	char *ct=new char[strlen(t)+1];
	strcpy(ct,t);
	sleep(20);
	xt->setRunnable(&f);
	xt->start(ct);
	sleep(50);
	delete xt;
	st->stop();
	delete st;
	return 0;
}



