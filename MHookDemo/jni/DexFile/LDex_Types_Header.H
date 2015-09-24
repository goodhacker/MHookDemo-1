/*
 * LDex_Types_Header.H
 *
 *  Created on: 2015年9月24日
 *      Author: Ls Mouse
 */

#ifndef LDEX_TYPES_HEADER_H_
#define LDEX_TYPES_HEADER_H_
//__________________________________________________________
//存放typeItem
typedef struct typeItem{
	struct typeItem* next;
	u4	 type;
}typeItem;
//存放typeList
//位置空间在Code中
//需要保存加载地址等信息，所以加载Base段
typedef struct typeList{
	struct typeList* next;
	Base	BS;
	struct typeItem* item;
	u4		Size;
}typeList;


#endif
/* LDEX_TYPES_HEADER_H_ */
