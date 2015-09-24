/*
 * LDex_Map_Header.H
 *
 *  Created on: 2015年9月24日
 *      Author: Ls Mouse
 */

#ifndef LDEX_MAP_HEADER_H_
#define LDEX_MAP_HEADER_H_
typedef struct mapsItem{
	struct mapsItem* next;
	u4	type;       		 /*Section type*/
 	u4	unused;				/*unused*/
	u4	size;        		/* section size*/
	u4	offset;     		/* section offset */
}mapsItem;
//
typedef struct maplist{
	struct mapsItem* item;
	size_t mapSize;
}maplist;
#endif /* LDEX_MAP_HEADER_H_ */
