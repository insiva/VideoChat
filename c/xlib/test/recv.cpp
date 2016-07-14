/*
 * recv.cpp
 *
 *  Created on: 2016年6月30日
 *      Author: zzh
 */
#include <string.h>

#include <stdio.h>

#include<xlib/UdpListener.h>

void onDataRecved(const char* buffer,size_t len,struct sockaddr *fromAddr,void *ptr){
	//printf(buffer);
	//printf("Length = %u, first char=%c\n",len,buffer[0]);
	char * buff=new char[len+2];
	for(unsigned int i=0;i<len;i++){
		buff[i]=buffer[i];
	}
	buff[len]='\n';
	buff[len+1]=0;
	char ip[50];
	//strcpy(ip, (const char*)inet_ntoa(((struct sockaddr_in *)fromAddr)->sin_addr));
	LOG("%s\n",fromAddr->sa_data);
	LOG(buff);
	//printf("\n");
	delete []buff;
}

int main(int argc, char** argv) {
	int err=0;
	UdpListener* ul=new UdpListener(5678);
	//UdpListener* ul=new UdpListener("127.0.0.1",5678);
	FuncHandler<FuncOnDataRecved> handler(XNULL,&onDataRecved);
	ul->setDataRecvedFunc(&handler);
	err=ul->listen();
	if(err<0){
		LOG("failed:%d\n",err);
	}
	delete ul;
	return 0;
}
