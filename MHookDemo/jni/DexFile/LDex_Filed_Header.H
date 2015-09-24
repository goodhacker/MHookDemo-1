/*
 * LDex_Filed_Header.H
 *
 *  Created on: 2015年9月24日
 *      Author: Ls Mouse
 */

#ifndef LDEX_FILED_HEADER_H_
#define LDEX_FILED_HEADER_H_
typedef struct FieldsItem{
	struct FieldsItem* next;
	u4  classIdx;           /* index into typeIds list for defining class */
	u4  typeIdx;            /* index into typeIds for field type */
	u4  nameIdx;            /* index into stringIds for field name */
}FieldsItem;
#endif
/* LDEX_FILED_HEADER_H_ */
