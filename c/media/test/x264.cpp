/*
 * x264.cpp
 *
 *  Created on: 2016年7月6日
 *      Author: zzh
 */

#include <stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include <media/H264Encoder.h>
#include <media/H264Decoder.h>

int main(int argc, char** argv) {
	int width = 1280, height = 720;
	int bufferSize = width * height * 3 / 2;

	//H264Decoder::init();

	uchar *buffer = new uchar[bufferSize];
	uchar *i420Buffer = new uchar[bufferSize];
	H264Encoder * encoder = new H264Encoder();
	encoder->set(width, height,30,1024);
	encoder->open();
	int rFile = open("/home/zzh/video", O_RDONLY);
	remove("/home/zzh/h264");
	int wFile = open("/home/zzh/h264",
	O_CREAT | O_WRONLY, 444);
	x264_nal_t *nals;
	int nnal;
	int ii = 0;

	H264Decoder *decoder=new H264Decoder();
	decoder->set(1280,720,30);
	int frameFinished=0;

	while (read(rFile, buffer, bufferSize) > 0) {
		ii++;
		H264Encoder::yv12ToI420(i420Buffer, buffer, width, height);
		int fs = encoder->encode(&nals, &nnal, i420Buffer);
		LOG("I=%d,I420Size=%d, FS=%d\n", ii, bufferSize, fs);
		if (fs > 0) {
			for (int i = 0; i < nnal; ++i) { //将编码数据写入文件.
				write(wFile, nals[i].p_payload, nals[i].i_payload);
				LOG("\ti=%d,H264Size=%d\n", i, nals[i].i_payload);

				decoder->decode(nals[i].p_payload,nals[i].i_payload,&frameFinished,buffer);
				if(frameFinished){
					LOG("FFMpeg Decode H264 Success!\n");
				}
			}
		} else {

		}
	}
	LOG("complete3,i420=%d\n", i420Buffer);
	//delete[] i420Buffer;
	delete[] buffer;
	LOG("complete0\n");
	encoder->close();
	LOG("complete1\n");
	delete encoder;
	close(rFile);
	close(wFile);
	delete decoder;
	LOG("complete2\n");
	LOG("complete3,i420=%d\n", i420Buffer);
	LOG("complete4\n");
}

