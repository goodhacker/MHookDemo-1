#ifndef __Shell_H_
#define __Shell_H_
#include <stdio.h>
#include <unistd.h>
#include "UNKnown_Shell.H"
#include "Baidu_Shell.H"
#include "Ali_Shell.H"
#include "Bangcle_Shell.H"
#define UNKnown					0
#define Baidu_Jiagu				1
#define Ali_Jiagu				2
#define Bangcle_Jiagu			3

#define Final_Baidu_Jiagu		"libbaiduprotect"
#define Final_Ali_Jiagu			"libmobisec"
#define Final_Bangcle_Jiagu		"libsecexe"

/*
************************************************************
*		getencryptClass
*获取加固类型
*根据加载so判断
************************************************************
*/
static int encryptClas = UNKnown;
void setencryptClass(char* inLoadSo){
	if(strstr(inLoadSo,Final_Baidu_Jiagu)){
//		LOGD("加固方式：百度加固");
		encryptClas = Baidu_Jiagu;
	}else if(strstr(inLoadSo,Final_Ali_Jiagu)){
//		LOGD("加固方式：阿里加固");
		encryptClas = Ali_Jiagu;
	}else if(strstr(inLoadSo,Final_Bangcle_Jiagu)){
//		LOGD("加固方式：梆梆加固");
		encryptClas = Bangcle_Jiagu;
	}
}
/*
************************************************************
*		BeforeParse_UnShell
*在解析之前的处理函数*
************************************************************
*/
void BeforeParse_UnShell(void* inAddr,size_t inLen){
	if(encryptClas == UNKnown){
		LOGD("BeforeParse_UnShell：未知加固");
		BeforeParse_UNKnow(inAddr,inLen);
	}else if(encryptClas == Baidu_Jiagu){
		LOGD("BeforeParse_UnShell：百度加固");
		BeforeParse_Baidu(inAddr,inLen);
	}else if(encryptClas == Ali_Jiagu){
		LOGD("BeforeParse_UnShell：阿里加固");
		BeforeParse_Ali(inAddr,inLen);
	}else if(encryptClas == Bangcle_Jiagu){
		LOGD("BeforeParse_UnShell：梆梆加固");
	}
}
/*
************************************************************
*		BeforeParse_UnShell
*在解析后的处理函数
*比如一些加固需要延时才能让Dex完整
************************************************************
*/
void AfterParse_UnShell(void* inDex){
	if(encryptClas == UNKnown){
		LOGD("AfterParse_UnShell：未知加固");
		AfterParse_UNKnow(inDex);
	}else if(encryptClas == Baidu_Jiagu){
		LOGD("AfterParse_UnShell：百度加固");
		AfterParse_Baidu(inDex);
	}else if(encryptClas == Ali_Jiagu){
		LOGD("AfterParse_UnShell：阿里加固");
		AfterParse_Ali(inDex);
	}else if(encryptClas == Bangcle_Jiagu){
		LOGD("AfterParse_UnShell：梆梆加固");

	}
}
#endif

