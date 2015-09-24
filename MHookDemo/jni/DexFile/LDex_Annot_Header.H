/*
 * LDex_Annot_Header.H
 *
 *  Created on: 2015年9月23日
 *      Author: Ls Mouse
 */
#ifndef LDEX_ANNOT_HEADER_H_
#define LDEX_ANNOT_HEADER_H_
/*
*关系对应图
*	Annotation Dir(kDexTypeAnnotationsDirectoryItem)(级别大)
*		指向
*		annotation set ref list(kDexTypeAnnotationSetRefList)
*			指向
* 			annotation set(kDexTypeAnnotationSetItem)
* 				指向
*				annotation(kDexTypeAnnotationItem)(级别小)
*					指向
* 					annotation element
*/
//annotation element

typedef struct Annot_Element{
	struct Annot_Element* next;
	Base	BS;
	u4		Type;
	u4		Value;
	u4		ValueLen;
}Annot_Element;
//annotation element list
typedef struct Annot_List{
	Annot_List* next;
	Base	BS;
	u4		Ann_Ele;
	u4		Ele_Type;
	u4		Ele_Size;
	u4		Ele_Value;
	u4		EleValue_Len;
	Annot_Element*  Item;
	Annot_List* son;
}Annot_List;
//Annotation
typedef struct Annot{
	struct Annot* next;
	Base	BS;
	Annot_Element* Element;
	Annot_List*	List;
	u4		Visibility;
	u4		Type;
	u4 		Count;
}Annot;
//Annot_Set			kDexTypeAnnotationSetItem
typedef struct Annot_Set_Item{
	Annot_Set_Item* next;
	u4	Offset_Annot;
}Annot_Set_Item;
typedef struct Annot_Set{
	struct Annot_Set* next;
	Base	BS;
	Annot_Set_Item* Item;
	u4		Size;
}Annot_Set;
//Annot_SRL 			kDexTypeAnnotationSetRefList
typedef struct Annot_SRL_Item{
	struct Annot_SRL_Item* next;
	u4	Offset;
}Annot_SRL_Item;
typedef struct Annot_SRL{
	struct Annot_SRL* next;
	Base	BS;
	u4		Size;
	Annot_SRL_Item* iTem;
	u4		Offset_AnnotSet;
}Annot_SRL;
//AnnotDir			kDexTypeAnnotationsDirectoryItem
typedef struct AnnotDir_Item{
	struct AnnotDir_Item* next;
	Base BS;
	u4	Idx;
	u4	Offset_Annot;
}AnnotDir_Item;
typedef struct AnnotDir{
	struct AnnotDir* next;
	Base BS;
	u4 	Offset_Class;
	u4	Size_Feilds;
	u4	Size_Methods;
	u4	Size_Params;
	AnnotDir_Item*	item;
}AnnotDir;
#endif
/* LDEX_ANNOT_HEADER_H_ */
