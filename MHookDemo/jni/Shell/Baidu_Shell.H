#ifndef __Baidu_Shell_H_
#define __Baidu_Shell_H_
#include "Dump_Dex/DexParse.H"
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
using namespace Dex;
extern JavaVM* GVM;
extern char* AppName;
DexParse* BaiduDex = NULL;
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
*/
//_________________________________________________________________________________________________________________
//加密Hook
static void (*$BaidDu_A_e)(JNIEnv*,jobject,jobject);
static void $My_BaidDu_A_e(JNIEnv *jni, jobject obj,jobject inStr)
{
	LOGD("My_BaidDu_A_e %p %p %p",jni,obj,inStr);
	LOGD("My_BaidDu_A_e %s ", jstringTostring(jni,(jstring)inStr));
}
//解密Hook
static void (*$BaidDu_A_d)(JNIEnv*,jobject,jobject);
static void $My_BaidDu_A_d(JNIEnv *jni, jobject obj,jobject inStr)
{
	LOGD("My_BaidDu_A_d %p %p %p",jni,obj,inStr);
	LOGD("My_BaidDu_A_d %s ", jstringTostring(jni,(jstring)inStr));
}
//将传入参数，直接解密
static void RestoreCode_1(JNIEnv* env,char* pstr){
	char* m_str = (char*)malloc(1024);
	//组建字符串
	memset(m_str,0,1024);
	sprintf(m_str,"%s->onCreate001(Landroid/os/Bundle;)V",pstr);
	jstring msrt = pcharToJstring(env,m_str,NULL);
	$BaidDu_A_d(env,NULL,msrt);
	free(m_str);
}
//___________________________________________________________________________________________________________________
/**
 *
 */
void* Baidu_DumpToFile(void* inp){
	BaiduDex->DumpToFile(AppName,DexUtil::GetTimeName("Demo3"));
	return NULL;
}
/**
 *BeforeParse_Baidu	->系统解析之前调用
 *初始化加载地址和加载宽度
 *至于解析还是放到系统解析完毕后在解析
 */
void BeforeParse_Baidu(void* inAddr,size_t inLen){
	LOGD("BeforeParse_Baidu");
	BaiduDex = new DexParse();
	BaiduDex->loadAddr = inAddr;
	BaiduDex->Size = inLen;
}
/**
 *AfterParse_Baidu	-> 系统解析之后调用
 *传入系统获取到的 DexFile*
 *
 */
void AfterParse_Baidu(void* inDex){
	LOGD("AfterParse_Baidu");
	BaiduDex->dex = (DexFile*)inDex;
	//解密时候数据分段必须非常犀利,不能额报错产生,解析全部Ids头,主要是获取ClassDef
	//然后遍历onCreate001,有onCreate001则直接使用解密函数进行解密
	BaiduDex->Parse();
	//判断是否存在修复类，就显示修复
	JNIEnv* env = NULL; //注册时在JNIEnv中实现的，所以必须首先获取它
	if(GVM != NULL){
		if(GVM->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK){
			LOGD("获取 JNIEnv 失败");
		}else{
			//判断第三代加固主要类是否存在
			jclass cls1 = env->FindClass("com/baidu/protect/A");
			if(cls1 != NULL){
				//判断解密类是否存在
				jmethodID clsmid = env->GetStaticMethodID(cls1,"d","(Ljava/lang/String;)V");
				if(clsmid == NULL){
					LOGD("com/baidu/protect/A;->d");
				}else{
					MSJavaHookMethod(env,cls1,clsmid,(void*)&$My_BaidDu_A_d,(void**)&$BaidDu_A_d);
				}
				//判断加密类是否存在
				clsmid = env->GetStaticMethodID(cls1,"e","(Ljava/lang/String;)V");
				if(clsmid == NULL){
					LOGD("com/baidu/protect/A;->e");
				}else{
					MSJavaHookMethod(env,cls1,clsmid,(void*)&$My_BaidDu_A_e,(void**)&$BaidDu_A_e);
				}
				//遍历METHODS,获取需要解析的类，还原代码，
				Dex_Methods* method = BaiduDex->methods;
				Dex_Types* types = BaiduDex->types;
				Dex_String* string = BaiduDex->string;
				//获取onCreate方法名
				size_t onCreate = string->getIDSNo("onCreate");
				size_t onCreate001 = string->getIDSNo("onCreate001");
				LOGD("onCreate ：%d",onCreate);
				LOGD("onCreate001 ：%d",onCreate001);
				//遍历查找方法名为onCreate001的method
				method->IDS.Reset();
				while(NULL != method->IDS.Get()){
					if(onCreate001 == method->IDS.Get()->nameIdx){
						//查找到onCreate001方法，获取类名
						size_t classIdx = method->IDS.Get()->classIdx;
						//从类名中获取字符串
						size_t inType = types->getIdxNoType(classIdx);
						char* mString = string->getString(inType);
						LOGD("onCreate001 :%s",mString);
						RestoreCode_1(env,mString);
					}
					method->IDS.Next();
				}
				//更换String onCreate  onCreate001
				/*
				stringItem*	str1 = (stringItem*)malloc(sizeof(stringItem));
				stringItem* strOncreat = string->IDS.get(onCreate);
				stringItem* strOncreat001 = string->IDS.get(onCreate001);
				memcpy(str1,strOncreat,sizeof(stringItem));
				memcpy(strOncreat,strOncreat001,sizeof(stringItem));
				memcpy(strOncreat001,str1,sizeof(stringItem));/**/
				/**/
				//更新全部代码
				BaiduDex->Dexclass->Code_AddAll(BaiduDex->dex);
			}
		}
	}else{
		LOGD("GVM is null");
	}
	/**/
	pthread_t thread;
	pthread_create(&thread,NULL,Baidu_DumpToFile,NULL);
}
#endif
