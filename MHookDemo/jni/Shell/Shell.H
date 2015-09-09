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
*		Start_UnShell
*根据加固类型获取加固方式*
************************************************************
*/
void Start_UnShell(){
	if(encryptClas == Baidu_Jiagu){
		//百度加固脱壳
		Dump_Baidu();
	}
}
#endif

