/*
 * app.h
 *
 */

#ifndef APP_H_
#define APP_H_

#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>

SI_SBIT (SDA, SFR_P1, 2);              // SMBus on P1.2
SI_SBIT(SCL, SFR_P1, 3);               // and P1.3

SI_SBIT (LED1, SFR_P1, 4);             // LED green

// debug options
#define U2F_PRINT
void flush_messages();

typedef enum
{
	APP_NOTHING = 0,
	APP_WINK,
	_APP_WINK,
}
APP_STATE;

struct APP_DATA
{
	uint8_t hidmsgbuf[64];
	uint8_t state;
};

extern data struct APP_DATA appdata;

struct debug_msg
{
	char buf[40];
};



#define FIFO_HEADER(NAME, TYPE)\
void NAME##_fifo_init();\
int NAME##_fifo_append(TYPE* _data);\
int NAME##_fifo_get(TYPE* _data)


#ifdef U2F_PRINT
FIFO_HEADER(debug, struct debug_msg);
#else
#define debug_fifo_init(x)
#define debug_fifo_get(x)
#define debug_fifo_append(x)
#endif

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


#endif /* APP_H_ */
