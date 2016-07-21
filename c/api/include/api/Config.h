/*
 * Config.h
 *
 *  Created on: 2016年7月7日
 *      Author: zzh
 */

#ifndef INCLUDE_API_CONFIG_H_
#define INCLUDE_API_CONFIG_H_

#include <VcConfig.h>

const ulong CALL_NO_RESPONSE_TIMEOUT = 20 * 1000;

const int ERROR_NO_FRIEND_FOUND = -1;

const int WHAT_CALL_INVITE_SUCCESS = 0x0001;
const int WHAT_CALL_INVITE_FAIL = 0x1001;

const int WHAT_CALL_NO_RESPONSE_TIMEOUT = 0x2001;
const int WHAT_CALL_ACTION = 0x2002;

const int WHAT_NEW_YV12_FRAME_PUSHED = 0x3001;
const int WHAT_NEW_H264_FRAME_RECVED = 0x3002;

const int VIDEO_BITRATE = 1024 * 3;

#endif /* INCLUDE_API_CONFIG_H_ */
