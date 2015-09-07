#ifndef __DexDebug_H_
#define __DexDebug_H_
#include <stdio.h>
#include <unistd.h>
#include "HFile/NativeLog.h"
#include "Dump_Dex.H"
#include "Module_Mem.H"
#include "DexUtil.H"
struct DebugItem{
	DebugItem* 	next;
	u4	param_name;
};
//说明Size_Reg不表示DebugItem长度
struct DebugList{
	DebugList* 	next;
	Base 	BS;
	struct DebugItem* Item;
	u4	number;
	u4	Size_Reg;
};
class Dex_Debug:DexUtil{
public:
	DebugList* debug = NULL;
/*
************************************************************
*				Dex_Debug
*设置Dex_Debug*
************************************************************
*/
	Dex_Debug(){
		debug = (DebugList*)Alloc(sizeof(DebugList));
	}
/*
************************************************************
*				Dex_Debug
*设置Dex_Debug*
************************************************************
*/
	Dex_Debug(DexFile* inDex,Dex_Maps* inMap){
		DEXLOG("[Parse]Dex_Debug");
		mapsItem* inMapsItem = inMap->getTypeItem(kDexTypeDebugInfoItem);
		mapsItem* nextMaps = inMap->getTypeItem(kDexTypeCodeItem);
		if(inMapsItem == NULL)return;
		u4	mMemStart =(u4)inDex->pHeader + inMapsItem->offset;
		u1* mMem =(u1*)mMemStart;
		DebugList* _debug = NULL;
		debug = (DebugList*)Alloc(sizeof(DebugList));
		debug->BS.SetLoad((u4)inDex->pHeader,inMapsItem->offset);
		debug->BS.MemSize = nextMaps->offset -inMapsItem->offset;
	/*	for(int m_i = 0;m_i < inMapsItem->size;m_i++){
			DebugList* _List = (DebugList*)Alloc(sizeof(DebugList));
			_List->BS.SetLoad((u4)inDex->pHeader,(u4)mMem);
			mMem = readLeb128(mMem,&_List->number);
			mMem = readLeb128(mMem,&_List->Size_Reg);
			DEXLOG("Dex_Debug %p %p",_List->number,_List->Size_Reg);
			//获取parameter name
			for(int m_j = 0;m_j < _List->Size_Reg;m_j++){
				u4 mData = 0;
				mMem = readLeb128(mMem,&mData);
			}
			bool isOver = false;
			u4 oldData = 0;
			do{
				u4 mData =*mMem++;
				if(mData == 0){
					if((oldData != 3)&&(oldData != 4)&&(oldData != 5)){
						isOver = true;
					}
				}
				if((oldData == 5)&&(mData == 5)){
					mData = 0;
				}
				oldData = mData;
			}while(!isOver);
			_List->BS.MemSize = SubAddr3(mMem,_List->BS.Offset_Load,inDex->pHeader);
			if(NULL == _debug){
				debug =  (DebugList*)AddToLast(debug,_List);
				_debug =  (DebugList*)GetLastLink(debug);
			}else{
				_debug =  (DebugList*)AddLink(_debug,_List);
			}
		}/**/
	}
/*
************************************************************
*				DatatoFile
*设置DatatoFile*
************************************************************
*/
	void DatatoFile(Mod_Mem* inMem,Dex_Maps* inmaps){
		//判断是否存在调试模块
		if(NULL == debug){
			DEXLOG("没有存在debug数据,直接退出!");
			return ;
		}
		//获取debug模块内存，没有就新建
		DEXLOG("Dex_Debug DatatoFile");
		str_ModMem* modList =inMem->getByName("DebugInfo");
		if(NULL == modList){
			modList = inMem->newNameMem("DebugInfo",AlignAdd(debug->BS.MemSize,0x04));
			DEXLOG("debug %08x %08x",modList->Offset,modList->Length);
		}
		memcpy(modList->Addr,(u1*)debug->BS.GetLoad(),debug->BS.MemSize);
		debug->BS.Offset_File = modList->Offset;
		return ;
		/*
		str_ModMem* modList =inMem->getByName("DebugInfo");
		if(NULL == modList){
			modList = inMem->newNameMem("DebugInfo",AlignAdd(GetSize(debug),0x04));
			DEXLOG("debug %08x %08x",modList->Offset,modList->Length);
		}
		//设置
		mapsItem* _mapItem = inmaps->getTypeItem(kDexTypeDebugInfoItem);
		_mapItem->offset =modList->Offset;
		//遍历设置
		U1*	mMem = modList->Addr;
		DebugList* _List =debug;
		while(NULL != _List){
			_List->BS.Offset_File = AddAddr2(modList->Offset,SubAddr2(mMem,modList->Addr));
			memcpy(mMem,(u1*)_List->BS.GetLoad(),_List->BS.MemSize);
			mMem += _List->BS.MemSize;
			/*mMem = writeL128(mMem,_List->number);
			mMem = writeL128(mMem,_List->Size_Reg);
			DebugItem* _Item = _List->Item;
			while(NULL != _Item){
				mMem =  writeL128(mMem,_Item->param_name);
				_Item = _Item->next;
			}
			_List = _List->next;
		}/**/
	}
/*
************************************************************
*				LoadToFile
*将加载地址改变为文件偏移地址
************************************************************
*/
	size_t LoadToFile(size_t inLoad){
		if(inLoad == 0)return 0;
		size_t mSize = inLoad - debug->BS.Offset_Load;
		mSize +=debug->BS.Offset_File;
		/*
		DebugList* mList = debug;
		while(mList != NULL){
			if(mList->BS.Offset_Load == inLoad){
				return mList->BS.Offset_File;
			}
			mList = mList->next;
		}/**/
		return mSize;
	}
};

#endif
