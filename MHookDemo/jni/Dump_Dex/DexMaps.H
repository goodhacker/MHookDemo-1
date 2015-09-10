#ifndef __DexMaps_H_
#define __DexMaps_H_
#include <stdio.h>
#include <unistd.h>
#include "HFile/NativeLog.h"
#include "Dump_Dex.H"
#include "Module_Mem.H" 
#include "DexUtil.H"
//__________________________________________________________
typedef struct mapsItem{
	struct mapsItem* next;
	u4	type;       		 /*Section type*/
 	u4	unused;				/*unused*/
	u4	size;        		/* section size*/
	u4	offset;     		/* section offset */
}mapsItem;
//
typedef struct maplist{
	struct mapsItem* item;
	size_t mapSize;
}maplist;
/*
************************************************************
*				Dex_Maps*
************************************************************
*/
class Dex_Maps : DexUtil{
public:
	FastLink<mapsItem> IDS;
/*
************************************************************
*				Log_Maps
*传入地址为Maps起始地址
************************************************************
*/
	static void Log(void* inAddr){
		DEXLOG("Dex_Maps Log Addr:%08x",inAddr);
		u4	mSize = *(u4*)inAddr;
		DexMapId* MapsIds = (DexMapId*)((u1*)inAddr+4);
		DEXLOG("Maps Size :%08x",mSize);
		for(int m_i = 0;m_i < mSize;m_i++){
			DEXLOG("Item :%04x %08x %08x",MapsIds->type,MapsIds->size,MapsIds->offset);
			MapsIds++;
		}
	}
/*
************************************************************
*				Log_Addr
*传入地址为Dex起始地址
************************************************************
*/
	static void Log_Dex(void* inDex){
		DexHeader* pHeader = (DexHeader*)GetBase(inDex);
		Log(pHeader->mapOff+(u1*)pHeader);
	}
/*
************************************************************
*				Dex_Maps
*解析Maps*
************************************************************
*/
	Dex_Maps(DexFile* inDex){
		DEXLOG("[Parse]Dex_Maps");
		//获取起始地址和初始化Map结构
		u1*	mapStart =(u1*)AddAddr2(inDex->pHeader,inDex->pHeader->mapOff);
		//获取Map个数,设置Maps Item 结构起始地址
		Log(mapStart);
		size_t mapSize = *((u4*)mapStart);
		//Map->mapSize = 0x12;
		DexMapId* MapsIds =(DexMapId*)(mapStart+4);
		//遍历Map内容
		for(int m_i = 0;m_i < mapSize;m_i++){
			mapsItem* _item = (mapsItem*)Alloc(sizeof(mapsItem));
			_item->type = MapsIds->type;
			_item->unused = MapsIds->unused;
			_item->size = MapsIds->size;
			_item->offset = MapsIds->offset;
			MapsIds++;
			IDS.Add(_item);
		}
	}
/*
************************************************************
*				getNextOffset
*获取下一个偏移
*	这里指最小大于此Offset
************************************************************
*/
	u4	getNextOffset(u4 inOffset){
		size_t m_offset = 0xFFFFFFFF;
		IDS.Reset();
		while(IDS.Get() != NULL){
			if(IDS.Get()->offset > inOffset){
				if(m_offset > IDS.Get()->offset){
					m_offset = IDS.Get()->offset;
				}
			}
			IDS.Next();
		}
		if(m_offset == 0xFFFFFFFF)return 0;
		return m_offset;
	}
/*
************************************************************
*				getTypeOffset
*获取子模块结构体
************************************************************
*/
	mapsItem* getTypeItem(u2 in_type){
		IDS.Reset();
		while(IDS.Get() != NULL){
			if(IDS.Get()->type == in_type){
				return IDS.Get();
			}
			IDS.Next();
		}
		return NULL;
	}
/*
************************************************************
*				setTypeSize
*设置子模块偏移地址
************************************************************
*/
	void setTypeSize(u2 in_type,u4 inSize){
		IDS.Reset();
		while(IDS.Get()  != NULL){
			if(IDS.Get() ->type == in_type){
				IDS.Get() ->size = inSize;
				return ;
			}
			IDS.Next();
		}
		mapsItem* mItem = (mapsItem*)Alloc(sizeof(mapsItem));
		mItem->type = in_type;
		mItem->size = inSize;
		IDS.Add(mItem);
	}
/*
************************************************************
*				setTypeOffset
*设置子模块偏移地址
************************************************************
*/
	void setTypeOffset(u2 in_type,u4 inoffset){
		IDS.Reset();
		while(IDS.Get()  != NULL){
			if(IDS.Get() ->type == in_type){
				IDS.Get() ->offset = inoffset;
				return ;
			}
			IDS.Next();
		}
		mapsItem* mItem = (mapsItem*)Alloc(sizeof(mapsItem));
		mItem->type = in_type;
		mItem->offset = inoffset;
		IDS.Add(mItem);
	}
/*
************************************************************
*				IDStoFile
*这里只是获取内存宽度，不设置数据
************************************************************
*/
	void IDStoFile(Mod_Mem* inMem,	Dex_Header* inheader){
		//判断是否存在 IDSString段，不存在就添加IDSString段
		DEXLOG("Dex_Maps IDStoFile");
		str_ModMem* modIds =inMem->getByName("MapsIDS");
		if(modIds == NULL){
			modIds = inMem->newNameMem("MapsIDS",IDS.GetSize()*0xC+4);
			DEXLOG("Dex_Maps DatatoFile %08x %08x %08x",modIds->Addr,modIds->Length,modIds->Offset);
		}
		//设置头系统文件中的数据
		getTypeItem(kDexTypeMapList)->offset = modIds->Offset;
		getTypeItem(kDexTypeMapList)->size = IDS.GetSize();
		inheader->Header->mapOff =modIds->Offset;
		//遍历设置数据
		*((u4*)modIds->Addr) =IDS.GetSize();
		IDS.Reset();
		DexMapId* mapIds =(DexMapId*)(modIds->Addr+4);
		while(IDS.Get() != NULL){
			mapIds->type = IDS.Get()->type;
			mapIds->unused = IDS.Get()->unused;
			mapIds->size = IDS.Get()->size;
			mapIds->offset = IDS.Get()->offset;
			mapIds++;
			IDS.Next();
		}
	}
};
#endif
