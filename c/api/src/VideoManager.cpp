/*
 * VideoManager.cpp
 *
 *  Created on: 2016年7月11日
 *      Author: zzh
 */

#include<api/VideoManager.h>

VideoManager::VideoManager(RtpManager * rtpManager) :
		pRtpManager(rtpManager), pEncodeI420Buffer(XNULL), pDecodeYv12Buffer(
		XNULL), pDecodeI420Buffer(XNULL), mEncodeWidth(0), mEncodeFps(0), mEncodeHeight(
				0), mDecodeWidth(0), mDecodeHeight(0), mDecodeFps(0) {
	this->pEncoder = new H264Encoder();
	this->pDecoder = new H264Decoder();
}

VideoManager::~VideoManager() {
	delete this->pEncoder;
	delete this->pDecoder;
}

void VideoManager::initEncoder(int width, int height, int fps) {
	this->pEncoder->set(width, height, fps, VIDEO_BITRATE);
	this->pEncoder->open();
	this->pEncodeI420Buffer = new uchar[width * height * 3 / 2];
	this->mEncodeWidth = width;
	this->mEncodeHeight = height;
	this->mEncodeFps = fps;
}

void VideoManager::deinitEncoder() {
	this->pEncoder->close();
	if (this->pEncodeI420Buffer != XNULL) {
		delete[] this->pEncodeI420Buffer;
		this->pEncodeI420Buffer = XNULL;
	}
	this->mEncodeWidth = 0;
	this->mEncodeHeight = 0;
	this->mEncodeFps = 0;
}

void VideoManager::initDecoder(int width, int height, int fps) {
	this->pDecoder->set(width, height, fps);
	this->pDecodeYv12Buffer = new uchar[width * height * 3 / 2];
	this->pDecodeI420Buffer = new uchar[width * height * 3 / 2];
	this->mDecodeWidth = width;
	this->mDecodeHeight = height;
	this->mDecodeFps = fps;
}

void VideoManager::deinitDecoder() {
	if (this->pDecodeYv12Buffer != XNULL) {
		delete[] this->pDecodeYv12Buffer;
		this->pDecodeYv12Buffer = XNULL;
		delete[] this->pDecodeI420Buffer;
		this->pDecodeI420Buffer = XNULL;
	}
	this->mDecodeWidth = 0;
	this->mDecodeHeight = 0;
	this->mDecodeFps = 0;
}

void VideoManager::onYv12FramePushed(uchar *yv12Frame) {
	VideoManager::yv12ToI420(this->pEncodeI420Buffer, yv12Frame,
			this->mEncodeWidth, this->mEncodeHeight);
	x264_nal_t *nals;
	int nnal;
	int fs = this->pEncoder->encode(&nals, &nnal, this->pEncodeI420Buffer);
	if (fs > 0) {
		for (int i = 0; i < nnal; ++i) { //将编码数据写入文件.
			LOG("A Pushed Frame Encoded, Index=%d, Size=%d.\n", i,
					nals[i].i_payload);
			this->pRtpManager->sendRtp(RtpType::H264,
					(uchar *) nals[i].p_payload, nals[i].i_payload);
		}
	}
}

void VideoManager::onH264FrameRecved(uchar *h264Buffer, size_t h264Length) {
	int frameFinished = 0;
	this->pDecoder->decode((uchar *) h264Buffer, h264Length, &frameFinished,
			this->pDecodeI420Buffer);
	if (frameFinished) {
		LOG("A Recved H264 Frame Decoded, Size=%d.\n", h264Length);
		VideoManager::i420ToYv12(this->pDecodeYv12Buffer,
				this->pDecodeI420Buffer, this->mDecodeWidth,
				this->mDecodeHeight);
	}

}

void VideoManager::yv12ToI420(uchar *i420Buffer, const uchar *yv12Buffer,
		int width, int height) {
	memcpy(i420Buffer, yv12Buffer, width * height);
	memcpy(i420Buffer + width * height, yv12Buffer + width * height * 5 / 4,
			width * height / 4);
	memcpy(i420Buffer + width * height * 5 / 4, yv12Buffer + width * height,
			width * height / 4);
}

void VideoManager::i420ToYv12(uchar *yv12Buffer, const uchar *i420Buffer,
		int width, int height) {
	memcpy(yv12Buffer, i420Buffer, width * height);
	memcpy(yv12Buffer + width * height * 5 / 4, i420Buffer + width * height,
			width * height / 4);
	memcpy(yv12Buffer + width * height, i420Buffer + width * height * 5 / 4,
			width * height / 4);
}

