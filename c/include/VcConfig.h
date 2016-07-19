/*
 * config.h
 *
 *  Created on: 2016年6月29日
 *      Author: zzh
 */

#ifndef CONFIG_CONFIG_H_
#define CONFIG_CONFIG_H_

#define _XDEBUG_

#ifdef __ANDROID__
#include<android/log.h>
#define _LOG_TAG_ "video_chat"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,_LOG_TAG_,  __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG ,_LOG_TAG_ , __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO ,_LOG_TAG_  , __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,_LOG_TAG_   , __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,_LOG_TAG_  , __VA_ARGS__)
#define LOG(...) __android_log_print(ANDROID_LOG_INFO   ,_LOG_TAG_, __VA_ARGS__)
#else
#include <stdio.h>
#define LOG(...) printf(__VA_ARGS__)
#endif

#ifdef _XDEBUG_
#define DLOG(...) LOG(__VA_ARGS__)
#else
#define DLOG(...)
#endif
#include <assert.h>

#define	XASSERT(c,...) if(!c){LOG(__VA_ARGS__);assert(c);}
#define XERR(err,msg) if(err==0){LOG("%s Success!\n",msg);}else{LOG("%s Fail, Code: %d, Msg: %s\n",msg,err,gai_strerror(err));}
//#define XERRI(err,suc,fail) if(err==0){LOG("%s\n",suc);}else{LOG("%s, Code: %d, Msg: %s\n",fail,err,gai_strerror(err));}

#define RESULT_OK 0
#define RESULT_ERROR -1
#define XNULL 0

#define UDP_MTU_SIZE 1024

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef long long llong;
typedef unsigned char uchar;
typedef long int lint;
typedef long ptr;

enum RtpType {
	RTP_NONE = 0, H264 = 1, SPEEX = 2
};

enum RtcpType {
	RTCP_NONE = 0,
//	FEEDBACK ,
	CALL_INVITE,
	CALL_ANSWER,
	CALL_REJECT,
	CALL_HANGUP,
	CALL_CAMERA_PARAMETERS
};

#define ERROR_CODE_RTP_DATA_OVERSIZE -2

#endif /* CONFIG_CONFIG_H_ */
