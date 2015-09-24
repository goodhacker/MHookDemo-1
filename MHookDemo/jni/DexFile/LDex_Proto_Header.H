/*
 * LDex_Proto_Header.H
 *
 *  Created on: 2015年9月24日
 *      Author: Ls Mouse
 */

#ifndef LDEX_PROTO_HEADER_H_
#define LDEX_PROTO_HEADER_H_
//__________________________________________________________
typedef struct ProtoItem{
	struct ProtoItem* next;
	u4	shortyIdx;          /* index into stringIds for shorty descriptor */
	u4	returnTypeIdx;      /* index into typeIds list for return type */
	u4	parametersOff;      /* file offset to type_list for parameter types */
}ProtoItem;



#endif
/* LDEX_PROTO_HEADER_H_ */
