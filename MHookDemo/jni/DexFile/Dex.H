/*
 * Dex.H
 *
 *  Created on: 2015年9月23日
 *      Author: Ls Mouse
 */
#ifndef DEX_H_
#define DEX_H_

#include "DexFile/Dex_SystemDefine.H"
#include <android/log.h>
#define DEXLOG(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "LsSub_DEX", __VA_ARGS__))
//
#include "DexFile/LDex_Util.H"
#include "Common/FastLink.H"
#include "L_IO/Module_Mem.H"
//用于新建线程，传递数据
typedef struct DumpInfo{
	void* addr;
	size_t len;
	DexFile* Dex;
	void* BackOldDex;
	void* DexParse;
}DumpInfo;

/* DEX_H_ */
#endif
/* DEX_H_ */
