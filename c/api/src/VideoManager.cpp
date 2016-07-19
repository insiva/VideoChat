/*
 * VideoManager.cpp
 *
 *  Created on: 2016年7月11日
 *      Author: zzh
 */

#include<api/VideoManager.h>

VideoManager::VideoManager(RtpManager * rtpManager) :
		pRtpManager(rtpManager), pEncodeI420Buffer(XNULL), pDecodeYv12Buffer(
		XNULL), pDecodeI420Buffer(XNULL), nEncodeWidth(0), nEncodeFps(0), nEncodeHeight(
				0), nDecodeWidth(0), nDecodeHeight(0), nDecodeFps(0) {
	this->pEncoder = new H264Encoder();
	this->pDecoder = new H264Decoder();
#ifdef __ANDROID__
	nGlViewWidth=0;
	nGlViewHeight=0;
	pGlHelper=XNULL;
#endif
}

VideoManager::~VideoManager() {
	delete this->pEncoder;
	delete this->pDecoder;
}

void VideoManager::initEncoder(int width, int height, int fps) {
	this->pEncoder->set(width, height, fps, VIDEO_BITRATE);
	this->pEncoder->open();
	this->pEncodeI420Buffer = new uchar[width * height * 3 / 2];
	this->nEncodeWidth = width;
	this->nEncodeHeight = height;
	this->nEncodeFps = fps;
	LOG("Encoder Init, Width=%d, Height=%d, FPS=%d.\n",width,height,fps);
}

void VideoManager::deinitEncoder() {
	this->pEncoder->close();
	if (this->pEncodeI420Buffer != XNULL) {
		delete[] this->pEncodeI420Buffer;
		this->pEncodeI420Buffer = XNULL;
	}
	this->nEncodeWidth = 0;
	this->nEncodeHeight = 0;
	this->nEncodeFps = 0;
}

void VideoManager::initDecoder(int width, int height, int fps) {
	this->pDecoder->set(width, height, fps);
	this->pDecodeYv12Buffer = new uchar[width * height * 3 / 2];
	this->pDecodeI420Buffer = new uchar[width * height * 3 / 2];
	this->nDecodeWidth = width;
	this->nDecodeHeight = height;
	this->nDecodeFps = fps;
	LOG("Decoder Init, Width=%d, Height=%d, FPS=%d.\n",width,height,fps);
#ifdef __ANDROID__
	this->initGlHepler(this->nGlViewWidth, this->nGlViewHeight);
#endif
}

void VideoManager::deinitDecoder() {
	if (this->pDecodeYv12Buffer != XNULL) {
		delete[] this->pDecodeYv12Buffer;
		this->pDecodeYv12Buffer = XNULL;
		delete[] this->pDecodeI420Buffer;
		this->pDecodeI420Buffer = XNULL;
	}
	this->nDecodeWidth = 0;
	this->nDecodeHeight = 0;
	this->nDecodeFps = 0;
}

void VideoManager::onYv12FramePushed(uchar *yv12Frame) {
	VideoManager::yv12ToI420(this->pEncodeI420Buffer, yv12Frame,
			this->nEncodeWidth, this->nEncodeHeight);
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
#ifdef __ANDROID__
	if (this->pGlHelper == XNULL) {
		return;
	}
#endif

	int frameFinished = 0;
	this->pDecoder->decode((uchar *) h264Buffer, h264Length, &frameFinished,
			this->pDecodeI420Buffer);
	if (frameFinished) {
		LOG("A Recved H264 Frame Decoded, Size=%d.\n", h264Length);
#ifdef __ANDROID__
		this->pGlHelper->write(this->pDecodeI420Buffer,
				this->nDecodeWidth * this->nDecodeHeight * 3 / 2);
#endif
		/*
		 VideoManager::i420ToYv12(this->pDecodeYv12Buffer,
		 this->pDecodeI420Buffer, this->mDecodeWidth,
		 this->mDecodeHeight);
		 */
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

#ifdef __ANDROID__
void VideoManager::initGlHepler(int viewWidth, int viewHeight) {
	this->nGlViewWidth = viewWidth;
	this->nGlViewHeight = viewHeight;
	if (this->nGlViewWidth > 0 && this->nDecodeWidth > 0) {
		if (this->pGlHelper != XNULL) {
			delete this->pGlHelper;
			this->pGlHelper = XNULL;
		}
		this->pGlHelper = new GlHelper(this->nGlViewWidth, this->nGlViewHeight,
				this->nDecodeWidth, this->nDecodeHeight);
	}
}

void VideoManager::deinitGlHelper() {
	if (this->pGlHelper != XNULL) {
		delete this->pGlHelper;
		this->pGlHelper = XNULL;
	}
	this->nGlViewHeight = 0;
	this->nGlViewWidth = 0;
}

void VideoManager::render() {
	if (this->pGlHelper != XNULL) {
		this->pGlHelper->render();
	}
}
#endif
