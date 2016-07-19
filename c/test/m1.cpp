/*
 * main.cpp
 *
 *  Created on: 2016年7月11日
 *      Author: zzh
 */

#include<api/VcManager.h>
#include<unistd.h>
#include<api/VcCallback.h>
#include<xlib/XThread.h>
#include<fcntl.h>

const char *IP = "127.0.0.1";
class Callback: public VcCallback {
public:
	Callback() {
	}
	~Callback() {
	}
	void onIncoming(VcCall * call) {
		LOG("Incoming Call!\n");
		VcManager::get()->handleCall(call, VcCallAction::CALL_ACTION_ANSWER);
	}
	void onDisconnect(VcCall *call) {
		LOG("Call Disconnected!\n");
	}
	void onConfirmed(VcCall *call) {
		//sleep(3);
		VcManager::get()->setMyCameraParameters(1280,720,30);
		LOG("Call Confirmed!\n");
	}
	void onEstablished(VcCall *call) {
		LOG("Call Established!\n");
	}
	void onOutgoFail(VcCall *call) {
	}
	void onTimeout(VcCall *call) {
		LOG("Call Timeout!\n");
	}
};

void * pushFrame(void * args) {
	LOG("Push Frame Thread Start!");
	int width = 1280, height = 720;
	int bufferSize = width * height * 3 / 2;
	char *buffer = new char[bufferSize];
	int i = 0;

	while (true) {
		//LOG("Read Start!\n");
		int rFile = open("/home/zzh/video", O_RDONLY);
		//int rFile = open("/sdcard/video", O_RDONLY);
		int ii = 0;
		i++;
		while (read(rFile, buffer, bufferSize) > 0) {
			LOG("%d.%d, Read a YV12 Frame!\n", i, ++ii);
			VcManager::get()->pushYv12Frame(buffer, bufferSize);
			usleep(30*1000);
		}
		usleep(30*1000);
		close(rFile);
		//LOG("Read End!\n");
	}
	return XNULL;
}

int main(int argc, char** argv) {
	LOG("ARGC=%d\n", argc);
	VcManager::init(IP, 4523, 1);

	XThread *xt = new XThread();
	xt->setRunnable(&pushFrame);
	//atof("");
	xt->start();
	VcManager::get()->addFriend(2, IP, 4525);
	Callback cb;
	VcManager::get()->setCallback(&cb);
	sleep(1000);
	return 0;
}

