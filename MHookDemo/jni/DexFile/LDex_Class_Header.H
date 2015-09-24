/*
 * LDex_Class_Header.H
 *
 *  Created on: 2015年9月23日
 *      Author: Ls Mouse
 */
#ifndef LDEX_CLASS_HEADER_H_
#define LDEX_CLASS_HEADER_H_
typedef struct ClassDefItem{
	struct ClassDefItem* next;
	u4  classIdx;           /* index into typeIds for this class */
	u4  accessFlags;
	u4  superclassIdx;      /* index into typeIds for superclass */
	u4  interfacesOff;      /* file offset to DexTypeList */
	u4  sourceFileIdx;      /* index into stringIds for source file name */
	u4  annotationsOff;     /* file offset to annotations_directory_item */
	u4  classDataOff;       /* file offset to class_data_item */
	u4  staticValuesOff;    /* file offset to DexEncodedArray */
}ClassDefItem;
//Class Code
typedef struct DexClassDataHeader {
	u4 staticFieldsSize;
	u4 instanceFieldsSize;
	u4 directMethodsSize;
	u4 virtualMethodsSize;
} DexClassDataHeader;
/* expanded form of encoded_field */
typedef struct DexField {
	struct DexField* next;
	u4 fieldIdx;    /* index to a field_id_item */
	u4 accessFlags;
} DexField;
/* expanded form of encoded_method */
typedef struct DexMethod {
	struct DexMethod* next;
	u4 methodIdx;    /* index to a method_id_item */
	u4 accessFlags;
	u4 codeOff;      /* file offset to a code_item */
} DexMethod;
/* expanded form of class_data_item. Note: If a particular item is
 * absent (e.g., no static fields), then the corresponding pointer
 * is set to NULL. */
typedef struct DexClassData {
	struct DexClassData* next;
	Base	BS;
	DexClassDataHeader header;
	DexField*	Fields;
	DexMethod* 	Methods;
} DexClassData;
//Code
/*
 * Direct-mapped "try_item".
 */
typedef struct DexCatchItemL {
	DexCatchItemL* 	next;
	u4	Type;
	u4	Byte_Addr;
}DexCatchItemL;
//
typedef struct DexCatchListL{
	DexCatchListL* 	next;
	DexCatchItemL*	Item;
	u4	Size_Handler;
	u4	catch_all;
}DexCatchListL;
//
typedef struct DexTryL{
	DexTryL* 	next;
	u4	StartAddr;
	u2	Insncount;
	u2	Offset_Handler;
}DexTryL;
/*
 * Direct-mapped "code_item".
 *
 * The "catches" table is used when throwing an exception,
 * "debugInfo" is used when displaying an exception stack trace or
 * debugging. An offset of zero indicates that there are no entries.
 */
typedef struct DexCodeL{
	DexCodeL* 	next;
	Base	BS;
	u2	Size_Reg;
	u2	Size_inArg;
	u2	Size_outArg;
	u2	Size_Tries;
	u4	DebugInfo;
	u4	Size_Code;
//
	DexTryL* Try;
	DexCatchListL* Catch;
//
	void* CodeInfo;
	size_t CodeSize;
//
	void* Try_CatchIngfo;
	size_t Try_CatchSize;
	u4	CatchBlocks;
//
}DexCodeL;
//
#endif
/* LDEX_CLASS_HEADER_H_ */
