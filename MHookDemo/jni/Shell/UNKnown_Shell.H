#ifndef __UNKNOWN_SHELL_H_
#define __UNKNOWN_SHELL_H_
#include "Dump_Dex/DexParse.H"
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
using namespace Dex;
DexParse* UnknowDex = NULL;
extern JavaVM* GVM;
extern char* AppName;
/*
*									未知加固说明
*1.	未知加固
*2.	先Dump从DexParse加载的数据，
*3.	设置UnknowDex数据
*/
void* UNKnow_DumpToFile(void* inp){
	sleep(10);
	UnknowDex->DumpToFile(AppName,LDex_Util::GetTimeName("Unknow2"));
	return NULL;
}

/**
*	BeforeParse_UNKnow	->系统解析之前调用*
*/
void BeforeParse_UNKnow(void* inAddr,size_t inLen){
	Mod_Mem::SaveFile(AppName,LDex_Util::GetTimeName("Unknow1"),inAddr,inLen);
	UnknowDex = new DexParse();
	UnknowDex->loadAddr = inAddr;
	UnknowDex->Size = inLen;
}
/**
*	AfterParse_UNKnow	->系统解析之后调用*
*/
void AfterParse_UNKnow(void* inDex){
	UnknowDex->dex = (DexFile*)inDex;
	pthread_t thread;
	pthread_create(&thread,NULL,UNKnow_DumpToFile,NULL);
}
#endif
