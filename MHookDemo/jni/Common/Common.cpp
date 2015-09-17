#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include "HFile/NativeLog.h"
/*
************************************************************
* 获取进程名字
************************************************************
*/
char* getpidName(pid_t inpid){
	char proc_pid_path[1024];
	char buf[1024];
	char* outname = NULL;
	memset(buf,0,1024);
	memset(proc_pid_path,0,1024);
	sprintf(proc_pid_path, "/proc/%d/status", inpid);
	FILE* fp = fopen(proc_pid_path, "r");
	if(NULL != fp){
		if( fgets(buf, 1023, fp)== NULL ){
			fclose(fp);
		}
		fclose(fp);
		outname = (char*)malloc(1024);
		memset(outname,0,1024);
		sscanf(buf, "%*s %s", outname);
	}
	return outname;
}
/*
************************************************************
* 获取配置文件
************************************************************
*/
 char* getConfig(){
	FILE* fd =fopen("/sdcard/Config.txt","r");
	//判断文件是否打开成功*没有SD的程序不能成功
	char* mConfig = NULL;
	if(fd != NULL){
		char* mConfig = (char*)malloc(10240);
		memset(mConfig,0,10240);
		fgets(mConfig,10240,fd);
		fclose(fd);
		size_t mConfigSize = strlen(mConfig);
		if(mConfigSize == 0){
			free(mConfig);
			mConfig = NULL;
		}
	}

	if(mConfig != NULL)return mConfig;
	fd =fopen("/data/local/Config.txt","r");
	//判断文件是否打开成功*没有SD的程序不能成功
	if(fd != NULL){
		mConfig = (char*)malloc(10240);
		memset(mConfig,0,10240);
		fgets(mConfig,10240,fd);
		fclose(fd);
		size_t mConfigSize = strlen(mConfig);
		if(mConfigSize == 0){
			free(mConfig);
			mConfig = NULL;
		}
	}
	return mConfig;
}
/*
************************************************************
*				trim
************************************************************
*/
 void trim(char *s)
{
	int len = strlen(s);
	/* trim trailing whitespace */
	while ( len > 0 && isspace(s[len-1]))
	      s[--len]='\0';
	/* trim leading whitespace */
	memmove(s, &s[strspn(s, " \n\r\t\v")], len);
}
/*
************************************************************
* 获取</proc/self/status> 内容
************************************************************
*/
char* getSelfCmdline(){
	FILE* fd =fopen("/proc/self/status","r");
	if(fd == NULL)return NULL;
	char* mBuf = (char*)malloc(1024);	
	memset(mBuf,0,1024);
	fgets(mBuf,1024,fd);
	trim(mBuf);
	return mBuf;
}
/*
************************************************************
* 获取</proc/self/status> 名字呢看内容
************************************************************
*/
char* getSelfName(){
	FILE* fd =fopen("/proc/self/status","r");
	if(fd == NULL)return NULL;
	char* mBuf = (char*)malloc(1024);	
	memset(mBuf,0,1024);
	fgets(mBuf,1024,fd);
	if((strlen(mBuf) == 0)||(strstr(mBuf,":") == NULL)){
		free(mBuf);
		return NULL;
	}
	char* mOut = strdup(strstr(mBuf,":")+1);
	free(mBuf);
	trim(mOut);
	return mOut;
}
/*
************************************************************
* 在Self/maps中读取加载地址
************************************************************
*/
unsigned long getStartAddrByName(char* inName){
	FILE* fd =fopen("/proc/self/maps","r");
	if(NULL == fd)return 0;
	char* mBuf = (char*)malloc(1024);
	memset(mBuf,0,1024);
	unsigned long mStartAddr = 0;
	while(NULL != fgets(mBuf,1024,fd)){
		if(NULL != strstr(mBuf,inName)){
			mStartAddr = strtoul(mBuf,0,0x10);
			if(0 != mStartAddr)
				break;
		}
	}
	free(mBuf);
	return mStartAddr;
}
unsigned long getEndAddrByName(char* inName){
	FILE* fd =fopen("/proc/self/maps","r");
	if(NULL == fd)return 0;
	char* mBuf = (char*)malloc(1024);
	memset(mBuf,0,1024);
	unsigned long mEndAddr = 0;
	while(NULL != fgets(mBuf,1024,fd)){
		if(NULL != strstr(mBuf,inName)){
			char* pEnd = strstr(mBuf,"-")+1;
			mEndAddr = strtoul(pEnd,0,0x10);
			if(0 != mEndAddr)
				break;
		}
	}
	free(mBuf);
	return mEndAddr;
}
