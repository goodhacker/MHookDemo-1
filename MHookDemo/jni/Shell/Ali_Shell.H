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
/**
*	BeforeParse_Ali	->系统解析之前调用*
*/
void BeforeParse_Ali(void* inAddr,size_t inLen){

}
/**
*	AfterParse_Ali	->系统解析之后调用*
*/
void AfterParse_Ali(void* inDex){

}


#endif
