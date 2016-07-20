%module(directors="1") VC /* directors的设置是为了使SWIG支持回调 */  
%{  
/* 指定在当前文本中要引用到的头文件 */  
#include "api/VcManager.h"  
%}


 
%feature("director") VcCallback; /* 设置VcCallback, C++回调Java用的基类 */

/* 解析头文件 */  
%include "api/VcManager.h"
%include "api/VcFriend.h"
%include "api/VcCall.h"
%include "api/VcCallback.h"

%apply(char *STRING, size_t LENGTH) { (char *buffer, size_t len) };
%inline %{
	int pushYv12Frame(char *buffer, size_t len) {
		return VcManager::get()->pushYv12Frame(buffer,len);
	}
%}