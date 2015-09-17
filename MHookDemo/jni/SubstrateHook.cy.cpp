#include <stdio.h>
#include <unistd.h>
#include "Substrate.H"
#include <sys/stat.h>
#include "Common/Common.H"
#include "Hook/Hook_JNI.H"
#include "Shell/Shell.H"
#include "HFile/NativeLog.h"
#include <dlfcn.h>
#include <fcntl.h>
//存放读取的配置文件
char* Config = NULL;
char* AppName = NULL;
JavaVM* GVM = NULL;
void* Jni_Onload_Addr = NULL;
//存放全部需要进程Hook的进程名
MSConfig(MSFilterLibrary, "/system/lib/libdvm.so");
//基本函数入口点捕捉
#define libdvm				"/system/lib/libdvm.so"
#define dvmLoadNativeCode	"_Z17dvmLoadNativeCodePKcP6ObjectPPc"
#define Final_JNI_OnLoad	"JNI_OnLoad"
//JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void *reserved)
void* (*_JNI_OnLoad)(JavaVM* vm, void *reserved) = NULL;
void* My_JNI_OnLoad(JavaVM* vm, void *reserved){
	//通过Hook获取到JavaVM，保存JavaVM
	LOGD("My_JNI_OnLoad is Run %p",vm);
	GVM = vm;
	return _JNI_OnLoad(vm,reserved);
}
//Hook dvmLoadNativeCode
bool (*_dvmLoadNativeCode)(char* pathName, void* classLoader, char** detail);
bool My_dvmLoadNativeCode(char* pathName, void* classLoader, char** detail){
	char* mConfig = getConfig();
	//获取SO配置信息,和Hook应用名单
	LOGD("My_dvmLoadNativeCode :%s %s",pathName,mConfig);
	//判断加载的SO是否存在配置包名，
	//加载SO必须存在Jni_Onload否则忽视
	//设置包名来判断是否已经被Hook
	setencryptClass(pathName);
	if(NULL == _JNI_OnLoad){
		MSImageRef image = dlopen(pathName, RTLD_NOW);
		if(image != NULL){
			Jni_Onload_Addr= MSFindSymbol(image,Final_JNI_OnLoad);
			if(Jni_Onload_Addr != NULL){
				MSHookFunction(Jni_Onload_Addr,(void*)&My_JNI_OnLoad,(void**)&_JNI_OnLoad);
			}
		}
	}
	if((AppName == NULL)&&(mConfig != NULL)){
		char *delim = ",";
		char* msrc = (char*)malloc(strlen(mConfig)+1);
		memset(msrc,0,strlen(mConfig)+1);
		memcpy(msrc,mConfig,strlen(mConfig));
		char *p  =  strtok(mConfig, delim);
		if(*p != NULL){
			do{
				if(strstr(pathName,p) != NULL){
					p = strdup(p);
					break;
				}
			}while(p = strtok(NULL, delim));
			free(msrc);
		}
		free(mConfig);
		if(p != NULL){
			//设置AppName,在这里设置是为防止Hook_DVM多次运行，
			AppName =p;
			LOGD("dvmLoadNativeCode Hook_Main %s",AppName);
			Hook_DVM();
		}
	}
	return _dvmLoadNativeCode(pathName,classLoader,detail);
}
/**
 *			MSInitialize
 *	程序入口点，
 * 		一定是最开始运行，但是不一定是进程中最开始运行
 *
 */
MSInitialize
{
	//开始一些基本Hook来捕捉程序
	MSImageRef image = MSGetImageByName(libdvm);
	void* mFun = NULL;
	if(image != NULL){		
		mFun = MSFindSymbol(image,dvmLoadNativeCode);
		if(mFun != NULL){
			MSHookFunction(mFun,(void*)&My_dvmLoadNativeCode,(void**)&_dvmLoadNativeCode);
		}
	}
}
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void *reserved) //这是JNI_OnLoad的声明，必须按照这样的方式声明
{
	LOGD("Substrate JNI_OnLoad");
	//保存全局JavaVM
	GVM = vm;
	//注册时在JNIEnv中实现的，所以必须首先获取它
	JNIEnv* env = NULL;
	jint result = -1;
	//从JavaVM获取JNIEnv，一般使用1.4的版本
	if(vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
		return -1;
	//这里很重要，必须返回版本，否则加载会失败。
	return JNI_VERSION_1_4;
}
