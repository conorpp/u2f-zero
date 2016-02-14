/*
 * bsp.c
 *
 */
#include <stdarg.h>
#include <stdio.h>
#include "app.h"
#include "bsp.h"


void u2f_delay(uint16_t ms) {
	volatile int16_t x;
	volatile int16_t y;
	for (y = 0; y < ms; y++) {
		for (x = 0; x < 1000; x) {
			x++;
		}
	}
}

#ifdef U2F_PRINT
void u2f_print(char* fmt, ...)
{

	struct debug_msg dbg;
	va_list args;

	va_start(args,fmt);
	if(vsprintf(dbg.buf, fmt, args) > sizeof(struct debug_msg)-1)
	{
		u2f_write_s("vsprintf stack corrupt!\r\n");
	}
	va_end(args);
	debug_fifo_append(&dbg);
}
#endif


void putf(void* _notused, char c)
{
	xdata int i;
	SBUF0 = c;
	for (i=0; i<400; i++);
}

void u2f_write_s(char* d)
{
	data uint16_t i;
	while(*d)
	{
		// UART0 output queue
		putf(NULL,*d++);

	}
}



