#ifndef __Hook_DVM_H_
#define __Hook_DVM_H_
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Substrate.h"
#include "HFile/NativeLog.h"
//dvm  Hook 段
#define libdvm			"/system/lib/libdvm.so"
#define dexFileParse	"_Z12dexFileParsePKhji"
#define dvmDexFileOpenFromFd "_Z20dvmDexFileOpenFromFdiPP6DvmDex"
#define dvmDexFileOpenPartial "_Z21dvmDexFileOpenPartialPKviPP6DvmDex"
//获取LRS
long GETLR(){
	long mLR = NULL;
	__asm__ __volatile__("mov %0, lr\n\t":  "=r"(mLR));
	return mLR;
}
//调用外部Dump_DexFile
extern void Dump_DexFile(void* inAddr,size_t inLen,void* inDex);
//__________________________________________________________________________________________
//									Libdvm
extern JavaVM* GVM;
//
static char* jstringTostring(JNIEnv* env, jstring jstr)
{
	char* rtn = NULL;
	jclass clsstring = env->FindClass("java/lang/String");
	jstring strencode = env->NewStringUTF("utf-8");
	jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray barr= (jbyteArray) env->CallObjectMethod( jstr, mid, strencode);
	jsize alen = env->GetArrayLength(barr);
	jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
	if (alen > 0)
	{
		rtn = (char*)malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	env->ReleaseByteArrayElements(barr, ba, 0);
	return rtn;
}
jstring pcharToJstring(JNIEnv* env, const char* pat, const char* encoding)
{
    jstring jencoding;
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
    jclass  gStringClass= (env)->FindClass("java/lang/String");
    jmethodID  gmidStringInit= (env)->GetMethodID(gStringClass, "<init>", "([BLjava/lang/String;)V");

    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    if (encoding == NULL)
        jencoding= (env)->NewStringUTF("utf-8");
    else
        jencoding=(env)->NewStringUTF(encoding);

    return (jstring)(env)->NewObject(gStringClass, gmidStringInit, bytes, jencoding);
}
//
static void (*_BaidDu_A_e)(JNIEnv*,jobject,jobject);
static void $My_BaidDu_A_e(JNIEnv *jni, jobject obj,jobject inStr)
{
	LOGD("My_BaidDu_A_e %p %p %p",jni,obj,inStr);
	LOGD("My_BaidDu_A_e %s ", jstringTostring(jni,(jstring)inStr));
}
static void (*_BaidDu_A_d)(JNIEnv*,jobject,jobject);
static void $My_BaidDu_A_d(JNIEnv *jni, jobject obj,jobject inStr)
{
	LOGD("My_BaidDu_A_d %p %p %p",jni,obj,inStr);
	LOGD("My_BaidDu_A_d %s ", jstringTostring(jni,(jstring)inStr));
//	_BaidDu_A_d(jni,NULL,inStr);
}
/*
 */
static void RestoreCode_1(JNIEnv* env,char* pstr){
	jstring msrt = pcharToJstring(env,pstr,NULL);
	_BaidDu_A_d(env,NULL,msrt);
}
static void RestoreCode(JNIEnv* env){
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/AboutActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/AccountBalanceWillDepositActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/AccountBalanceWillWithdrawActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/AccountsBalanceActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/AccountsBalanceTransactionsActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/AllDetailActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/AddBankActivity;->onCreate001(Landroid/os/Bundle;)V");

	RestoreCode_1(env,"Lcom/qsq/qianshengqian/BankcardsActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/BankcardsRestrictActivity;->AccountsBalonCreate001anceActivity(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/BankcardsSafetyActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/BuyModeActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/BuySuccessActivity;->onCreate001(Landroid/os/Bundle;)V");

	RestoreCode_1(env,"Lcom/qsq/qianshengqian/FeedbackActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/ForgetPwdActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/GestureLockActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/LoginActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/NewsActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/NotificationDetailActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/NotificationsActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/ProductsDetailActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/QsqMainActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/RegisterActivity;->onCreate001(Landroid/os/Bundle;)V");

	RestoreCode_1(env,"Lcom/qsq/qianshengqian/SafetySettingActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/SelectBankActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/SelectVoucherActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/ServerActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/SetLockActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/SetLockConfirmActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/SetPwdActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/SplashActivity;->onCreate001(Landroid/os/Bundle;)V");

	RestoreCode_1(env,"Lcom/qsq/qianshengqian/VoucherActivity;->onCreate001(Landroid/os/Bundle;)V");

	RestoreCode_1(env,"Lcom/qsq/qianshengqian/WealthAssetsListActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/WealthDetailActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/WealthInterestListActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/WealthWillwithdrawActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/WealthWithdrawActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/WebActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/qsq/qianshengqian/WilldepositActivity;->onCreate001(Landroid/os/Bundle;)V");

	RestoreCode_1(env,"Lcom/tencent/android/tpush/XGPushActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/umeng/socialize/view/ShareActivity;->onCreate001(Landroid/os/Bundle;)V");
	RestoreCode_1(env,"Lcom/yintong/secure/activity/BaseActivity;->onCreate001(Landroid/os/Bundle;)V");
}
//
static void* (*_dexFileParse)(int *, unsigned int, int);
static void* My_dexFileParse(int *inAddr, unsigned int length, int parseFlags){
	void* Out_DexFile = _dexFileParse(inAddr,length,parseFlags);
	LOGD("LibCall dexFileParse:inDex:0x%08X,length:0x%08X,DexFile:0x%08X",inAddr,length,Out_DexFile);
	//Hook回复函数
	JNIEnv* env = NULL; //注册时在JNIEnv中实现的，所以必须首先获取它
	if(GVM != NULL){
		if(GVM->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK){
			LOGD("获取 JNIEnv 失败");
		}else{
			jclass cls1 = env->FindClass("com/baidu/protect/A");
			jmethodID clsmid = env->GetStaticMethodID(cls1,"d","(Ljava/lang/String;)V");
			if(clsmid == NULL){
				LOGD("com/baidu/protect/A;->d");
			}else{
				MSJavaHookMethod(env,cls1,clsmid,(void*)&$My_BaidDu_A_d,(void**)&_BaidDu_A_d);
			}
			cls1 = env->FindClass("com/baidu/protect/A");
			clsmid = env->GetStaticMethodID(cls1,"e","(Ljava/lang/String;)V");
			if(clsmid == NULL){
				LOGD("com/baidu/protect/A;->e");
			}else{
				MSJavaHookMethod(env,cls1,clsmid,(void*)&$My_BaidDu_A_e,(void**)&_BaidDu_A_e);
			}
			RestoreCode(env);
		}
	}else{
		LOGD("GVM is null");
	}
	//屏蔽长度过小的长度
/*	if(length > 0x2A00){
		Dump_DexFile(inAddr,length,Out_DexFile);
	}/**/
	Dump_DexFile(inAddr,length,Out_DexFile);
	return Out_DexFile;
}
//dvmDexFileOpenFromFd
int (*_dvmDexFileOpenFromFd)(int fd, void* ppDvmDex);
int My_dvmDexFileOpenFromFd(int fd, void* ppDvmDex){
	LOGD("LibCall My_dvmDexFileOpenFromFd %p",GETLR());
	return _dvmDexFileOpenFromFd(fd,ppDvmDex);
}
//dvmDexFileOpenPartial
int (*_dvmDexFileOpenPartial)(const void* addr, int len, void* ppDvmDex);
int My_dvmDexFileOpenPartial(const void* addr, int len, void* ppDvmDex){
	LOGD("LibCall _dvmDexFileOpenPartial");
	return _dvmDexFileOpenPartial(addr,len,ppDvmDex);
}
/**
 *			Hook_DVM
 *align_Len
 */
int Hook_DVM(){
	LOGD("Hook_DVM StartHook");
	MSImageRef image = MSGetImageByName(libdvm);
	void* mFun = NULL;
	if(image != NULL){
		//Hook解析函数
		mFun = MSFindSymbol(image,dexFileParse);
		if(mFun != NULL){
			MSHookFunction(mFun,(void*)&My_dexFileParse,(void**)&_dexFileParse);
		}
		//
		mFun = MSFindSymbol(image,dvmDexFileOpenFromFd);
		if(mFun != NULL){
//			MSHookFunction(mFun,(void*)&My_dvmDexFileOpenFromFd,(void**)&_dvmDexFileOpenFromFd);
		}
		//
		mFun = MSFindSymbol(image,dvmDexFileOpenPartial);
		if(mFun != NULL){
//			MSHookFunction(mFun,(void*)&My_dvmDexFileOpenPartial,(void**)&_dvmDexFileOpenPartial);
		}
	}
	return 0;
}





#endif
