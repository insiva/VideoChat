/*
 * H264Decoder.cpp
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#include<media/H264Decoder.h>
#include <string.h>

bool H264Decoder::bInitialized = false;

void H264Decoder::init() {
	/* must be called before using avcodec lib*/
	//avformat_open_input();
	/* register all the codecs */
	avcodec_register_all();
	H264Decoder::bInitialized = true;
}

H264Decoder::H264Decoder() {
	if(!H264Decoder::bInitialized)
	{
		H264Decoder::init();
	}
	XASSERT(H264Decoder::bInitialized, "H264Decoder Not Inited!\n");
	pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	pCodecCtx = avcodec_alloc_context3(pCodec);
	this->pPacket = new AVPacket();
	bool b = (avcodec_open2(pCodecCtx, pCodec, XNULL) >= 0);
	XASSERT(b, "Avcodec Open Failed!\n");
	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	XASSERT(pFrame && pFrameRGB, "Frame alloced failed!\n");
	img_convert_ctx = XNULL;
}

H264Decoder::~H264Decoder() {
	avcodec_close(pCodecCtx);
	av_free(pCodecCtx);
	pCodecCtx = XNULL;
	av_free(pFrame);
	pFrame = XNULL;
	av_free(pFrameRGB);
	pFrameRGB = XNULL;
	delete this->pPacket;
	this->pPacket = XNULL;
}

H264Decoder *H264Decoder::set(int width,int height,int fps) {
	pCodecCtx->time_base.num = 1;
	pCodecCtx->frame_number = 1; //每包一个视频帧
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->bit_rate = 0;
	pCodecCtx->time_base.den = fps; //帧率
	pCodecCtx->width = width; //视频宽
	pCodecCtx->height = height; //视频高
	return this;
}

void H264Decoder::decode(uchar *h264Buffer, int h264Len, int *frameFinished,
		uchar *i420Buffer) {
	*frameFinished = 0;
	this->pPacket->data = h264Buffer; //这里填入一个指向完整H264数据帧的指针
	this->pPacket->size = h264Len; //这个填入H264数据帧的大小
	avcodec_decode_video2(pCodecCtx, pFrame, frameFinished, this->pPacket);
	if (*frameFinished) {
		int picSize = pCodecCtx->height * pCodecCtx->width;
		//int newSize = picSize * 1.5;

		//申请内存
		//unsigned char *buf = new unsigned char[newSize];

		int height = pCodecCtx->height;
		int width = pCodecCtx->width;

		//写入数据
		int a = 0, i;
		for (i = 0; i < height; i++) {
			memcpy(i420Buffer + a, pFrame->data[0] + i * pFrame->linesize[0],
					width);
			a += width;
		}
		for (i = 0; i < height / 2; i++) {
			memcpy(i420Buffer + a, pFrame->data[1] + i * pFrame->linesize[1],
					width / 2);
			a += width / 2;
		}
		for (i = 0; i < height / 2; i++) {
			memcpy(i420Buffer + a, pFrame->data[2] + i * pFrame->linesize[2],
					width / 2);
			a += width / 2;
		}
	}
}
