/*
 * LDex_String_Header.H
 *
 *  Created on: 2015年9月24日
 *      Author: Ls Mouse
 */

#ifndef LDEX_STRING_HEADER_H_
#define LDEX_STRING_HEADER_H_
//字符串保存列表
typedef struct stringItem{
	struct stringItem* next;
	Base	BS;
	//
	char* 	str;
	u4		L128Len;
	u4		L128Code;
	u4		strLen;
	u4	 	size;
}stringItem;



#endif
/* LDEX_STRING_HEADER_H_ */
