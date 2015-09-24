#ifndef __DexParse_H_
#define __DexParse_H_
#include <stdio.h>
#include <unistd.h>
#include "DexFile/Dex.H"
//
#include "DexHeader.H"
#include "DexString.H"
#include "DexTypes.H"
#include "DexProto.H"
#include "DexFields.H"
#include "DexMethods.H"
#include "DexMaps.H"
#include "Dex_Encoded.H"
#include "Dex_Annot.H"
#include "DexDebug.H"
#include "DexClass.H"
/*
*									解析Dex文件格式
*1.此类为全部脱壳最基本类，此类主要分析Dex文件内容
*2.将全部的数据分段处理
*3.然后通过合并将Dex合并在一起
*4.但是注意，Code代码要进行特殊处理，在优化之前能收集的代码就可以不再进行更新，防止代码OAT或ODEX化
*5.但是特殊加固需要特殊处理
*/
namespace Dex{
class DexParse : LDex_Util{
public:
	void* 	loadAddr = NULL;
	size_t	Size = NULL;
	DexFile*	dex = NULL;
	Mod_Mem*	mem = NULL;
//IDS段
	Dex_Header* 	header = NULL;
	Dex_String* 	string = NULL;
	Dex_Types*  	types = NULL;
	Dex_Proto*  	proto = NULL;
	Dex_Fileds*		fileds = NULL;
	Dex_Methods* 	methods = NULL;
	Dex_Class* 		Dexclass = NULL;
	Dex_Maps*		maps = NULL;
//可能不会存在段
	Dex_Encoded*	encoded = NULL;
	Dex_Annot* 		annot = NULL;
	Dex_Debug*		debug = NULL;
	DexParse(){}
/*
************************************************************
*				ParseAllIds
* 解析全部ParseAllIds，
*这样保存ParseAllIds数据防止后续发生更改，尤其是Header
************************************************************
*/
	void ParseAllIds(){
		header = new Dex_Header(dex);
		maps = new Dex_Maps(dex);
		string = new Dex_String(dex,maps);
		types = new Dex_Types(dex,maps);
		proto = new Dex_Proto(dex,maps);
		fileds = new Dex_Fileds(dex,maps);
		methods = new Dex_Methods(dex,maps);
		Dexclass->Dex_ClassDef(dex,maps);
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
		header->IDStoFile(mem);
		string->IDStoFile(mem,header,maps);
		types->IDStoFile(mem,header,maps);
		proto->IDStoFile(mem,header,maps);
		fileds->IDStoFile(mem,header,maps);
		methods->IDStoFile(mem,header,maps);
		Dexclass->ClassDef_IDStoFile(mem,header,maps);
	//以下便是Code数据了,设置数据开始,
		header->Header->dataOff= mem->Size();
	//解析IDS对应的数据(code)
		//TypeList
		types->DatatoFile(mem,maps);
		//proto(需要TypeList偏移地址)
		proto->DatatoFile(mem,header,maps,types);
		annot->AnnotToData(mem,maps);
		annot->AnnotSetToData(mem,maps);
		annot->AnnotSRLToData(mem,maps);
		annot->AnnotDirToData(mem,maps);
	//encoded array
		if(NULL != encoded){
			encoded->DatatoFile(mem,maps);
		}
	//Debug
		if(NULL != debug){
			debug->DatatoFile(mem,maps);
		}
	//Code
		Dexclass->Code_DatatoFile(mem,maps,debug);
		Dexclass->ClassCode_DatatoFile(mem,maps);
		Dexclass->ClassDef_DatatoFile(mem,types,annot,encoded);
		string->DatatoFile(mem,maps);
	//根据DUMP数据正在修改MAPS和Header
		maps->IDStoFile(mem,header);
		header->IDStoFile(mem);
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
 	DexParse(void* inAddr,DexFile* inDex){
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
		header = new Dex_Header(dex);
		maps = new Dex_Maps(dex);
		//string、types、proto、fileds、methods、Class_Def
		string = new Dex_String(dex,maps);
		types = new Dex_Types(dex,maps);
		proto = new Dex_Proto(dex,maps);
		fileds = new Dex_Fileds(dex,maps);
		methods = new Dex_Methods(dex,maps);
		//将Annot分开处理、使用Annot类进行解析
		//Annot中包含kDexTypeAnnotationSetRefList、kDexTypeAnnotationSetItem、kDexTypeAnnotationItem、kDexTypeAnnotationsDirectoryItem
		annot = new Dex_Annot(dex,maps);
		//下面首先要判断是否存在，然后再去初始化
		//Encoded、Debug、Dex_Code
		if(maps->getTypeItem(kDexTypeEncodedArrayItem) != NULL){
			encoded = new Dex_Encoded(dex,maps);
		}
		if(maps->getTypeItem(kDexTypeDebugInfoItem) != NULL){
			debug = new Dex_Debug(dex,maps);
		}
		//包含了 Class_Def、Class_Code、Code
		Dexclass = new Dex_Class();
		Dexclass->Dex_ClassDef(dex,maps);
		//解析ClassData  然后根据内容解析Dex_Code
		//百度加固就消灭了kDexTypeClassDataItem(数据在只是将Maps从0x12改成0x11),
		if(NULL != maps->getTypeItem(kDexTypeClassDataItem)){
			DEXLOG("采用maps，读取Class_data");
			Dexclass->Dex_ClassCode(dex,maps);
		}else{
			DEXLOG("不存在kDexTypeClassDataItem,从ClassDef中读取数据");
			Dexclass->Dex_ClassCode(dex);
		}
		//通过读取Class_data偏移获取Code
		Dexclass->Code_AddAll(dex);
		DEXLOG("[Parse]Over");
 	}
};};
#endif
