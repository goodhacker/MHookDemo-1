#ifndef __Shell_H_
#define __Shell_H_
#include <stdio.h>
#include <unistd.h>
#include "UNKnown_Shell.H"
#include "Baidu_Shell.H"
#include "Ali_Shell.H"
#include "Bangcle_Shell.H"
#include "Najia_Shell.H"
//
#define UNKnown					0		//未知加固
#define Baidu_Jiagu				1		//百度加固
#define Ali_Jiagu				2		//阿里加固
#define Bangcle_Jiagu			3		//梆梆加固
#define Najia_Jiagu				4		//娜迦加固
//关键库判断
#define Final_Baidu_Jiagu		"libbaiduprotect"
#define Final_Ali_Jiagu			"libmobisec"
#define Final_Bangcle_Jiagu		"libsecexe"
#define Final_Najia_Jiagu		"libddog"
/*
************************************************************
*				getencryptClass
* 获取加固类型
* 根据加载so库判断
************************************************************
*/
static int encryptClas = UNKnown;
void setencryptClass(char* inLoadSo){
	if(strstr(inLoadSo,Final_Baidu_Jiagu)){
//		DEXLOG("加固方式：百度加固");
		encryptClas = Baidu_Jiagu;
	}else if(strstr(inLoadSo,Final_Ali_Jiagu)){
//		DEXLOG("加固方式：阿里加固");
		encryptClas = Ali_Jiagu;
	}else if(strstr(inLoadSo,Final_Bangcle_Jiagu)){
//		DEXLOG("加固方式：梆梆加固");
		encryptClas = Bangcle_Jiagu;
	}else if(strstr(inLoadSo,Final_Najia_Jiagu)){
//		DEXLOG("加固方式：娜迦加固");
		encryptClas = Najia_Jiagu;
	}
}
/*
************************************************************
*				BeforeParse_UnShell
* 在解析之前的处理函数*
************************************************************
*/
void BeforeParse_UnShell(void* inAddr,size_t inLen){
	if(encryptClas == UNKnown){
		DEXLOG("BeforeParse_UnShell：未知加固");
		BeforeParse_UNKnow(inAddr,inLen);
	}else if(encryptClas == Baidu_Jiagu){
		DEXLOG("BeforeParse_UnShell：百度加固");
		BeforeParse_Baidu(inAddr,inLen);
	}else if(encryptClas == Ali_Jiagu){
		DEXLOG("BeforeParse_UnShell：阿里加固");
		BeforeParse_Ali(inAddr,inLen);
	}else if(encryptClas == Bangcle_Jiagu){
		DEXLOG("BeforeParse_UnShell：梆梆加固");
		BeforeParse_Bangcle(inAddr,inLen);
	}else if(encryptClas == Najia_Jiagu){
		DEXLOG("BeforeParse_UnShell：娜迦加固");
		BeforeParse_Najia(inAddr,inLen);
	}
}
/*
************************************************************
*				BeforeParse_UnShell
* 在解析后的处理函数
* 比如一些加固需要延时才能让Dex完整
************************************************************
*/
void AfterParse_UnShell(void* inDex){
	if(encryptClas == UNKnown){
//		DEXLOG("AfterParse_UnShell：未知加固");
		AfterParse_UNKnow(inDex);
	}else if(encryptClas == Baidu_Jiagu){
//		DEXLOG("AfterParse_UnShell：百度加固");
		AfterParse_Baidu(inDex);
	}else if(encryptClas == Ali_Jiagu){
//		DEXLOG("AfterParse_UnShell：阿里加固");
		AfterParse_Ali(inDex);
	}else if(encryptClas == Bangcle_Jiagu){
//		DEXLOG("AfterParse_UnShell：梆梆加固");
		AfterParse_Bangcle(inDex);
	}else if(encryptClas == Najia_Jiagu){
//		DEXLOG("AfterParse_UnShell：娜迦加固");
//		不需要后续调用，之前调用已经Dump完毕
//		AfterParse_Najia(inDex);
	}
}
/*
************************************************************
*				noDynLoad_UnShell
* 在解析后的处理函数
* 比如一些加固需要延时才能让Dex完整
*
************************************************************
*/
void noDynLoad_UnShell(void* inAddr,size_t inLen){
//	sleep(1);
//	Mod_Mem::SaveFile(AppName,LDex_Util::GetTimeName("Unknow1"),inAddr,inLen);
//	sleep(30);
//	Mod_Mem::SaveFile(AppName,LDex_Util::GetTimeName("Unknow2"),inAddr,inLen);
}
#endif

