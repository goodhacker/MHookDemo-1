#ifndef __Shell_H_
#define __Shell_H_
#include <stdio.h>
#include <unistd.h>
#include "Baidu_Shell.H"
#define UNKnown					0
#define Baidu_Jiagu				1
#define Final_Baidu_Jiagu		"libbaiduprotect"
/*
************************************************************
*		getencryptClass
*获取加固类型*
************************************************************
*/
static int encryptClas = UNKnown;

void setencryptClass(char* inLoadSo){
	if(strstr(inLoadSo,Final_Baidu_Jiagu)){
		LOGD("加固方式：百度加固");
		encryptClas = Baidu_Jiagu;
	}
}
/*
************************************************************
*		BeforeParse_UnShell
*在解析之前的处理函数*
************************************************************
*/
void BeforeParse_UnShell(void* inAddr,size_t inLen){
	if(encryptClas == Baidu_Jiagu){
		BeforeParse_Baidu(inAddr,inLen);
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
	if(encryptClas == Baidu_Jiagu){
		AfterParse_Baidu(inDex);
	}
}
#endif

