#ifndef __DexClassDef_H_
#define __DexClassDef_H_
#include <stdio.h>
#include <unistd.h>
#include "DexFile/Dex.H"
#include "DexTypes.H"
#include "Dex_Encoded.H"
#include "Dex_Annot.H"
#include "DexFile/LDex_Class_Header.H"
namespace Dex{
class Dex_Class : LDex_Util{
public:
	FastLink<ClassDefItem>	ClassDef;
	FastLink<DexClassData>	ClassData;
	FastLink<DexCodeL> 	ClassCode;
/**
 *初始化Dex_Class
 *没有实现任何功能
 */
	Dex_Class(){}
/*
************************************************************
*				Dex_ClassDef
*解析Class Define*
************************************************************
*/
	void Dex_ClassDef(DexFile* inDex,Dex_Maps* inmaps){
		//获取基本数据
		DexHeader*	pHeader = (DexHeader*)inDex->pHeader;
		mapsItem*	_mapsItem =inmaps->getTypeItem(kDexTypeClassDefItem);
		DEXLOG("[Parse]Dex_ClassDef %p %p",pHeader->classDefsOff,pHeader->classDefsSize);
		//根据DexClassDef的文件结构，设置文件头DexClassDef
		DexClassDef* Item =(DexClassDef*)AddAddr2(pHeader,pHeader->classDefsOff);
		//遍历ClassDefItem
		for(int m_i = 0;m_i < pHeader->classDefsSize;m_i++){
			ClassDefItem* _item =(ClassDefItem*)Alloc(sizeof(ClassDefItem));
			_item->classIdx = Item->classIdx;
			_item->accessFlags = Item->accessFlags;
			_item->superclassIdx = Item->superclassIdx;
			_item->interfacesOff = Item->interfacesOff;
			_item->sourceFileIdx = Item->sourceFileIdx;
			_item->annotationsOff = Item->annotationsOff;
			_item->classDataOff = Item->classDataOff;
			_item->staticValuesOff = Item->staticValuesOff;
			//指向下一个ClassDefItem数据
			Item++;
			ClassDef.Add(_item);
		}
	}
/*
************************************************************
*				IDStoFile
*将Dex_ClassDef宽度放入Mod_Mem模块中,
*设置数据必须要偏移地址的数据，所以这次只是为了获取内存宽度
*
************************************************************
*/
	void ClassDef_IDStoFile(Mod_Mem* inMem,Dex_Header* inheader,Dex_Maps* inmaps){
		//判断是否存在 IDSString段，不存在就添加IDSString段
		str_ModMem* modIds =inMem->getByName("ClassDef");
		if(modIds== NULL){
			modIds = inMem->newNameMem("ClassDef",ClassDef.GetSize()*0x20);
		}
		DEXLOG("[File]ClassDefine %08x %08x",modIds->Offset,modIds->Length);
		//修改maps偏移地址
		inmaps->getTypeItem(kDexTypeClassDefItem)->offset = modIds->Offset;
		inmaps->getTypeItem(kDexTypeClassDefItem)->size = ClassDef.GetSize();
		inheader->Header->classDefsOff = modIds->Offset;
		inheader->Header->classDefsSize =ClassDef.GetSize();
		 return ;
	}
/*
************************************************************
*				ClassDef_DatatoFile
*将数据偏移地址设置为文件加载偏移。
*
************************************************************
*/
	void ClassDef_DatatoFile(Mod_Mem* inMem,Dex_Types* types,Dex_Annot* annot,Dex_Encoded* encoded){
		//
		str_ModMem* modIds =inMem->getByName("ClassDef");
		if(modIds== NULL){
			DEXLOG("DatatoFile 设置原先没有数据，这里申请数据可能会出错,也不自动修复Header和MAPS");
			modIds = inMem->newNameMem("ClassDef",ClassDef.GetSize()*0x20);
			DEXLOG("ClassDef DatatoFile %08x %08x ",modIds->Offset,modIds->Length);
		}
		//设置Fileds IDS数据
		ClassDef.Reset();
		DexClassDef* dexItem =(DexClassDef*)(modIds->Addr);
		while(ClassDef.Get() != NULL){
			dexItem->classIdx = ClassDef.Get()->classIdx;
			dexItem->accessFlags = ClassDef.Get()->accessFlags;
			dexItem->superclassIdx = ClassDef.Get()->superclassIdx;
			dexItem->interfacesOff = types->LoadToFile(ClassDef.Get()->interfacesOff);
			dexItem->sourceFileIdx = ClassDef.Get()->sourceFileIdx;
			dexItem->annotationsOff = annot->LoadToFile(ClassDef.Get()->annotationsOff);
			dexItem->classDataOff = LoadToFile(ClassDef.Get()->classDataOff);
			dexItem->staticValuesOff = encoded->LoadToFile(ClassDef.Get()->staticValuesOff);
			dexItem++;
			ClassDef.Next();
		}
	}
//
/*
************************************************************
*				Dex_ClassCode
*解析ClassCode,采用Maps中读取数据*
************************************************************
*/
	DexClassData* ClassCode_ConvertList(u1* inAddr,DexHeader* in_pheader){
		DexClassData* mList = (DexClassData*)Alloc(sizeof(DexClassData));
		//获取加载偏移
		mList->BS.Offset_Load = (u4)inAddr -(u4)in_pheader;
		inAddr = readLeb128(inAddr,&mList->header.staticFieldsSize);
		inAddr = readLeb128(inAddr,&mList->header.instanceFieldsSize);
		inAddr = readLeb128(inAddr,&mList->header.directMethodsSize);
		inAddr = readLeb128(inAddr,&mList->header.virtualMethodsSize);

		FastLink<DexField> mFL_Field;
		size_t msize =mList->header.staticFieldsSize + mList->header.instanceFieldsSize;
		for(int m_j =0;m_j < msize;m_j++){
			DexField* mField = (DexField*)Alloc(sizeof(DexField));
			inAddr = readLeb128(inAddr,&mField->fieldIdx);
			inAddr = readLeb128(inAddr,&mField->accessFlags);
			mFL_Field.Add(mField);
		}
		mList->Fields = mFL_Field.First;

		FastLink<DexMethod> mFL_Method;
		msize =mList->header.directMethodsSize + mList->header.virtualMethodsSize;
		for(int m_j =0;m_j < msize ;m_j++){
			DexMethod* mMethod = (DexMethod*)Alloc(sizeof(DexMethod));
			inAddr = readLeb128(inAddr,&mMethod->methodIdx);
			inAddr = readLeb128(inAddr,&mMethod->accessFlags);
			inAddr = readLeb128(inAddr,&mMethod->codeOff);
			mFL_Method.Add(mMethod);
		}
		mList->Methods = mFL_Method.First;

		mList->BS.MemSize = (u4)inAddr - mList->BS.Offset_Load - (u4)in_pheader;
		return mList;
	}
/*
************************************************************
*				Dex_ClassCode
*解析ClassCode,采用Class_Def中读取数据*
************************************************************
*/
	void Dex_ClassCode(DexFile* inDex){
		//获取基本数据
		DexHeader*  pHeader = (DexHeader*)inDex->pHeader;
		DEXLOG("[Parse]Dex_ClassCode ClassDef");
		int MethodNum = 0;
		ClassDef.Reset();
		while(NULL != ClassDef.Get()){
			if(NULL != ClassDef.Get()->classDataOff){
				u1* _Addr = (u1*)AddAddr2(pHeader,ClassDef.Get()->classDataOff);
				DexClassData* _List = ClassCode_ConvertList(_Addr,pHeader);
				ClassData.Add(_List);
				MethodNum++;
			}
			ClassDef.Next();
		}
		DEXLOG("[Parse]Dex_ClassCode ClassDef Size %08x",MethodNum);
	}
/*
************************************************************
*				Dex_ClassCode
*解析ClassCode,采用Maps中读取数据*
************************************************************
*/
	void Dex_ClassCode(DexFile* inDex,Dex_Maps* inmaps){
		DEXLOG("[Parse]Dex_ClassCode Maps");
		//MethodNum为添加，Code个数
		mapsItem* inMapClassCode =inmaps->getTypeItem(kDexTypeClassDataItem);
		//定位头地址
		u1* mCodeAddr =(u1*)((u4)inDex->pHeader + inMapClassCode->offset);
		//遍历全部的DexClassData
		for(int m_i =0;m_i < inMapClassCode->size;m_i++){
			DEXLOG("Dex_ClassCode %d",m_i);
			DexClassData* mList = (DexClassData*)Alloc(sizeof(DexClassData));
			//获取加载偏移
			mList->BS.Offset_Load = (u4)mCodeAddr -(u4)inDex->pHeader;
			mCodeAddr = readLeb128(mCodeAddr,&mList->header.staticFieldsSize);
			mCodeAddr = readLeb128(mCodeAddr,&mList->header.instanceFieldsSize);
			mCodeAddr = readLeb128(mCodeAddr,&mList->header.directMethodsSize);
			mCodeAddr = readLeb128(mCodeAddr,&mList->header.virtualMethodsSize);

			FastLink<DexField> mFL_Field;
			size_t msize =mList->header.staticFieldsSize + mList->header.instanceFieldsSize;
			for(int m_j =0;m_j < msize;m_j++){
				DexField* mField = (DexField*)Alloc(sizeof(DexField));
				mCodeAddr = readLeb128(mCodeAddr,&mField->fieldIdx);
				mCodeAddr = readLeb128(mCodeAddr,&mField->accessFlags);
				mFL_Field.Add(mField);
			}
			mList->Fields = mFL_Field.First;

			FastLink<DexMethod> mFL_DexMethod;
			msize =mList->header.directMethodsSize + mList->header.virtualMethodsSize;
			for(int m_j =0;m_j < msize ;m_j++){
				DexMethod* mMethod = (DexMethod*)Alloc(sizeof(DexMethod));
				mCodeAddr = readLeb128(mCodeAddr,&mMethod->methodIdx);
				mCodeAddr = readLeb128(mCodeAddr,&mMethod->accessFlags);
				mCodeAddr = readLeb128(mCodeAddr,&mMethod->codeOff);
				mFL_DexMethod.Add(mMethod);
			}
			mList->Methods = mFL_DexMethod.First;

			mList->BS.MemSize = (u4)mCodeAddr - mList->BS.Offset_Load - (u4)inDex->pHeader;
			ClassData.Add(mList);
		}
	}
/*
************************************************************
*				DatatoFile
*设置DatatoFile*
************************************************************
*/
	void ClassCode_DatatoFile(Mod_Mem* inMem,Dex_Maps* inmaps){
		if(NULL == ClassData.GetSize()){
			DEXLOG("[ERR]没有存在ClassD数据,直接退出!");
			return ;
		}
		//获取debug模块内存，没有就新建
		str_ModMem* modList =inMem->getByName("ClassData");
		if(NULL == modList){
			modList = inMem->newNameMem("ClassData",AlignAdd(ClassData.GetMemSize(),0x04));
		}
		DEXLOG("[File]ClassD %08x %08x",modList->Offset,modList->Length);
		//
		inmaps->setTypeOffset(kDexTypeClassDataItem,modList->Offset);
		inmaps->setTypeSize(kDexTypeClassDataItem,ClassData.GetSize());
		//
		DexClassData* mData = ClassData.First;
		u1* mAddr = modList->Addr;
		while(mData != NULL){
			//
			mData->BS.Offset_File = modList->Offset +(u4)mAddr - (u4)modList->Addr;
			mAddr = writeL128(mAddr,mData->header.staticFieldsSize);
			mAddr = writeL128(mAddr,mData->header.instanceFieldsSize);
			mAddr = writeL128(mAddr,mData->header.directMethodsSize);
			mAddr = writeL128(mAddr,mData->header.virtualMethodsSize);
			DexField* mField = mData->Fields;
			while(mField != NULL){
				mAddr = writeL128(mAddr,mField->fieldIdx);
				mAddr = writeL128(mAddr,mField->accessFlags);
				mField = mField->next;
			}
			DexMethod* mMethod = mData->Methods;
			while(mMethod != NULL){
				mAddr = writeL128(mAddr,mMethod->methodIdx);
				mAddr = writeL128(mAddr,mMethod->accessFlags);
				//Load To File
				mAddr = writeL128(mAddr,Code_LoadToFile(mMethod->codeOff));
				mMethod = mMethod->next;
			}
			mData = mData->next;
		}
	}
/*
************************************************************
*				Code_ConvertList
*将偏移地址中的inOffset解析成Code*
*************************************************************
*/
	DexCodeL* Code_ConvertList(DexFile* inDex,u4 inOffset){
		DexCodeL* mCode = (DexCodeL*)Alloc(sizeof(DexCodeL));
		mCode->BS.SetLoadOffset(inOffset);
		u1* mMem =(u1*)((u4)inDex->pHeader + inOffset);
		u4	mMemStart =(u4)mMem;
		//复制DexCodeL
		mCode->Size_Reg = *(u2*)mMem;mMem += 2;
		mCode->Size_inArg =*(u2*)mMem;mMem += 2;
		mCode->Size_outArg =*(u2*)mMem;mMem += 2;
		mCode->Size_Tries =*(u2*)mMem;mMem += 2;
		mCode->DebugInfo =*(u4*)mMem;mMem += 4;
		mCode->Size_Code =*(u4*)mMem;mMem += 4;
		//获取整个方法代码宽度、然后对齐、定方位代码尾端，查看是否有其他数据
		mCode->CodeSize =(mCode->Size_Code*2+3)&(~3);
		mCode->CodeInfo = (char*)malloc(mCode->CodeSize);
		memcpy(mCode->CodeInfo,mMem,mCode->CodeSize);
//		mCode->CodeInfo = (void*)mMem;
		mMem += mCode->CodeSize;
		//判断是否存在Try 和Catch,然后获取异常宽度
		if(mCode->Size_Tries != 0){
			mCode->Try_CatchIngfo = (void*)mMem;
			//解析TryCatch分支	 Number of instructions in try
			for(int m_i = 0;m_i < mCode->Size_Tries;m_i++){
					DexTryL* mTry  =(DexTryL*)Alloc(sizeof(DexTryL));
					mTry->StartAddr =*(u4*)mMem;
					mMem += 4;
					mTry->Insncount =*(u2*)mMem;
					mMem += 2;
					mTry->Offset_Handler =*(u2*)mMem;
					mMem += 2;
					mCode->Try =  (DexTryL*)Link::AddToLast(mCode->Try,mTry);
			}
			// # Number of catch handler blocks: 0x2
			mMem = readLeb128(mMem,&mCode->CatchBlocks);
			for(int m_i = 0;m_i < mCode->CatchBlocks;m_i++){
				DexCatchListL* mList = (DexCatchListL*)Alloc(sizeof(DexCatchListL));
				//Size_Handler 是按Sleb128保存的，当小于0时表示存在catch_all内容
				//catch_all于blocks 一一对应。
				mMem = readSLeb128(mMem,&mList->Size_Handler);
				u4 msize = mList->Size_Handler;
				if(msize >= 0x80000000){
					msize = 0 - msize;
				}
				if(msize == 0){
					mMem = readLeb128(mMem,&mList->catch_all);
				}else{
					for(int m_j = 0;m_j < msize; m_j++){
						DexCatchItemL* _Item = (DexCatchItemL*)Alloc(sizeof(DexCatchItemL));
						mMem = readLeb128(mMem,&_Item->Type);
						mMem = readLeb128(mMem,&_Item->Byte_Addr);
						mList->Item =  (DexCatchItemL*)Link::AddToLast(mList->Item,_Item);
					}
					if(mList->Size_Handler >= 0x80000000){
						mMem = readLeb128(mMem,&mList->catch_all);
					}
				}
				mCode->Catch =  (DexCatchListL*)Link::AddToLast(mCode->Catch,mList);
			}
			//设置内存大小，必须对齐,
			mMem = (u1*)((u4)(mMem+3)&(~3));
			//直接获取保存数据
			mCode->Try_CatchSize =(u4)mMem - (u4)mCode->Try_CatchIngfo;
	//		DEXLOG("Code  Try_CatchIngfo %08x %08x",mCode->Try_CatchIngfo,mCode->Try_CatchSize);
		}
		mCode->BS.SetMemSize((u4)mMem - mMemStart);
		return mCode;
	}
/*
************************************************************
*				DatatoFile
*设置DatatoFile*
************************************************************
*/
	void Code_AddAll(DexFile* inDex){
		//获取基本数据
		DexHeader*  pHeader = (DexHeader*)inDex->pHeader;
		DexCodeL* _Code = NULL;
		ClassCode.First = NULL;
		ClassData.Reset();
		while(NULL != ClassData.Get()){
			DexMethod* mMethod = ClassData.Get()->Methods;
			while(NULL != mMethod){
				if(NULL != mMethod->codeOff){
					DexCodeL* _item = Code_ConvertList(inDex,mMethod->codeOff);
					ClassCode.Add(_item);
				}
				mMethod = mMethod->next;
			}
			ClassData.Next();
		}
	}
/*
************************************************************
*				DatatoFile
*设置DatatoFile*
************************************************************
*/
	void Code_DatatoFile(Mod_Mem* inMem,Dex_Maps* inmaps,Dex_Debug* debug){
		if(NULL == ClassCode.GetSize()){
			DEXLOG("[ERR]没有存在Code数据,直接退出!");
			return ;
		}
		//获取Code模块内存，没有就新建
		str_ModMem* modList =inMem->getByName("Code");
		if(NULL == modList){
			modList = inMem->newNameMem("Code",AlignAdd(ClassCode.GetMemSize(),0x1000));
		}
		DEXLOG("[File]Code DatatoFile %08x %08x",modList->Offset,modList->Length);
		inmaps->setTypeOffset(kDexTypeCodeItem,modList->Offset);
		inmaps->setTypeSize(kDexTypeCodeItem,ClassCode.GetSize());
		//
		DexCodeL* mCode = ClassCode.First;
		u1* mMem = modList->Addr;
		int m_i = 0;
		while(NULL != mCode){
			mCode->BS.SetFileOffset(AddAddr2(mMem,SubAddr2(modList->Offset,modList->Addr)));
			//复制数据到内存
			*(u2*)mMem =mCode->Size_Reg;mMem+=2;
			*(u2*)mMem =mCode->Size_inArg;mMem+=2;
			*(u2*)mMem =mCode->Size_outArg;mMem+=2;
			*(u2*)mMem =mCode->Size_Tries;mMem+=2;
			if(debug != NULL){
				*(u4*)mMem = debug->LoadToFile(mCode->DebugInfo);mMem+=4;
			}else{
				*(u4*)mMem = mCode->DebugInfo;mMem+=4;
			}
			//可能需要修改
			*(u4*)mMem =mCode->Size_Code;mMem+=4;
			//获取代码宽Size_Code
			memcpy(mMem,mCode->CodeInfo,mCode->CodeSize);
			mMem += mCode->CodeSize;
			//Try_Catch分支，是直接拷贝过来的，因为里面代码于位置无关，
			if(mCode->Size_Tries != 0){
				memcpy(mMem,mCode->Try_CatchIngfo,mCode->Try_CatchSize);
				mMem += mCode->Try_CatchSize;
			}
			mCode = mCode->next;
		}
	}
//
	u4 Data_LoadToFile(u4 inAddr){
		DexClassData* mdata = ClassData.First;
		while(NULL != mdata){
			if(mdata->BS.Offset_Load == inAddr){
				return mdata->BS.Offset_File;
			}
			mdata = mdata->next;
		}
		return inAddr;
	}
//
	u4 Code_LoadToFile(u4 inAddr){
		DexCodeL* mCode = ClassCode.First;
		while(NULL != mCode){
			if(mCode->BS.Offset_Load == inAddr){
				return mCode->BS.Offset_File;
			}
			mCode = mCode->next;
		}
		return inAddr;
	}
//
	u4 LoadToFile(u4 inAddr){
		DexClassData* mdata = ClassData.First;
		while(NULL != mdata){
			if(mdata->BS.Offset_Load == inAddr){
				return mdata->BS.Offset_File;
			}
			mdata = mdata->next;
		}
		DexCodeL* mCode = ClassCode.First;
		while(NULL != mCode){
			if(mCode->BS.Offset_Load == inAddr){
				return mCode->BS.Offset_File;
			}
			mCode = mCode->next;
		}
		return inAddr;
	}
};};
#endif
