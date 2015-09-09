#ifndef __Hook_DVM_H_
#define __Hook_DVM_H_
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Substrate.h"
#include "HFile/NativeLog.h"
#include "Common/Utils.H"
#include "Shell/Shell.H"
//dvm  Hook 段
#define libdvm			"/system/lib/libdvm.so"
#define dexFileParse	"_Z12dexFileParsePKhji"
#define dvmDexFileOpenFromFd "_Z20dvmDexFileOpenFromFdiPP6DvmDex"
#define dvmDexFileOpenPartial "_Z21dvmDexFileOpenPartialPKviPP6DvmDex"

//调用外部Dump_DexFile
extern void Dump_DexFile(void* inAddr,size_t inLen,void* inDex);
//__________________________________________________________________________________________
//									Libdvm
extern JavaVM* GVM;
//
static void* (*_dexFileParse)(int *, unsigned int, int);
static void* My_dexFileParse(int *inAddr, unsigned int length, int parseFlags){
	LOGD("LibCall dexFileParse:inDex:%p,length:%p",inAddr,length);
	BeforeParse_UnShell(inAddr,length);
	void* Out_DexFile = _dexFileParse(inAddr,length,parseFlags);
	LOGD("LibCall dexFileParse:DexFile:%p",Out_DexFile);
	AfterParse_UnShell(Out_DexFile);
	//屏蔽长度过小的长度
/*	if(length > 0x2A00){
		Dump_DexFile(inAddr,length,Out_DexFile);
	}/**/
	Dump_DexFile(inAddr,length,Out_DexFile);
	return Out_DexFile;
}
//	dvmDexFileOpenFromFd
int (*_dvmDexFileOpenFromFd)(int fd, void* ppDvmDex);
int My_dvmDexFileOpenFromFd(int fd, void* ppDvmDex){
	LOGD("LibCall My_dvmDexFileOpenFromFd %p",GETLR());
	return _dvmDexFileOpenFromFd(fd,ppDvmDex);
}
//	dvmDexFileOpenPartial
int (*_dvmDexFileOpenPartial)(const void* addr, int len, void* ppDvmDex);
int My_dvmDexFileOpenPartial(const void* addr, int len, void* ppDvmDex){
	LOGD("LibCall _dvmDexFileOpenPartial");
	return _dvmDexFileOpenPartial(addr,len,ppDvmDex);
}
/**
 *			Hook_DVM
 *	开始Hook LibDvm相关库函数
 *
 */
int Hook_DVM(){
	LOGD("Hook_DVM StartHook");
	MSImageRef image = MSGetImageByName(libdvm);
	void* mFun = NULL;
	if(image != NULL){
		//Hook解析函数
		mFun = MSFindSymbol(image,dexFileParse);
		if(mFun != NULL){
			MSHookFunction(mFun,(void*)&My_dexFileParse,(void**)&_dexFileParse);
		}
		//
		mFun = MSFindSymbol(image,dvmDexFileOpenFromFd);
		if(mFun != NULL){
//			MSHookFunction(mFun,(void*)&My_dvmDexFileOpenFromFd,(void**)&_dvmDexFileOpenFromFd);
		}
		//
		mFun = MSFindSymbol(image,dvmDexFileOpenPartial);
		if(mFun != NULL){
//			MSHookFunction(mFun,(void*)&My_dvmDexFileOpenPartial,(void**)&_dvmDexFileOpenPartial);
		}
	}
	return 0;
}
#endif
