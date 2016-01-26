/*
 * fifo.h
 *
 *  Created on: Jan 25, 2016
 *      Author: Conor
 */

#ifndef FIFO_H_
#define FIFO_H_

#include "app.h"

#define FIFO_HEADER(NAME, TYPE)\
void NAME##_fifo_init();\
int NAME##_fifo_append(TYPE* _data);\
int NAME##_fifo_get(TYPE* _data)

FIFO_HEADER(debug, struct debug_msg);


#endif /* FIFO_H_ */
