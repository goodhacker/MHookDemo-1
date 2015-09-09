#ifndef __Baidu_Shell_H_
#define __Baidu_Shell_H_
#include "Dump_Dex/DexParse.H"
using namespace Dex;
extern JavaVM* GVM;
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
	jstring msrt = pcharToJstring(env,pstr,NULL);
	$BaidDu_A_d(env,NULL,msrt);
}
//解密全部函数
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
//___________________________________________________________________________________________________________________
/**
 *BeforeParse_Baidu	->系统解析之前调用
 *初始化加载地址和加载宽度
 *至于解析还是放到系统解析完毕后在解析
 */
void BeforeParse_Baidu(void* inAddr,size_t inLen){
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
	BaiduDex->dex = (DexFile*)inDex;
	//解密时候数据分段必须非常犀利,不能额报错产生
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
				//恢复代码，低级恢复
				RestoreCode(env);
			}
		}
	}else{
		LOGD("GVM is null");
	}
}
#endif
