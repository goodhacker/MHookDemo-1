#ifndef __Hook_DVM_H_
#define __Hook_DVM_H_
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "jni.H"
#include "Substrate.h"
#include "Common/Utils.H"
#include "Shell/Shell.H"
//dvm  Hook 段
#define libdvm					"/system/lib/libdvm.so"
#define dexFileParse			"_Z12dexFileParsePKhji"
#define dvmResolveClass			"dvmResolveClass"
#define dvmDexFileOpenFromFd 	"_Z20dvmDexFileOpenFromFdiPP6DvmDex"
#define dvmDexFileOpenPartial 	"_Z21dvmDexFileOpenPartialPKviPP6DvmDex"
//
void* OldLoadDexAddr = NULL;
size_t OldLoadDexLen = NULL;
bool Dex_Dyload = false;
//调用外部Dump_DexFile
extern void Dump_DexFile(void* inAddr,size_t inLen,void* inDex);
//__________________________________________________________________________________________
//									Libdvm
extern JavaVM* GVM;
//
static void* (*_dexFileParse)(int *, unsigned int, int);
static void* My_dexFileParse(int *inAddr, unsigned int length, int parseFlags){
	//已使用分析函数，判定为动态加载
	Dex_Dyload = true;
	void* Out_DexFile = NULL;
	DEXLOG("LibCall dexFileParse:inDex:%p,length:%p",inAddr,length);
	//屏蔽长度过小的长度
	if(length > 0x2000){
		//先查找到原始Dex加载地址
		BeforeParse_UnShell(inAddr,length);
		Out_DexFile = _dexFileParse(inAddr,length,parseFlags);
		DEXLOG("LibCall dexFileParse:DexFile:%p",Out_DexFile);
		AfterParse_UnShell(Out_DexFile);
	}else{
		Out_DexFile = _dexFileParse(inAddr,length,parseFlags);
	}
	//Dump_DexFile(inAddr,length,Out_DexFile);
	return Out_DexFile;
}
//	dvmDexFileOpenFromFd
int (*_dvmDexFileOpenFromFd)(int fd, void* ppDvmDex);
int My_dvmDexFileOpenFromFd(int fd, void* ppDvmDex){
	DEXLOG("LibCall My_dvmDexFileOpenFromFd %p",GETLR());
	return _dvmDexFileOpenFromFd(fd,ppDvmDex);
}
//	dvmDexFileOpenPartial(const ClassObject* referrer,u4 classIdx,bool fromUnverifiedConstant);
int (*_dvmResolveClass)(const void* referrer, int classIdx, bool ppDfromUnverifiedConstantvmDex);
int My_dvmResolveClass(const void* referrer, int classIdx, bool ppDfromUnverifiedConstantvmDex){
	DEXLOG("LibCall My_dvmResolveClass %p %p %p",referrer,classIdx,ppDfromUnverifiedConstantvmDex);
	//打印Dvmdex
	u4* pDvmDex = (u4*)(referrer+40);
	DEXLOG("LibCall pDvmDex  %p",*pDvmDex);
	return _dvmResolveClass(referrer,classIdx,ppDfromUnverifiedConstantvmDex);
}
//	CheckDynLoad
void* CheckDynLoad(void* inNull){
	//判断是否采用动态加载
	//采用动态加载直接跳过，程序会的dexFileParse中分析
	if(!Dex_Dyload){
		OldLoadDexAddr = (void*)getStartAddrByName("/data/dalvik-cache/data");
		OldLoadDexLen = getEndAddrByName("/data/dalvik-cache/data") - (u4)OldLoadDexAddr;
		noDynLoad_UnShell(OldLoadDexAddr,OldLoadDexLen);
	}
	return NULL;
}
/**
 *			Hook_DVM
 *	开始Hook LibDvm相关库函数
 *
 */
int Hook_DVM(){
	DEXLOG("Hook_DVM StartHook");
	//打开libdvm.so
	MSImageRef image = MSGetImageByName(libdvm);
	void* mFun = NULL;
	//开始获取Dex原始地址，此Dex是系统启动加载的Dex,即是原包中包含的Dex
	//获取Dex开线程，若10S内没有使用dexFileParse函数，则表示没有使用动态加载技术。
	if(NULL == OldLoadDexAddr){
		//开线程10S内未使用Dexparse则判断为无自动加载模块
		pthread_t thread;
		pthread_create(&thread,NULL,CheckDynLoad,NULL);
	}
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
		mFun = MSFindSymbol(image,dvmResolveClass);
		if(mFun != NULL){
//			直接打印dvmResolveClass头数据，判断是否被Hook
//			MSHookFunction(mFun,(void*)&My_dvmResolveClass,(void**)&_dvmResolveClass);
		}
	}
	return 0;
}
#endif
