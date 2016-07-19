/*
 * main.cpp
 *
 *  Created on: 2016年7月11日
 *      Author: zzh
 */

#include<api/VcManager.h>
#include<unistd.h>
#include<api/VcCallback.h>
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
		//sleep(5);
		VcManager::get()->handleCall(call, VcCallAction::CALL_ACTION_ANSWER);
	}
	void onDisconnect(VcCall *call) {
		LOG("Call Disconnected!\n");
	}
	void onConfirmed(VcCall *call) {
		LOG("Call Confirmed!\n");
		//VcManager::get()->handleCall(call,CallAction::CALL_ACTION_HANGUP);
		//sleep(3);
		VcManager::get()->setMyCameraParameters(1280,720,30);
		int *data = new int();
		*data = VcCallAction::CALL_ACTION_HANGUP;
		XMessage *msg = XMessage::obtain(WHAT_CALL_ACTION, data);
		VcManager::get()->sendMessageDelay(msg, 5000 * 1000);
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
	VcManager::init(IP, 4525, 2);
	VcManager::get()->addFriend(1, IP, 4523);

	XThread *xt = new XThread();
	xt->setRunnable(&pushFrame);
	xt->start();

	Callback cb;
	VcManager::get()->setCallback(&cb);
	sleep(1);
	VcManager::get()->makeCall(1);
	sleep(1000);
	VcManager::get()->deinit();
	return 0;
}
