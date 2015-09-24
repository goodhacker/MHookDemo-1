/*
 * LDex_Method_Header.H
 *
 *  Created on: 2015年9月24日
 *      Author: Ls Mouse
 */

#ifndef LDEX_METHOD_HEADER_H_
#define LDEX_METHOD_HEADER_H_
//__________________________________________________________
typedef struct MethodsItem{
	struct MethodsItem* next;
	u4  classIdx;           /* index into typeIds list for defining class */
	u4  protoIdx;           /* index into protoIds for method prototype */
	u4  nameIdx;            /* index into stringIds for method name */
}MethodsItem;


#endif
/* LDEX_METHOD_HEADER_H_ */
