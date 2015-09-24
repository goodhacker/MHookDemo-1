/*
 * LDex_Filed.H
 *
 *  Created on: 2015年9月23日
 *      Author: Ls Mouse
 */
#ifndef LDEX_FILED_H_
#define LDEX_FILED_H_
#include <stdio.h>
#include <unistd.h>
#include "Dex.H"
#include "LDex_Filed_Header.H"
#include "LDex_Header.H"
#include "LDex_Map.H"

class LDex_Filed:LDex_Util{
public:
	FastLink<FieldsItem> IDS;
/*
************************************************************
*				LDex_Filed
*解析Fileds
************************************************************
*/
	LDex_Filed(DexFile* inDex,LDex_Map* inmaps){
		//获取基本数据
		mapsItem*	_mapsItem =inmaps->getTypeItem(kDexTypeFieldIdItem);
		DexHeader*  pHeader = (DexHeader*)inDex->pHeader;
		DEXLOG("[Parse]Dex_Fileds %08x %08x",pHeader->fieldIdsOff,pHeader->fieldIdsSize);
		//按照Fileds保存方式设置头，然后按结构体读取
		DexFieldId* fieldIds =(DexFieldId*)AddAddr2(pHeader,pHeader->fieldIdsOff);
		//遍历添加FieldsItem
		for(int m_i = 0;m_i < pHeader->fieldIdsSize;m_i++){
			FieldsItem* _item = (FieldsItem*)Alloc(sizeof(FieldsItem));
			_item->classIdx = fieldIds->classIdx;
			_item->typeIdx = fieldIds->typeIdx;
			_item->nameIdx = fieldIds->nameIdx;
			fieldIds++;
			//将_item添加到列表中，AddToLast非常耗时
			IDS.Add(_item);
		}
	}
/*
************************************************************
*				IDStoFile
*将FiledsIDS宽度放入Mod_Mem模块中,并设置数据
************************************************************
*/
	void IDStoFile(Mod_Mem* inMem,LDex_Header* inheader,LDex_Map* inmaps){
		//判断是否存在 IDSString段，不存在就添加IDSString段
		str_ModMem* modIds =inMem->getByName("FiledsIDS");
		if(modIds== NULL){
			modIds = inMem->newNameMem("FiledsIDS",IDS.GetSize()*8);
		}
		DEXLOG("[File]Fileds_Ids %08x %08x",modIds->Offset,modIds->Length);
		//修改maps偏移地址
		inmaps->getTypeItem(kDexTypeFieldIdItem)->offset = modIds->Offset;
		inmaps->getTypeItem(kDexTypeFieldIdItem)->size = IDS.GetSize();
		inheader->Header->fieldIdsOff = modIds->Offset;
		inheader->Header->fieldIdsSize = IDS.GetSize();
		//设置Fileds IDS数据
		IDS.Reset();
		DexFieldId* mID = (DexFieldId*)modIds->Addr;
		while(IDS.Get() != NULL){
			mID->classIdx = IDS.Get()->classIdx;
			mID->typeIdx = IDS.Get()->typeIdx;
			mID->nameIdx = IDS.Get()->nameIdx;
			mID++;
			IDS.Next();
		}
	}
};
#endif
/* LDEX_FILED_H_ */
