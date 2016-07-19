/*
 * X264Encoder.cpp
 *
 *  Created on: 2016年7月6日
 *      Author: zzh
 */

#include <string.h>
#include <media/H264Encoder.h>

H264Encoder::H264Encoder() :
		pX264Handle(XNULL), pX264Param(XNULL) {
	this->pPicIn = new x264_picture_t();
	this->pPicOut = new x264_picture_t();
}

H264Encoder::~H264Encoder() {
	if (this->pX264Param != XNULL) {
		delete this->pX264Param;
		pX264Param = XNULL;
	}
	delete this->pPicIn;
	pPicIn = XNULL;
	delete this->pPicOut;
	pPicOut = XNULL;
	//this->pX264Handle = XNULL;
}

H264Encoder *H264Encoder::set(int width, int height, int fps, int bitRate) {
	this->pX264Param = new x264_param_t();
	//* 配置参数
	//* 使用默认参数，在这里因为我的是实时网络传输，所以我使用了zerolatency的选项，使用这个选项之后就不会有delayed_frames，如果你使用的不是这样的话，还需要在编码完成之后得到缓存的编码帧
	x264_param_default_preset(pX264Param, PRESET, TUNE);
	//* cpuFlags
	pX264Param->i_threads = X264_SYNC_LOOKAHEAD_AUTO;	    //* 取空缓冲区继续使用不死锁的保证.
	//* 视频选项
	pX264Param->i_width = width; //* 要编码的图像宽度.
	pX264Param->i_height = height; //* 要编码的图像高度
	pX264Param->i_frame_total = 0; //* 编码总帧数.不知道用0.
	pX264Param->i_keyint_max = 10;
	//* 流参数
	pX264Param->i_bframe = 5;
	pX264Param->b_open_gop = 0;
	pX264Param->i_bframe_pyramid = 0;
	pX264Param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
	//* Log参数，不需要打印编码信息时直接注释掉就行
#ifdef _XDEBUG_
	//pX264Param->i_log_level = X264_LOG_DEBUG;
#endif
	//* 速率控制参数
	//pX264Param->rc.i_rc_method = X264_RC_CQP;  //参数i_rc_method表示码率控制，CQP(恒定质量)，CRF(恒定码率)，ABR(平均码率)
	pX264Param->rc.i_rc_method = X264_RC_ABR;
	pX264Param->rc.i_bitrate = bitRate;	//1024 * 10; //* 码率(比特率,单位Kbps),X264_RC_ARP时有效
	//* muxing parameters
	pX264Param->i_fps_den = 1; //* 帧率分母
	pX264Param->i_fps_num = fps; //* 帧率分子
	pX264Param->i_timebase_den = pX264Param->i_fps_num;
	pX264Param->i_timebase_num = pX264Param->i_fps_den;

	/*
	 * 图像质量控制
	 rc.f_rf_constant是实际质量，越大图像越花，越小越清晰。
	 param.rc.f_rf_constant_max ，图像质量的最大值。
	 */
	pX264Param->rc.f_rf_constant = 25;
	pX264Param->rc.f_rf_constant_max = 45;

	//* 设置Profile.使用Baseline profile
	x264_param_apply_profile(pX264Param, x264_profile_names[0]);
	this->initPicture();
	return this;
}

void H264Encoder::initPicture() {
	x264_picture_init(pPicOut);
	x264_picture_alloc(pPicIn, X264_CSP_I420, pX264Param->i_width,
			pX264Param->i_height);
	pPicIn->img.i_csp = X264_CSP_I420;
	pPicIn->img.i_plane = 3;
}

void H264Encoder::open() {
	XASSERT(pX264Param, "Encode Params Not Set!\n");
	pX264Handle = x264_encoder_open(pX264Param);
	XASSERT(pX264Handle, "Open X264 Handle Failed!\n");
}

bool H264Encoder::isOpen() const {
	return this->pX264Handle != XNULL;
}

void H264Encoder::close() {
	x264_picture_clean(pPicIn);
	//x264_picture_clean(pPicOut);
	if (pX264Handle != XNULL) {
		x264_encoder_close(pX264Handle);
		pX264Handle = NULL;
	}
}

int H264Encoder::encode(x264_nal_t **ppNals, int *piNal, uchar *i420Buffer) {
	XASSERT(pX264Handle, "Open X264 Handle Failed!\n");
	pPicIn->img.plane[0] = i420Buffer;
	pPicIn->img.plane[1] = pPicIn->img.plane[0]
			+ pX264Param->i_width * pX264Param->i_height;
	pPicIn->img.plane[2] = pPicIn->img.plane[1]
			+ pX264Param->i_width * pX264Param->i_height;
	int frameSize = x264_encoder_encode(pX264Handle, ppNals, piNal,
			this->pPicIn, this->pPicOut);
	pPicIn->i_dts++;
	return frameSize;
}

void H264Encoder::yv12ToI420(uchar *i420, const uchar *yv12, int width,
		int height) {
	memcpy(i420, yv12, width * height);
	//System.arraycopy(yv12bytes, 0, i420bytes, 0, width * height);
	memcpy(i420 + width * height, yv12 + width * height * 5 / 4,
			width * height / 4);
	//System.arraycopy(yv12bytes, width * height + width * height / 4, i420bytes,width * height, width * height / 4);
	memcpy(i420 + width * height * 5 / 4, yv12 + width * height,
			width * height / 4);
	//System.arraycopy(yv12bytes, width * height, i420bytes,width * height + width * height / 4, width * height / 4);
}
