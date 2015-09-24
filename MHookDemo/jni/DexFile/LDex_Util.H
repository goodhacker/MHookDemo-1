#ifndef __LDex_Util_H_
#define __LDex_Util_H_
#include "Common/FastLink.H"
#define AddAddr2(arg0,arg1)		((u4)arg0+(u4)arg1)
#define SubAddr2(arg0,arg1)		((u4)arg0-(u4)arg1)
#define SubAddr3(arg0,arg1,arg2)		((u4)arg0-(u4)arg1-(u4)arg2)
#define Ali(arg0,arg1)			(((u4)arg0+((u4)arg1-1))&(~((u4)arg1-1)))
//

/*
************************************************************
*			DexUtil
*Dex通用类
************************************************************
*/
class LDex_Util{
public:
/*
************************************************************
*			NEXTLeb128
* @param	inAddr -> 数据起始地址
* @return	result -> 返回下一个数据地址
************************************************************
*/
static unsigned char* NEXTLeb128(void* inAddr){
	unsigned char* in_Addr =(unsigned char*)inAddr;
	int m_i;
	for(m_i = 0;m_i < 5;m_i++){
		if(*(in_Addr+m_i) <= 0x7f){
			return in_Addr+m_i+1;
		}
	}
	return in_Addr+m_i+1;
}
/*
************************************************************
*			readLeb128
* @param	inAddr ->	数据起始地址
* @return	result ->	读取readLeb128(unsigned leb128)数据
************************************************************
*/
static u4 readLeb128(void* inAddr){
	unsigned char* in_Addr = (unsigned char*)inAddr;
	u4 result = 0;
	for(int m_i = 0;m_i < 5;m_i++){
		u1 m_byte = *(in_Addr + m_i);
		result |= (m_byte&0x7F)<<(m_i*7);
		if(m_byte <= 0x7f){
			return result;
		}
	}
	return result;
}/*
************************************************************
*			readSLeb128
* @param	inAddr ->	数据起始地址
* @return	result ->	读取readLeb128(signed leb128)数据
************************************************************
*/
static u4 readSLeb128(unsigned char* inAddr){
	u4	result_2 = 0xFFFFFFFF;
	u4	result_1 = 0x00000000;
	for(int m_i = 0;m_i < 5;m_i++){
		char m_byte = *(inAddr + m_i);
		result_2 &= ~(0x7F<<(m_i*7));
		result_1 |= (m_byte&0x7F)<<(m_i*7);
		result_2 |= (m_byte&0x7F)<<(m_i*7);
		if((m_byte & 0x80) == 0)
		{
			if((m_byte & 0x40)){
				return result_2;
			}
			return result_1;
		}
	}
	return result_1;
}/*
************************************************************
*			readLeb128
* @param	inAddr ->	数据起始地址
* @param	inDeta ->	数据指针—存放数据
* @return	result ->	返回下一个数据地址
************************************************************
*/
static unsigned char* readLeb128(void* inAddr,u4* inDeta){
	*inDeta = readLeb128(inAddr);
	return NEXTLeb128(inAddr);
}/*
************************************************************
*			readSLeb128
* @param	inAddr ->	数据起始地址
* * @param	inDeta ->	数据指针—存放数据
* @return	result ->	返回下一个数据地址
************************************************************
*/
static unsigned char* readSLeb128(unsigned char* inAddr,u4* inDeta){
	*inDeta = readSLeb128(inAddr);
	return NEXTLeb128(inAddr);
}
/*
************************************************************
*			readLimitLeb128
* @param	inAddr ->	数据起始地址
* @return	result ->	返回Leb128数据宽度
************************************************************
*/
static u4 readLimitLeb128(unsigned char* inAddr){
	int m_i = 0;
	for(m_i = 0;m_i < 5;m_i++){
		if(*(inAddr+m_i) <= 0x7f){
			return  m_i+1;
		}
	}
	return  m_i+1;
}
/*
************************************************************
*			writeL128
* @param	inAddr ->	数据起始地址
* @param	mData ->	数据
* @return	result ->	返回Leb128数据宽度
************************************************************
*/
static unsigned char* writeL128(unsigned char* inAddr,u4 mData){
	int m_i;
	for(m_i = 0;m_i< 5;m_i++){
		u1 mByte =	mData &0x7F;
		mData >>= 7;
		if(mData != 0){
			*(inAddr+m_i) = mByte|0x80;
		}else{
			*(inAddr+m_i) = mByte&0x7F;
			return NEXTLeb128(inAddr);
		}
	}
	return NEXTLeb128(inAddr);
}
static unsigned char* writeSL128(unsigned char* inAddr,u4 mData){
	if((mData&0x80000000) == 0){
		writeL128(inAddr,mData);
	}else{
		unsigned int m_Data = 0xFFFFFFFF;
		int m_i;
		for(m_i = 0;m_i< 5;m_i++){
			u1 mByte = (mData>>(m_i*7));
			m_Data >>= 7;
			mData >>= 7;
			if((m_Data^mData) == 0){
				*(inAddr+m_i) = mByte&0x7F;
				return NEXTLeb128(inAddr);

			}else{
				*(inAddr+m_i) = mByte|0x80;
			}
		}
	}
	return NEXTLeb128(inAddr);
}
/*
************************************************************
*			writeAdd128_Limit
*会根据原先数据宽度设置数据
* @param	inAddr ->	数据起始地址
* @param	mData ->	数据
* @return	result ->	返回Leb128数据宽度
************************************************************
*/
static int writeAdd128_Limit(unsigned char* inAddr,u4 inData){
	u4 mLength = readLimitLeb128(inAddr);	for(int m_i = 0;m_i< mLength;m_i++){
		u1 mByte = (inData>>(m_i*7))&0x7F;
		if((m_i+1)!= mLength){
			*(inAddr+m_i) = mByte|0x80;
		}else{
			*(inAddr+m_i) = mByte;
		}
	}
	return 0;
}
/*
************************************************************
*			Alloc
*获取清零内存
* @param	inSize ->	内存宽度
* @return	out ->		内存指针
************************************************************
*/
	static void* Alloc(size_t inSize){
		void* out = malloc(inSize);
		memset(out,0,inSize);
		return out;
	}
/*
************************************************************
*			Align
*对齐设置
* @param	inAddr ->	结束地址
* @param	inAli ->	对齐宽度
* @return	out ->		对齐需要宽度
************************************************************
*/
	static size_t Align(size_t inAddr,size_t inAli){
		size_t mSzie = inAli - inAddr&(inAli-1);
		return  mSzie;
	}

/*
************************************************************
*			AlignAdd
*对齐设置
* @param	inAddr ->	结束地址
* @param	inAli ->	对齐宽度
* @return	out ->		对齐需要宽度
************************************************************
*/
	static size_t AlignAdd(size_t inAddr,size_t inAli){
		size_t mSzie = (inAddr+inAli-1)&(~(inAli-1));
		return  mSzie;
	}
/*
************************************************************
*			isDex
*判断是否是Dex文件
* @param	inAddr ->	Dex起始地址
* @return	bool ->		0->不是Dex或ODex文件
* 						1->存在Dex或ODex的Magic
************************************************************
*/
	static bool isDex(void* inAddr){
		if((0 == memcmp(inAddr,"dex",3))||(0 == memcmp(inAddr,"dey",3))){
			return 1;
		}
		return 0;
	}
/*
************************************************************
*			SaveFile
*将内存数据保存成文件
* @param	addr ->		内存起始地址
* @param	inLen->		内存宽度
* @param	appName ->	应用名字
* @param	inFile ->	文件名字
************************************************************
*/
	static void SaveFile(void* addr,size_t inLen,char* appName,char* inFile){
		char* filePath = (char*)malloc(1024);
		memset(filePath,0,1024);
		sprintf(filePath,"/sdcard/DumpDex/%s",inFile);
		FILE* mfd=fopen(filePath,"wb");
		//查找可写文件路径
		if(!mfd){
//			DEXLOG("[Dump]Fail SD Path:%s",filePath);
//			DEXLOG("[Dump]Mod_Mem::SaveFile() SD Open file fail!");
			memset(filePath,0,1024);
			if(appName != NULL){
				sprintf(filePath,"/data/data/%s/%s",appName,inFile);
				mfd=fopen(filePath,"wb");
				if(!mfd){
//					DEXLOG("[Dump]Fail Data_Data Path:%s",filePath);
//					DEXLOG("[Dump]Mod_Mem::SaveFile() Data Open file fail!");
					free(filePath);
					return;
				}
			}else{
//				DEXLOG("[Dump]Fail appName Is Null");
				free(filePath);
				return ;
			}
		}
		fwrite(addr,1,inLen,mfd);
		fclose(mfd);
//		DEXLOG("[Dump]Dump File Path:%s",filePath);
		free(filePath);
	}
/*
************************************************************
*			GetTimeName
*获取实时时间名字
* @param	inName ->	名字前缀
************************************************************
*/
	static char*  GetTimeName(char* inName){
		struct timeval tv;
		gettimeofday(&tv,NULL);
		int m_HookM = (tv.tv_sec/60)%60;
		int m_HookH = (tv.tv_sec/3600)%24;
		char* mName = (char*)Alloc(1024);
		sprintf(mName,"%s_%02d_%02d",inName,m_HookH,m_HookM);
		return mName;
	}
/*
************************************************************
*				Dex_Header
*解析头文件
************************************************************
*/
	static void* GetBase(void* inAddr){
		u1* mAddr =(u1*)inAddr;
		if(0 == memcmp(mAddr,"dey",3)){
			return mAddr+0x28;
		}else if(0 == memcmp(mAddr,"dex",3)){
			return mAddr;
		}
		return NULL;
	}
};
#endif
