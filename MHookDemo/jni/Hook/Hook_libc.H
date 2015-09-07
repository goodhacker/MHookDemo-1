#ifndef __Hook_LIBC_H_
#define __Hook_LIBC_H_
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Substrate.h"
#include "HFile/NativeLog.h"
//libc
#define str_libc			"/system/lib/libc.so"
#define str_open			"open"
#define str_fopen			"fopen"
#define str_mmap			"mmap"
#define str_ptrace			"ptrace"
#define str_free			"free"
#define str_munmap			"munmap"
//
//open	int open(const char*pathname,int flags);
void* (*_open)(const char * path,int flags);
void* My_open(const char * path,int flags){
	LOGD("LibCall open Path:%s,Mode:%d",path,flags);
	return _open(path,flags);
}
//fopen
void* (*_fopen)(const char * path,const char * mode);
void* My_fopen(const char * path,const char * mode){
	LOGD("LibCall fopen Path:%s,Mode:%s",path,mode);
	return _fopen(path,mode);
}
//mmap
void*(*_mmap)(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void* My_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset){
	void* Result =_mmap(addr,len,prot,flags,fd,offset);
	LOGD("LibCall _mmap %08x %08x %08x %08x %08x %08x",addr,len,prot,flags,fd,offset);
	LOGD("LibCall _mmap Result %08x ",Result);
	return Result;
}
//ptrace
int (*_ptrace)(int request, int pid, int addr, int data);
int My_ptrace(int request, int pid, int addr, int data){
	int mres = _ptrace(request,pid,addr,data);
	LOGD("LibCall _ptrace in %08x %08x %08x %08x %08x %08x",request,pid,addr,data);
	LOGD("LibCall _ptrace res %08x ",mres);
	return mres;
}
//free
void (*_free)(void *ptr);
void My_free(void *ptr){
	LOGD("LibCall _free %08x",ptr);
	free(ptr);
}
// int munmap(void *start,size_t length);
int (*_munmap)(void *start,size_t length);
int My_munmap(void *start,size_t length){
	LOGD("LibCall _munmap %08x %08x",start,length);
	return _munmap(start,length);
}
/**
 *
 */
	int Hook_Libc(){
		MSImageRef image = MSGetImageByName(str_libc);
		if(image != NULL){
			void* mFun = MSFindSymbol(image,str_open);
			if(mFun != NULL){
				MSHookFunction(mFun,(void*)&My_open,(void**)&_open);
			}
			mFun = MSFindSymbol(image,str_fopen);
			if(mFun != NULL){
				MSHookFunction(mFun,(void*)&My_fopen,(void**)&_fopen);
			}
			mFun = MSFindSymbol(image,str_ptrace);
			if(mFun != NULL){
				MSHookFunction(mFun,(void*)&My_ptrace,(void**)&_ptrace);
			}
			mFun = MSFindSymbol(image,str_free);
			if(mFun != NULL){
			//	MSHookFunction(mFun,(void*)&My_free,(void**)&_free);
			}
			mFun = MSFindSymbol(image,str_munmap);
			if(mFun != NULL){
				MSHookFunction(mFun,(void*)&My_munmap,(void**)&_munmap);
			}
		}
		return 0;
	}
#endif
