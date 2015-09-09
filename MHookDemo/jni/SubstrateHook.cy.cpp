#include <stdio.h>
#include <unistd.h>
#include "Substrate.H"
#include <sys/stat.h>
#include "Common/Common.H"
#include "Hook/Hook_JNI.H"
#include "HFile/NativeLog.h"
#include <dlfcn.h>
#include <fcntl.h>
//存放读取的配置文件
char* Config = NULL;
char* AppName = NULL;
JavaVM* GVM = NULL;
//存放全部需要进程Hook的进程名
MSConfig(MSFilterLibrary, "/system/lib/libdvm.so");
//基本函数入口点捕捉
#define libdvm		"/system/lib/libdvm.so"
#define dvmLoadNativeCode	"_Z17dvmLoadNativeCodePKcP6ObjectPPc"
//Hook dvmLoadNativeCode
bool (*_dvmLoadNativeCode)(char* pathName, void* classLoader, char** detail);
bool My_dvmLoadNativeCode(char* pathName, void* classLoader, char** detail){
	//获取SO配置信息,和Hook应用名单
	LOGD("My_dvmLoadNativeCode :%s",pathName);
	char* mConfig = getConfig();
	LOGD("Config:%s",mConfig);
	//判断加载的SO是否存在配置包名，判断是否被Hook过
	if((AppName == NULL)&&(mConfig != NULL)){
		char *delim = ",";
		char* msrc = (char*)malloc(strlen(mConfig)+1);
		memset(msrc,0,strlen(mConfig)+1);
		memcpy(msrc,mConfig,strlen(mConfig));
		char *p  =  strtok(mConfig, delim);
		if(*p != NULL){
			do{
				if(strstr(pathName,p) != NULL){
					AppName =strdup(p);
					break;
				}
			}while(p = strtok(NULL, delim));
			free(msrc);
		}
		free(mConfig);
		if(AppName != NULL){
			LOGD("dvmLoadNativeCode Hook_Main");
			LOGD("AppName:%s",AppName);
			MSImageRef image = dlopen(pathName, RTLD_NOW);

			//Hook_DVM();
		//	Hook_Libc();
		}
	}
	return _dvmLoadNativeCode(pathName,classLoader,detail);
}
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void *reserved) //这是JNI_OnLoad的声明，必须按照这样的方式声明
{
	LOGD("Substrate JNI_OnLoad");
	GVM = vm;			//保存全局JavaVM
	JNIEnv* env = NULL; //注册时在JNIEnv中实现的，所以必须首先获取它
	jint result = -1;
	if(vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) //从JavaVM获取JNIEnv，一般使用1.4的版本
		return -1;
	return JNI_VERSION_1_4; //这里很重要，必须返回版本，否则加载会失败。
}
/**
 *			MSInitialize
 *	程序入口点，
 * 		一定是最开始运行，但是不一定是进程中最开始运行
 *
 */
MSInitialize
{
	//获取配置文件
	Config = getConfig();
	LOGD("MSInitialize Config:%s",Config);
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


