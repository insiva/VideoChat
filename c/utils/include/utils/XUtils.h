/*
 * XUtils.h
 *
 *  Created on: 2016年7月1日
 *      Author: zzh
 */

#ifndef INCLUDE_UTILS_XUTILS_H_
#define INCLUDE_UTILS_XUTILS_H_

#include <VcConfig.h>
#include <stdlib.h>
#include<limits.h>

namespace XUtils {

uint xrand(uint = INT_MAX);
lint currentSeconds();
lint currentMilliSeconds(lint = 0);
int bytesToHexString(const uchar *buffer, size_t len, char *hex);
int byteToHexString(uchar n, char *hex);
}

#endif /* INCLUDE_UTILS_XUTILS_H_ */
