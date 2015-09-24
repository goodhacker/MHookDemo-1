#ifndef __DexMethods_H_
#define __DexMethods_H_
#include <stdio.h>
#include <unistd.h>
#include "DexFile/Dex.H"
#include "DexFile/LDex_Method_Header.H"
class Dex_Methods : LDex_Util{
public:
	FastLink<MethodsItem> IDS;
/*
************************************************************
*				Dex_Methods
*解析Methods
*
************************************************************
*/
	Dex_Methods(DexFile* inDex,Dex_Maps* inmaps){
		//获取基本数据
		DexHeader*  pHeader = (DexHeader*)inDex->pHeader;
		mapsItem* _mapsItem =inmaps->getTypeItem(kDexTypeMethodIdItem);
		DEXLOG("[Parse]Dex_Methods %08x %08x",pHeader->methodIdsOff,pHeader->methodIdsSize);
		//根据MethodIds的文件结构，设置文件头MethodIds
		DexMethodId* MethodIds =(DexMethodId*)AddAddr2(pHeader,pHeader->methodIdsOff);
		//遍历MethodIds
		MethodsItem* _Idsitem = NULL;
		for(int m_i = 0;m_i < pHeader->methodIdsSize;m_i++){			MethodsItem* _item =(MethodsItem*)Alloc(sizeof(MethodsItem));
			_item->classIdx = MethodIds->classIdx;
			_item->protoIdx = MethodIds->protoIdx;
			_item->nameIdx = MethodIds->nameIdx;
			//指向下一个数据、将数据添加到链表
			MethodIds++;
			IDS.Add(_item);
		}
	}
/*
************************************************************
*				IDStoFile
*将FiledsIDS宽度放入Mod_Mem模块中,并设置数据
************************************************************
*/
	void IDStoFile(Mod_Mem* inMem,Dex_Header* inheader,Dex_Maps* inmaps){
		//判断是否存在 IDSString段，不存在就添加IDSString段
		str_ModMem* modIds =inMem->getByName("MethodsIDS");
		if(modIds== NULL){
			modIds = inMem->newNameMem("MethodsIDS",IDS.GetSize()*8);
		}
		DEXLOG("[File]Methods_Ids %08x %08x",modIds->Offset,modIds->Length);
		//修改maps偏移地址
		inmaps->getTypeItem(kDexTypeMethodIdItem)->offset = modIds->Offset;
		inmaps->getTypeItem(kDexTypeMethodIdItem)->size = IDS.GetSize();
		inheader->Header->methodIdsOff = modIds->Offset;
		inheader->Header->methodIdsSize = IDS.GetSize();
		//设置Fileds IDS数据
		IDS.Reset();
		DexMethodId* mID = (DexMethodId*)modIds->Addr;
		//设置数据
		while(IDS.Get() != NULL){
			mID->classIdx = IDS.Get()->classIdx;
			mID->protoIdx = IDS.Get()->protoIdx;
			mID->nameIdx = IDS.Get()->nameIdx;
			mID++;
			IDS.Next();
		}
	}
};


#endif
