/*
 * LDex_Proto.H
 *
 *  Created on: 2015年9月23日
 *      Author: Ls Mouse
 */
#ifndef LDEX_PROTO_H_
#define LDEX_PROTO_H_
#include <stdio.h>
#include <unistd.h>
#include "Dex.H"
#include "LDex_Proto_Header.H"
#include "LDex_Map.H"
class LDex_Proto : LDex_Util{
public:
	FastLink<ProtoItem> IDS;
/*
************************************************************
*				LDex_Proto
*解析LDex_Proto
*Dex_Proto需要type_list数据，我们解析时只保存加载数据
*后续将加载数据替换成文件偏移数据就OK了
************************************************************
*/
	LDex_Proto(DexFile* inDex,LDex_Map* inmaps){
		//获取基本数据
		DexHeader*  pHeader = (DexHeader*)inDex->pHeader;
		mapsItem* _mapsItem =inmaps->getTypeItem(kDexTypeProtoIdItem);
		DEXLOG("[Parse]Dex_Proto %08x %08x",pHeader->protoIdsOff,pHeader->protoIdsSize);
		//在文件中按结构体很好的保存，设置起始按结构体读取
		DexProtoId* protoItem =(DexProtoId*)AddAddr2(pHeader,pHeader->protoIdsOff);
		//遍历ProtoItem
		ProtoItem* _Idsitem = NULL;
		for(int m_i = 0;m_i < pHeader->protoIdsSize;m_i++){
			ProtoItem* _item =(ProtoItem*)Alloc(sizeof(ProtoItem));
			_item->shortyIdx = protoItem->shortyIdx;
			_item->returnTypeIdx = protoItem->returnTypeIdx;
			_item->parametersOff = protoItem->parametersOff;
			protoItem++;
			IDS.Add(_item);
		}
	}
/*
************************************************************
*				IDStoFile
*将ProtoIDS宽度放入Mod_Mem模块中,
************************************************************
*/
	void IDStoFile(Mod_Mem* inMem,LDex_Header* inheader,LDex_Map* inmaps){
		//判断是否存在 IDSString段，不存在就添加IDSString段
		str_ModMem* modIds =inMem->getByName("ProtoIDS");
		if(modIds== NULL){
			modIds = inMem->newNameMem("ProtoIDS",IDS.GetSize()*0xC);
		}
		DEXLOG("[File]Proto_Ids %08x %08x",modIds->Offset,modIds->Length);
		//修改maps偏移地址
		inmaps->getTypeItem(kDexTypeProtoIdItem)->offset = modIds->Offset;
		inmaps->getTypeItem(kDexTypeProtoIdItem)->size = IDS.GetSize();
		inheader->Header->protoIdsOff = modIds->Offset;
		inheader->Header->protoIdsSize = IDS.GetSize();
	}
/*
************************************************************
*				DatatoFile
*主要设置根据Type list 地址设置偏移
************************************************************
*/
	void DatatoFile(Mod_Mem* inMem,LDex_Header* inheader,LDex_Map* inmaps,LDex_Types* inType){
		//判断是否存在 IDSString段，不存在就添加IDSString段
		DEXLOG("[File]Dex_Proto");
		str_ModMem* modIds =inMem->getByName("ProtoIDS");
		if(modIds == NULL){
			modIds = inMem->newNameMem("ProtoIDS",IDS.GetSize()*12);
			DEXLOG("[File]Proto Ids %08x %08x",modIds->Offset,modIds->Length);
		}
		//修改maps偏移地址
		inmaps->getTypeItem(kDexTypeProtoIdItem)->offset = modIds->Offset;
		inmaps->getTypeItem(kDexTypeProtoIdItem)->size = IDS.GetSize();
		inheader->Header->protoIdsOff = modIds->Offset;
		inheader->Header->protoIdsSize =IDS.GetSize();
		DEXLOG("[File]Dex_Proto Code");
		//设置Proto IDS数据
		IDS.Reset();
		DexProtoId* mID = (DexProtoId*)modIds->Addr;
		while(IDS.Get() != NULL){
			mID->shortyIdx = IDS.Get()->shortyIdx;
			mID->returnTypeIdx = IDS.Get()->returnTypeIdx;
			mID->parametersOff = inType->LoadToFile(IDS.Get()->parametersOff);
			IDS.Next();
			mID++;
		}
	}
};
#endif
/* LDEX_PROTO_H_ */
