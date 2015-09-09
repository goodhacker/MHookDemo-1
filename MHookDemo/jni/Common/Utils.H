#ifndef __Utils_H_
#define __Utils_H_
//获取LRS
long GETLR(){
	long mLR = 0;
	__asm__ __volatile__("mov %0, lr\n\t":  "=r"(mLR));
	return mLR;
}
//
char* jstringTostring(JNIEnv* env, jstring jstr)
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
#endif
