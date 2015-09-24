#ifndef __DexString_H_
#define __DexString_H_
#include <stdio.h>
#include <unistd.h>
#include "DexFile/Dex.H"#include "DexMaps.H"
#include "DexFile/LDex_String_Header.H"
namespace Dex{
//字符串保存列表
class Dex_String : LDex_Util{
#define STR_StringIds		"StringIDS"
#define STR_StringCode		"StringCode"
public:
	FastLink<stringItem> IDS;
/*
************************************************************
*				getCodeLen
*1.获取String Data 数据宽度
************************************************************
*/
	size_t getIDSNo(char* pstr){
		IDS.Reset();
		size_t m_No = 0;
		while(IDS.Get() != NULL){
			if(strcmp(IDS.Get()->str,pstr) == 0){
				return m_No;
			}
			m_No++;
			IDS.Next();
		}

		return 0;
	}
	char* getString(int inNo){
		stringItem* item =IDS.get(inNo);
		if(item == NULL)return NULL;
		return item->str;
	}
/*
************************************************************
*				getCodeLen
*1.获取String Data 数据宽度
************************************************************
*/
	size_t getCodeLen(){
		size_t mSzie = NULL;
		IDS.Reset();
		while(IDS.Get() != NULL){
			mSzie += IDS.Get()->size;
			IDS.Next();
		}
		DEXLOG("Dex_String@getCodeLen:0x%08x",mSzie);
		return mSzie;
	}
/*
************************************************************
*				CreateIdsMemory
*创建保存IDS的内存空间*
************************************************************
*/
	void CreateIdsMemory(Mod_Mem* inMem){
		//判断原先是否存在
		if(NULL == inMem->getByName(STR_StringIds)){
			str_ModMem* modIds = inMem->newNameMem(STR_StringIds,Link::GetCount(IDS.First)*4);
			DEXLOG("[Create]String_Ids %08x %08x",modIds->Offset,modIds->Length);
		}
	}
/*
************************************************************
*				Dex_String
*解析StringIDS和StringCode
*将数据保存在Idsitem中(Code也是)
*由于StringIDS不存在分开存放，所以一次解析完，而且不需要保存偏移表加载地址、
*存放时会只需要将数据关联即可
*注意:Code长度可能不是Code说明长度，我们分析String 到\0结束
*Header里面数据不是十分可信，所以使用Maps里面数据
************************************************************
*/
	Dex_String(DexFile* inDex,Dex_Maps* inmaps){
		//获取基本数据
		DexHeader*  pHeader = (DexHeader*)inDex->pHeader;
		mapsItem* _mapsItem =inmaps->getTypeItem(kDexTypeStringIdItem);
		DEXLOG("[Parse]Dex_String %08x %08x",pHeader->stringIdsOff,pHeader->stringIdsSize);
		//获取IDS实际内存地址
		u4* mMemIds =(u4*)AddAddr2(pHeader,pHeader->stringIdsOff);
		//遍历字符串
		for(int m_i = 0;m_i < pHeader->stringIdsSize;m_i++){			stringItem* _item =(stringItem*)Alloc(sizeof(stringItem));
			//获取字符串Code 起始地址
			_item->BS.SetLoad((u4)pHeader,(u4)mMemIds);
			u1* mMemCode =(u1*)AddAddr2(pHeader,*mMemIds);
			_item->L128Code= readLeb128(mMemCode);
			_item->L128Len = readLimitLeb128(mMemCode);
			mMemCode = NEXTLeb128(mMemCode);
			_item->strLen= strlen((char*)mMemCode);
			//将长度最大的设置为字符串长度
			if(_item->L128Code > _item->strLen){
				_item->strLen = _item->L128Code;
			}
			//不申请内存存放字符串，直接指向DexFile文件内存
			_item->str = (char*)mMemCode;
			_item->size = _item->strLen + _item->L128Len + 1;
			//将字符串添加到列表中，AddToLast非常耗时
			IDS.Add(_item);
			//指向下一个字符串
			mMemIds++;
		}
	}
/*
************************************************************
*				IDStoFile
*将字符串IDS宽度设置字符串，
*但是字符串内容需要的DatatoFile中实现
*关联也在DatatoFile中实现
************************************************************
*/
	void IDStoFile(Mod_Mem* inMem,Dex_Header* inheader,Dex_Maps* inmaps){
		//判断是否存在 IDSString段，不存在就添加IDSString段
		str_ModMem* modIds =inMem->getByName("StringIDS");
		if(modIds== NULL){
			modIds = inMem->newNameMem("StringIDS",Link::GetCount(IDS.First)*4);
		}
		DEXLOG("[File]String_Ids %08x %08x",modIds->Offset,modIds->Length);
		//修改maps偏移地址
		inmaps->getTypeItem(kDexTypeStringIdItem)->offset = modIds->Offset;
		inmaps->getTypeItem(kDexTypeStringIdItem)->size = Link::GetCount(IDS.First);
		inheader->Header->stringIdsSize = Link::GetCount(IDS.First);
	}
/*
************************************************************
*				DatatoFile
*将StringCode按文件保存形式保存,这里应该在之前就保存好了StringIDS模块
*然后关联StringIDS和StringCode关联信息
************************************************************
*/
	void DatatoFile(Mod_Mem* inMem,Dex_Maps* inmaps){
		DEXLOG("Dex_String DatatoFile");
		//判断是否存在 IDSString段，不存在就添加IDSString段
		str_ModMem* modList = inMem->getByName("StringCode");
		if(NULL == modList){
			modList = inMem->newNameMem("StringCode",AlignAdd(getCodeLen(),0x100));
			DEXLOG("Dex_String DatatoFile %08x %08x %08x",modList->Addr,modList->Length,modList->Offset);
		}
		//获取需要关联的两个str_ModMem段(StringIDS和StringCode)
		str_ModMem* memIds = inMem->getByName("StringIDS");
		str_ModMem* memCode = inMem->getByName("StringCode");
		//修改maps偏移地址
		inmaps->setTypeOffset(kDexTypeStringDataItem,memCode->Offset);
		//填充数据
		u4* idsAddr =(u4*)memIds->Addr;
		u1* CodeAddr = (u1*)memCode->Addr;
		IDS.Reset();
		size_t offset = 0;
		while(IDS.Get() != NULL){
			//设置IDS数据指向Code偏移内存
			*idsAddr = (u4)CodeAddr +offset- (u4)memCode->Addr+memCode->Offset;
			idsAddr++;
			//将数据写到Code偏移内存
			writeL128(CodeAddr+offset,IDS.Get()->L128Code);
			memcpy(CodeAddr + offset + IDS.Get()->L128Len,IDS.Get()->str,IDS.Get()->strLen);
			//设置下一个数据
			offset += IDS.Get()->size;
			IDS.Next();
		}
	}
};};
#endif
