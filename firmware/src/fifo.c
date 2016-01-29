/*
 * fifo.c
 *
 *  Created on: Jan 25, 2016
 *      Author: Conor
 */
#include <stdio.h>
#include "fifo.h"
#include "app.h"


#define FIFO_CREATE(NAME, TYPE, SIZE) \
static SI_SEGMENT_VARIABLE(NAME##MEM[SIZE],TYPE,SI_SEG_XDATA);\
volatile TYPE* NAME##WP;\
volatile TYPE* NAME##RP;\
void NAME##_fifo_init()\
{\
	NAME##WP = NAME##MEM;\
	NAME##RP = NAME##MEM;\
}\
int NAME##_fifo_append(TYPE* _data)\
{\
	volatile TYPE* next = 0;\
	next = NAME##WP + 1;\
	if (next == (NAME##MEM + SIZE))\
	{\
		next = NAME##MEM;\
	}\
	if (next == NAME##RP)\
	{\
		return -1;\
	}\
	else\
	{\
		*(NAME##WP) = *_data;\
		NAME##WP = next;\
		return 0;\
	}\
}\
int NAME##_fifo_get(TYPE* _data)\
{\
	if (NAME##RP == NAME##WP)\
	{\
		return -1;\
	}\
	*_data = *(NAME##RP);\
	NAME##RP = NAME##RP + 1;\
	if (NAME##RP == &NAME##MEM[SIZE])\
	{\
		NAME##RP = NAME##MEM;\
	}\
	return 0;\
}

#ifdef U2F_PRINT
FIFO_CREATE(debug,struct debug_msg, 5)
#endif


