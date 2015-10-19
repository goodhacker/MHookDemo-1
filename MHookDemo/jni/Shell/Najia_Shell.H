#ifndef __NAJIA_Shell_H_
#define __NAJIA_Shell_H_
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "DexFile/LDexFile.H"
LDexFile* NajiaDex = NULL;
extern JavaVM* GVM;
extern char* AppName;
/*
*									娜迦加固说明
*1. 此加固采用一代加固方式，使用在解析时Dump出内存既可获取到一个完整Dex
*2.	反编译Dump出来的，修改application的name为app_name.
*/
/**
*	BeforeParse_Ali	->系统解析之前调用
*/
void BeforeParse_Najia(void* inAddr,size_t inLen){
	Mod_Mem::SaveFile(AppName,LDex_Util::GetTimeName("Najia"),inAddr,inLen);
	NajiaDex = new LDexFile();
	NajiaDex->loadAddr = inAddr;
	NajiaDex->Size = inLen;
}
#endif
