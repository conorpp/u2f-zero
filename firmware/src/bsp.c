/*
 * bsp.c
 *
 */
#include <stdarg.h>
#include <stdio.h>
#include "app.h"
#include "bsp.h"
#include "fifo.h"


void Delay(uint16_t ms) {
	int16_t x;
	int16_t y;
	for (y = 0; y < ms; y++) {
		for (x = 0; x < 1000; x) {
			x++;
		}
	}
}

void write_s_tx(char* d)
{
	uint16_t i;
	while(*d)
	{
		// UART0 output queue
		SBUF0 = *d++;
		// 115200 baud , byte time ~ 7*10^-5 s * (48 MHz) ~ 3333 cycles
		for (i=0; i<200; i++);
	}
}



void print_u2f(const char* fmt, ...)
{
	va_list args;
	struct debug_msg dbg;

	va_start(args,fmt);
	dbg.arglist = args;
	dbg.fmt = fmt;
	debug_fifo_append(&dbg);
	va_end(argptr);
	printf("u2f append\r\n");
}
