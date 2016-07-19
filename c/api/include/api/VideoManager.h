/*
 * VideoManager.h
 *
 *  Created on: 2016年7月11日
 *      Author: zzh
 */

#ifndef INCLUDE_API_VIDEOMANAGER_H_
#define INCLUDE_API_VIDEOMANAGER_H_

#include<api/Config.h>
#include<media/H264Encoder.h>
#include<media/H264Decoder.h>
#include<sim_rtp/RtpManager.h>

#ifdef __ANDROID__
#include<android/GlHelper.h>
#endif

class VideoManager {
	friend class VcManager;
public:
	VideoManager(RtpManager *rtpManager);
	~VideoManager();
	void initEncoder(int width, int height, int fps);
	void deinitEncoder();
	void initDecoder(int width, int height, int fps);
	void deinitDecoder();

	void onYv12FramePushed(uchar *yv12Frame);
	void onH264FrameRecved(uchar *h264Buffer, size_t h264Length);

private:
	H264Encoder *pEncoder;
	H264Decoder *pDecoder;
	uchar *pEncodeI420Buffer;
	uchar *pDecodeYv12Buffer, *pDecodeI420Buffer;
	int nEncodeWidth, nEncodeHeight;
	int nEncodeFps;
	int nDecodeWidth, nDecodeHeight;
	int nDecodeFps;
	RtpManager *pRtpManager;
	pthread_mutex_t mLock;// = PTHREAD_MUTEX_INITIALIZER;

#ifdef __ANDROID__
	void initGlHepler(int viewWidth, int viewHeight);
	void deinitGlHelper();
	void render();
	GlHelper *pGlHelper;
	int nGlViewWidth, nGlViewHeight;
#endif
	//void pushYv12Frame(char *buffer, size_t length);

private:
	static void yv12ToI420(uchar *i420Buffer, const uchar *yv12Buffer,
			int width, int height);
	static void i420ToYv12(uchar *yv12Buffer, const uchar *i420Buffer,
			int width, int height);
};

#endif /* INCLUDE_API_VIDEOMANAGER_H_ */
