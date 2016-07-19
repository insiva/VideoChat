/*
 * H264Decoder.h
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#ifndef INCLUDE_MEDIA_H264DECODER_H_
#define INCLUDE_MEDIA_H264DECODER_H_

#include<media/Config.h>
extern "C" {

#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}
;

class H264Decoder {
public:
	H264Decoder();
	~H264Decoder();
	H264Decoder *set(int width, int height, int fps);
	void decode(uchar *, int, int *, uchar *);
	bool isOpen() const;

private:
	AVCodec *pCodec;
	AVCodecContext *pCodecCtx;
	SwsContext *img_convert_ctx;
	AVFrame *pFrame;
	AVFrame *pFrameRGB;
	AVPacket *pPacket;
	bool bOpen;
public:
private:
	static void init();
	static bool bInitialized;
};

#endif /* INCLUDE_MEDIA_H264DECODER_H_ */
