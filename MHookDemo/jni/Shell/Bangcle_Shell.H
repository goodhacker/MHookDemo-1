#ifndef __Bangcle_Shell_H_
#define __Bangcle_Shell_H_
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "DexFile/LDexFile.H"
LDexFile* BangcleDex = NULL;
extern JavaVM* GVM;
extern char* AppName;

/*
*									梆梆加固说明
*1.	老阿里加固版本，采用二代加密技术，只是修改偏移内存地址

*4.
*/
void* Bangcle_DumpToFile(void* inp){
	/**/
	sleep(10);
	Mod_Mem::SaveFile(AppName,LDex_Util::GetTimeName("Bangcle2"),BangcleDex->loadAddr,BangcleDex->Size);
	return NULL;
}

/**
*	BeforeParse_Bangcle	->系统解析之前调用*
*/
void BeforeParse_Bangcle(void* inAddr,size_t inLen){
	Mod_Mem::SaveFile(AppName,LDex_Util::GetTimeName("Bangcle1"),inAddr,inLen);
	BangcleDex = new LDexFile();
	BangcleDex->loadAddr = inAddr;
	BangcleDex->Size = inLen;
}
/**
*	AfterParse_Bangcle	->系统解析之后调用*
*	阿里加固采用
*/
void AfterParse_Bangcle(void* inDex){
	BangcleDex->dex = (DexFile*)inDex;
	pthread_t thread;
	pthread_create(&thread,NULL,Bangcle_DumpToFile,NULL);
}
#endif
