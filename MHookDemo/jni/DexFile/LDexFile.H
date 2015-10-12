/*
 * LDexFile.h
 *
 *  Created on: 2015年9月23日
 *      Author: Ls Mouse
 */
#ifndef LDEXFILE_H_
#define LDEXFILE_H_
#include <stdio.h>
#include <unistd.h>
#include "Dex.H"
#include "LDex_Header.H"
#include "LDex_Map.H"
#include "LDex_String.H"
#include "LDex_Types.H"
#include "LDex_Proto.H"
#include "LDex_Filed.H"
#include "LDex_Method.H"
#include "LDex_Class.H"
#include "LDex_Encoded.H"
#include "LDex_Annot.H"
#include "LDex_Debug.H"
/**
 *			利用DexFile  对文件进行解析
 *	1.对DexFile指针进行解析，分离数据
 *	2.合成已分离的数据
 *	3.将合成数据保存成文件
 */
class  LDexFile: LDex_Util{
public:
//初始化变量
	void* 		loadAddr = NULL;
	size_t		Size = 0;
	DexFile*	dex = NULL;
	Mod_Mem*	mem = NULL;
//定义结构
	LDex_Header*	DexHeader = NULL;
	LDex_String*	DexString = NULL;
	LDex_Types*		DexTypes = NULL;
	LDex_Filed*		DexFiled = NULL;
	LDex_Proto*		DexProto = NULL;
	LDex_Method*	DexMethod = NULL;
	LDex_Class*		DexClass = NULL;
	LDex_Map*		DexMap = NULL;
	//
	LDex_Encoded*	DexEncoded = NULL;
	LDex_Annot*		DexAnnot = NULL;
	LDex_Debug*		DexDebug = NULL;
//初始化
	LDexFile(){}
/*
************************************************************
*				ParseAllIds
* 解析全部ParseAllIds，
*这样保存ParseAllIds数据防止后续发生更改，尤其是Header
************************************************************
*/
	void ParseAllIds(){
		DexHeader = new LDex_Header(dex);
		DexString = new LDex_String(dex,DexMap);
		DexTypes = new LDex_Types(dex,DexMap);
		DexProto = new LDex_Proto(dex,DexMap);
		DexFiled = new LDex_Filed(dex,DexMap);
		DexMethod = new LDex_Method(dex,DexMap);
		DexClass->Dex_ClassDef(dex,DexMap);
	}
/*
************************************************************
*				DumpToFile
* Dump出文件*
* 内存数据排放问题
*	使用map item type codes的大小顺序开始排放
*	设置对齐方便数据查看 mMem = mem->newMem(Align(mem->Szie(),0x100));
*先解码DexFile
************************************************************
*/
	void DumpToFile(char* appName,char* inFile){
		DEXLOG("DumpToFile");
		mem = new Mod_Mem();
		//header、string_ids、types_ids、
		DexHeader->IDStoFile(mem);
		DexString->IDStoFile(mem,DexHeader,DexMap);
		DexTypes->IDStoFile(mem,DexHeader,DexMap);
		DexProto->IDStoFile(mem,DexHeader,DexMap);
		DexFiled->IDStoFile(mem,DexHeader,DexMap);
		DexMethod->IDStoFile(mem,DexHeader,DexMap);
		DexClass->ClassDef_IDStoFile(mem,DexHeader,DexMap);
	//以下便是Code数据了,设置数据开始,
		DexHeader->Header->dataOff= mem->Size();
	//解析IDS对应的数据(code)
		//TypeList
		DexTypes->DatatoFile(mem,DexMap);
		//proto(需要TypeList偏移地址)
		DexProto->DatatoFile(mem,DexHeader,DexMap,DexTypes);
		DEXLOG("[File]Annot");

		DexAnnot->AnnotToData(mem,DexMap);

		DexAnnot->AnnotSetToData(mem,DexMap);
		DexAnnot->AnnotSRLToData(mem,DexMap);
		DexAnnot->AnnotDirToData(mem,DexMap);
	//encoded array
		if(NULL != DexEncoded){
			DexEncoded->DatatoFile(mem,DexMap);
		}
	//Debug
		if(NULL != DexDebug){
			DexDebug->DatatoFile(mem,DexMap);
		}
	//Code
		DexClass->Code_DatatoFile(mem,DexMap,DexDebug);
		DexClass->ClassCode_DatatoFile(mem,DexMap);
		DexClass->ClassDef_DatatoFile(mem,DexTypes,DexAnnot,DexEncoded);
		DexString->DatatoFile(mem,DexMap);
	//根据DUMP数据正在修改MAPS和Header
		DexMap->IDStoFile(mem,DexHeader);
		DexHeader->IDStoFile(mem);
	//Save To File
		DEXLOG("[File]Save");
		mem->SaveFile(appName,inFile);
	}
/*
************************************************************
*				DexParse
* 解析DexFile
* 解析每一个Item，然后在DumpToFile中实现组装
* 将解析和组装分开做
* 	1.为了2代以上加固
* 	2.为了调试添加数据
*	3.
************************************************************
*/
	LDexFile(void* inAddr,DexFile* inDex){
		loadAddr = inAddr;
		dex = inDex;
		Parse();
	}
/*
************************************************************
*				Parse
* 解析DexFile
* 解析每一个Item，然后在DumpToFile中实现组装
* 将解析和组装分开做
* 	1.为了2代以上加固
* 	2.为了调试添加数据
*	3.
************************************************************
*/
	void Parse(){
		DEXLOG("DexParse");
		//Header、maps
		DexHeader = new LDex_Header(dex);
		DexMap = new LDex_Map(dex);
		//string、types、proto、fileds、methods、Class_Def
		DexString = new LDex_String(dex,DexMap);
		DexTypes = new LDex_Types(dex,DexMap);
		DexProto = new LDex_Proto(dex,DexMap);
		DexFiled = new LDex_Filed(dex,DexMap);
		DexMethod = new LDex_Method(dex,DexMap);
		//将Annot分开处理、使用Annot类进行解析
		//Annot中包含kDexTypeAnnotationSetRefList、kDexTypeAnnotationSetItem、kDexTypeAnnotationItem、kDexTypeAnnotationsDirectoryItem
		DexAnnot = new LDex_Annot(dex,DexMap);
		//下面首先要判断是否存在，然后再去初始化
		//Encoded、Debug、Dex_Code
		if(DexMap->getTypeItem(kDexTypeEncodedArrayItem) != NULL){
			DexEncoded = new LDex_Encoded(dex,DexMap);
		}
		if(DexMap->getTypeItem(kDexTypeDebugInfoItem) != NULL){
			DexDebug = new LDex_Debug(dex,DexMap);
		}
		//包含了 Class_Def、Class_Code、Code
		DexClass = new LDex_Class();
		DexClass->Dex_ClassDef(dex,DexMap);
		//解析ClassData  然后根据内容解析Dex_Code
		//百度加固就消灭了kDexTypeClassDataItem(数据在只是将Maps从0x12改成0x11),
		if(NULL != DexMap->getTypeItem(kDexTypeClassDataItem)){
			DEXLOG("采用maps，读取Class_data");
			DexClass->Dex_ClassCode(dex,DexMap);
		}else{
			DEXLOG("不存在kDexTypeClassDataItem,从ClassDef中读取数据");
			DexClass->Dex_ClassCode(dex);
		}
		//通过读取Class_data偏移获取Code
		DexClass->Code_AddAll(dex);
		DEXLOG("[Parse]Over");
	}
};
#endif /* LDEXFILE_H_ */
