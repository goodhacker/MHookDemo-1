#ifndef __Ali_Shell_H_
#define __Ali_Shell_H_
#include "Dump_Dex/DexParse.H"
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
using namespace Dex;
DexParse* AliDex = NULL;
extern JavaVM* GVM;
extern char* AppName;
/*
*									阿里加固说明
*1.	老阿里加固版本，采用二代加密技术，只是修改偏移内存地址
*2.	在执行完Dexparse后，代码仍未还原，所以必须等待修复后才能DUMP
*3.	程序会使用优化函数，将Dex指令优化成ODex，但是ODex下载会导致解包错误，所以必须在优化之前使用
*4.
*/
void* Ali_DumpToFile(void* inp){
	/**/
	sleep(10);
	UnknowDex->DumpToFile(AppName,DexUtil::GetTimeName("Ali2"));
	return NULL;
}

/**
*	BeforeParse_Ali	->系统解析之前调用*
*/
void BeforeParse_Ali(void* inAddr,size_t inLen){
	Mod_Mem::SaveFile(AppName,DexUtil::GetTimeName("Ali1"),inAddr,inLen);
	AliDex = new DexParse();
	AliDex->loadAddr = inAddr;
	AliDex->Size = inLen;
}
/**
*	AfterParse_Ali	->系统解析之后调用*
*	阿里加固采用
*/
void AfterParse_Ali(void* inDex){
	AliDex->dex = (DexFile*)inDex;
	pthread_t thread;
	pthread_create(&thread,NULL,Ali_DumpToFile,NULL);
}
#endif
