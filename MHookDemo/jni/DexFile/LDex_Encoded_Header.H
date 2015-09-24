/*
 * LDex_Encoded_Header.H
 *
 *  Created on: 2015年9月24日
 *      Author: Ls Mouse
 */

#ifndef LDEX_ENCODED_HEADER_H_
#define LDEX_ENCODED_HEADER_H_
typedef struct Encoded_Item{
	struct Encoded_Item* next;
	u4	type;
	u4	HValue;
	u4	Value;
	u4 	KeyLen;
	u4 	ValueLen;
}Encoded_Item;

typedef struct Encoded_List{
	struct Encoded_List* next;
	Base	BS;
	Encoded_Item* item;
	size_t Itemsize;
}Encoded_List;
#endif /* LDEX_ENCODED_HEADER_H_ */
