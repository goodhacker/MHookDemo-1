#ifndef __Baidu_Shell_H_
#define __Baidu_Shell_H_
#include "DexFile/LDexFile.H"
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
extern JavaVM* GVM;
extern char* AppName;
LDexFile* BaiduDexFile = NULL;
static bool isRestore = false;
/*
*									百度加固说明
*1.此脱壳为脱百度 2代和3代结合版本
*2.加密流程:
*	采用动态加载assets下的baiduprotect.jar
*	然后采用重写onCreate,用onCreate001代替，onCreate内容为修复onCreate001代码、执行onCreate001代码、清楚onCreate001代码。
*	修复代码不能连续运行两次，
*3.采用Hook DexParse来获取Dex相关数据，然后遍历ClassDef将所有onCreate001类直接解码
*4.Dump出修复好的Dex
*5.然而Dump的Dex还要修复(可能可以根据ClassDef自动修改)
*
* 	Lcom/baidu/protect/A;->d(Ljava/lang/String;)V 	->	解密方法
* 	Lcom/baidu/protect/A;->e(Ljava/lang/String;)V 	->	加密方法
*	Lcom/qsq/qianshengqian/XXXXX;->onCreate001(Landroid/os/Bundle;)V"  ->	加解密传入参数
*
*/
//_________________________________________________________________________________________________________________
//加密Hook
static void (*$BaidDu_A_e)(JNIEnv*,jobject,jobject) = NULL;
static void $My_BaidDu_A_e(JNIEnv *env, jobject obj,jobject mJsrt)
{
//	DEXLOG("My_BaidDu_A_e %p %p %p",jni,obj,inStr);
	DEXLOG("My_BaidDu_A_e %s ", jstringTostring(env,(jstring)mJsrt));
//	$BaidDu_A_e(env,obj,mJsrt);
}
//解密Hook
static void (*$BaidDu_A_d)(JNIEnv*,jobject,jobject)= NULL;
static void $My_BaidDu_A_d(JNIEnv *env, jobject obj,jobject mJsrt)
{
//	DEXLOG("My_BaidDu_A_d %p %p %p",jni,obj,inStr);
	DEXLOG("My_BaidDu_A_d %s ", jstringTostring(env,(jstring)mJsrt));
//	$BaidDu_A_d(env,obj,mJsrt);
}
//将传入参数，直接解密
static void RestoreCode(JNIEnv* env,char* pstr){
	char* mstr = (char*)malloc(1024);
	//组建字符串
	memset(mstr,0,1024);
	sprintf(mstr,"%s->onCreate001(Landroid/os/Bundle;)V",pstr);
//	DEXLOG("RestoreCode_1 :%s",m_str);
	jstring mJsrt = pcharToJstring(env,mstr,NULL);
	$BaidDu_A_d(env,NULL,mJsrt);
	free(mstr);
	env->DeleteLocalRef(mJsrt);
}
//___________________________________________________________________________________________________________________
/**
 *
 */
void* Baidu_DumpToFile(void* inp){
	BaiduDexFile->DumpToFile(AppName,LDex_Util::GetTimeName("BaiduParse"));
	return NULL;
}
/**
 *BeforeParse_Baidu	->系统解析之前调用
 *初始化加载地址和加载宽度
 *至于解析还是放到系统解析完毕后在解析
 */
void BeforeParse_Baidu(void* inAddr,size_t inLen){
	DEXLOG("BeforeParse_Baidu");
	BaiduDexFile = new LDexFile();
	BaiduDexFile->loadAddr = inAddr;
	BaiduDexFile->Size = inLen;
}
/**
 *AfterParse_Baidu	-> 系统解析之后调用
 *传入系统获取到的 DexFile*
 *
 */
void AfterParse_Baidu(void* inDex){
	DEXLOG("AfterParse_Baidu");
	BaiduDexFile->dex = (DexFile*)inDex;
	//解密时候数据分段必须非常犀利,不能额报错产生,解析全部Ids头,主要是获取ClassDef
	//然后遍历onCreate001,有onCreate001则直接使用解密函数进行解密
	BaiduDexFile->Parse();
	//判断是否存在修复类，就显示修复
	JNIEnv* env = NULL; //注册时在JNIEnv中实现的，所以必须首先获取它
	if(GVM != NULL){
		if(GVM->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK){
			DEXLOG("获取 JNIEnvJNIEnv 失败");
		}else{
			jclass cls1 = env->FindClass("com/baidu/protect/A");
			//判断第三代加固主要类是否存在
			if(($BaidDu_A_e == NULL)&&(cls1 != NULL)){
				jmethodID clsmid = env->GetStaticMethodID(cls1,"e","(Ljava/lang/String;)V");
				if(clsmid != NULL){
					DEXLOG("开始Hook $My_BaidDu_A_e");
					MSJavaHookMethod(env,cls1,clsmid,(void*)&$My_BaidDu_A_e,(void**)&$BaidDu_A_e);
				}
			}
			if(($BaidDu_A_d == NULL)&&(cls1 != NULL)){
				jmethodID clsmid = env->GetStaticMethodID(cls1,"e","(Ljava/lang/String;)V");
				if(clsmid != NULL){
					DEXLOG("开始Hook $BaidDu_A_d");
					MSJavaHookMethod(env,cls1,clsmid,(void*)&$My_BaidDu_A_d,(void**)&$BaidDu_A_d);
				}
			}
			//判断是否被还原
			if(isRestore){
				return ;
			}else {
				isRestore = true;
			}
			//遍历METHODS,获取需要解析的类，还原代码，
			LDex_Method* DexMethod = BaiduDexFile->DexMethod;
			LDex_Types* DexTypes = BaiduDexFile->DexTypes;
			LDex_String* DexString = BaiduDexFile->DexString;
			//获取onCreate，onCreate001方法名，字符的StringIDS
			size_t onCreate = DexString->getIDSNo("onCreate");
			size_t onCreate001 = DexString->getIDSNo("onCreate001");
			DEXLOG("onCreate ：%d",onCreate);
			DEXLOG("onCreate001 ：%d",onCreate001);
			//遍历查找方法名为onCreate001的method
			DexMethod->IDS.Reset();
			while(NULL != DexMethod->IDS.Get()){
				if(onCreate001 == DexMethod->IDS.Get()->nameIdx){
					//查找到onCreate001方法，获取类名
					size_t classIdx = DexMethod->IDS.Get()->classIdx;
					//从类名中获取字符串
					size_t inType = DexTypes->getIdxNoType(classIdx);
					char* mString = DexString->getString(inType);
					DEXLOG("onCreate001 : %s",mString);
					RestoreCode(env,mString);
				}
				DexMethod->IDS.Next();
			}
			DEXLOG("恢复成功，开始获取代码");
			//更新全部代码
			BaiduDexFile->DexClass->Code_AddAll(BaiduDexFile->dex);
			DEXLOG("代码获取完毕");
			/**/
			Mod_Mem::SaveFile(AppName,LDex_Util::GetTimeName("Have"),BaiduDexFile->loadAddr,BaiduDexFile->Size);
		}
	}else{
		DEXLOG("GVM is null");
	}
	/**/
	pthread_t thread;
	pthread_create(&thread,NULL,Baidu_DumpToFile,env);
//	sleep(10);
//	pthread_create(&thread,NULL,Baidu_DumpToFile,NULL);
}
#endif
//更换String onCreate  onCreate001
/*
stringItem*	str1 = (stringItem*)malloc(sizeof(stringItem));
stringItem* strOncreat = string->IDS.get(onCreate);
stringItem* strOncreat001 = string->IDS.get(onCreate001);
memcpy(str1,strOncreat,sizeof(stringItem));
memcpy(strOncreat,strOncreat001,sizeof(stringItem));
memcpy(strOncreat001,str1,sizeof(stringItem));/**/
