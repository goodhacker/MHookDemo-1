#ifndef __HookTest_H_
#define __HookTest_H_
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Substrate.h"

/**
 *
 */
//判断JAVA层能否重复 Hook

//jni能否重复Hook
#define str_libc			"/system/lib/libc.so"
#define str_fopen			"fopen"
//fopen
void* (*_fopen1)(const char * path,const char * mode);
void* My_fopen1(const char * path,const char * mode){
	LOGD("LibCall fopen1 Path:%s,Mode:%s",path,mode);
	return _fopen1(path,mode);
}

void* (*_fopen2)(const char * path,const char * mode);
void* My_fopen2(const char * path,const char * mode){
	LOGD("LibCall fopen2 Path:%s,Mode:%s",path,mode);
	return _fopen2(path,mode);
}
void TestHook(){
	MSImageRef image = MSGetImageByName(str_libc);
	void* mFun = NULL;
	if(NULL != image){
		mFun = MSFindSymbol(image,str_fopen);
		LOGD("LibCall My_fopen1 Path:%p",mFun);
		if(mFun != NULL){
			MSHookFunction(mFun,(void*)&My_fopen1,(void**)&_fopen1);
		}
		mFun = MSFindSymbol(image,str_fopen);
		LOGD("LibCall My_fopen2 Path:%p",mFun);
		if(mFun != NULL){
			MSHookFunction(mFun,(void*)&My_fopen2,(void**)&_fopen2);
		}
	}
}







#endif
