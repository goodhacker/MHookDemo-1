/*
 * LDex_Debug_Header.H
 *
 *  Created on: 2015年9月24日
 *      Author: Ls Mouse
 */

#ifndef LDEX_DEBUG_HEADER_H_
#define LDEX_DEBUG_HEADER_H_
struct DebugItem{
	DebugItem* 	next;
	u4	param_name;
};
//说明Size_Reg不表示DebugItem长度
struct DebugList{
	DebugList* 	next;
	Base 	BS;
	struct DebugItem* Item;
	u4	number;
	u4	Size_Reg;
};
#endif /* LDEX_DEBUG_HEADER_H_ */
