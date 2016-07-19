/*
 * X264Encoder.h
 *
 *  Created on: 2016年7月6日
 *      Author: zzh
 */

#ifndef INCLUDE_XLIB_X264ENCODER_H_
#define INCLUDE_XLIB_X264ENCODER_H_

#include <media/Config.h>
#include <stdint.h>

extern "C" {
#include <x264.h>
};

#define PRESET "veryfast"
#define TUNE "zerolatency"

class H264Encoder {

public:
	H264Encoder();
	~H264Encoder();
	void open();
	void close();
	H264Encoder *set(int, int,int,int);
	int encode(x264_nal_t **, int *, uchar *);
	bool isOpen() const;
	static void yv12ToI420(uchar *,const uchar *,int,int);

private:
	void initPicture();
	x264_t *pX264Handle;
	x264_param_t *pX264Param;
	x264_picture_t *pPicIn;
	x264_picture_t *pPicOut;
};

#endif /* INCLUDE_XLIB_X264ENCODER_H_ */
