#ifndef __DexTypes_H_
#define __DexTypes_H_
#include <stdio.h>
#include <unistd.h>
#include "DexFile/Dex.H"
#include "DexFile/LDex_Types_Header.H"
#include "DexMaps.H"
class Dex_Types : LDex_Util{
public:
	FastLink<typeItem> IDS;
	FastLink<typeList> List;
/*
************************************************************
*				LoadToFile
*1.老函数，将加载内存替换成现在保存的文件偏移内存
************************************************************
*/
	size_t LoadToFile(size_t inLoad){
		if(inLoad == 0)return 0;
		List.Reset();
		while(List.Get() != NULL){
			if(inLoad == List.Get()->BS.Offset_Load){
				return List.Get()->BS.Offset_File;
			}
			List.Next();
		}
		return inLoad;
	}
/*
************************************************************
*				LoadToFile
*
************************************************************
*/
	size_t getIdxNoType(int inNo){
		typeItem* item =IDS.get(inNo);
		if(item == NULL)return NULL;
		return item->type;
	}
/*
************************************************************
*				getListCodeLen
*获取需要多少的Code空间*
************************************************************
*/
	size_t getCodeLen(){
		size_t mlen = 0;
		List.Reset();
		while(List.Get() != NULL){
			mlen += List.Get()->BS.MemSize;
			List.Next();
		}
		return mlen;
	}
/*
************************************************************
*				Dex_Types
*解析TypeItem和TypeList
*因为typeList在内存中，所以需要借助Base模块帮助其加载
*由于typeIds不存在分开存放，所以一次解析完，而且不需要保存偏移表加载地址、
*存放时会只需要将数据关联即可
*注意:Code长度可能不是Code说明长度，我们分析String 到\0结束
************************************************************
*/
	Dex_Types(DexFile* inDex,Dex_Maps* inmaps){
		//获取基本数据
		DexHeader*  pHeader = (DexHeader*)inDex->pHeader;
		mapsItem* _mapsItem =inmaps->getTypeItem(kDexTypeTypeIdItem);
		DEXLOG("[Parse]Dex_Types %08x %08x",inDex->pHeader->typeIdsOff,inDex->pHeader->typeIdsSize);
		//获取typeIds实际地址
		u4* mMemIds =(u4*)AddAddr2(inDex->pHeader,inDex->pHeader->typeIdsOff);
		//遍历typeIds
		for(int m_i = 0;m_i < inDex->pHeader->typeIdsSize;m_i++){			typeItem* _item = (typeItem*)Alloc(sizeof(typeItem));
			_item->type = *mMemIds++;
			IDS.Add(_item);
		}
		//查找Maps中是否存在TypeList
		_mapsItem = inmaps->getTypeItem(kDexTypeTypeList);
		if(NULL == _mapsItem){
			DEXLOG("[Parse]Dex_Types List不存在");
			return;
		}
		DEXLOG("[Parse]Dex_Types List %08x %08x",_mapsItem->offset, _mapsItem->size);
		//存在Type list，对Type list进行遍历
		u4* mMemList =(u4*)AddAddr2(inDex->pHeader, _mapsItem->offset);
		u4	mMemStart = (u4)mMemList;
		//遍历Type list
		typeList* _Codelist = NULL;
		for(int m_i = 0;m_i < _mapsItem->size;m_i++){			typeList* _List =(typeList*)Alloc(sizeof(typeList));
			//设置偏移地址
			_List->BS.Offset_Load = SubAddr2(mMemList,inDex->pHeader);
			_List->Size = *mMemList;
			mMemList++;
			//遍历type Item,小型链接就不在分开处理
			FastLink<typeItem> mIDS;
			for(int m_j = 0;m_j < _List->Size;m_j++){
				typeItem* _Item = (typeItem*)Alloc(sizeof(typeItem));
				_Item->type = *(u2*)mMemList;
				mMemList = (u4*)AddAddr2(mMemList,2);
				mIDS.Add(_Item);
			}
			_List->item = mIDS.First;
			//设置内存大小
			mMemList = (u4*)AlignAdd((u4)mMemList,0x04);
			_List->BS.MemSize =SubAddr2(mMemList,mMemStart);
			mMemStart =(u4)mMemList;
			//将_item添加到列表中，AddToLast非常耗时
			List.Add(_List);
		}
	}
/*
************************************************************
*				IDStoFile
*将TydeIDS宽度放入Mod_Mem模块中,
*会实现里面数据存放(与StringIDS不同，因为typeIds中不存在于位置有关数据)
*但是List内容需要的DatatoFile中实现
*关联也在DatatoFile中实现
************************************************************
*/
	void IDStoFile(Mod_Mem* inMem,Dex_Header* inheader,Dex_Maps* inmaps){
		//判断是否存在 IDSString段，不存在就添加IDSString段
		str_ModMem* modIds =inMem->getByName("TypeIDS");
		if(modIds == NULL){
			modIds = inMem->newNameMem("TypeIDS",Link::GetCount(IDS.First)*4);
		}
		DEXLOG("[File]Type_Ids %08x %08x",modIds->Offset,modIds->Length);
		//修改maps偏移地址
		inmaps->getTypeItem(kDexTypeTypeIdItem)->offset = modIds->Offset;
		inmaps->getTypeItem(kDexTypeTypeIdItem)->size = Link::GetCount(IDS.First);
		inheader->Header->typeIdsOff = modIds->Offset;
		inheader->Header->typeIdsSize = Link::GetCount(IDS.First);
		//遍历设置Type_Ids
		u4* memIds = (u4*)modIds->Addr;
		IDS.Reset();
		while(IDS.Get() != NULL){
			*memIds = IDS.Get()->type;
			IDS.Next();
			memIds++;
		}
	}
/*
************************************************************
*				DatatoFile
*将StringCode按文件保存形式保存,这里应该在之前就保存好了StringIDS模块
*然后关联StringIDS和StringCode关联信息
************************************************************
*/
	void DatatoFile(Mod_Mem* inMem,Dex_Maps* inmaps){
		//判断是否存在 IDSString段，不存在就添加IDSString段
		str_ModMem* modList =inMem->getByName("TypeList");
		if(NULL == modList){
			modList = inMem->newNameMem("TypeList",AlignAdd(getCodeLen(),0x04));
		}
		DEXLOG("[File]TypeList %08x %08x",modList->Offset,modList->Length);
		//获取List数据
		str_ModMem* memCode = inMem->getByName("TypeList");
		//修改maps偏移地址
		inmaps->getTypeItem(kDexTypeTypeList)->offset = memCode->Offset;
		//设置数据
		u4* memList = (u4*)modList->Addr;
		u4 memStart= (u4)memList;
		List.Reset();
		//写全部List数据
		while(List.Get() != NULL){
			List.Get()->BS.Offset_File = modList->Offset+(u4)memList-memStart;
			*memList = List.Get()->Size;
			memList++;
			u2* memItem = (u2*)memList;
			typeItem* mitem = List.Get()->item;
			while(mitem != NULL){
				*memItem = mitem->type;
				mitem = mitem->next;
				memItem++;
			}
			memItem = (u2*)Ali(memItem,0x04);
			memList = (u4*)memItem;
			List.Next();
		}
	}
};
#endif

