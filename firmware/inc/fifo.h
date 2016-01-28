/*
 * fifo.h
 *
 *  Created on: Jan 25, 2016
 *      Author: Conor
 */

#ifndef FIFO_H_
#define FIFO_H_



#define FIFO_HEADER(NAME, TYPE)\
void NAME##_fifo_init();\
int NAME##_fifo_append(TYPE* _data);\
int NAME##_fifo_get(TYPE* _data)


#define U2F_PRINT
#ifdef U2F_PRINT
FIFO_HEADER(debug, struct debug_msg);
#else
#define debug_fifo_init(x)
#define debug_fifo_get(x)
#define debug_fifo_append(x)
#endif


#endif /* FIFO_H_ */
