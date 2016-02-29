/*
 * bsp.c
 *
 */
#include <SI_EFM8UB1_Register_Enums.h>
#include <efm8_usb.h>
#include <stdarg.h>
#include <stdio.h>
#include "app.h"
#include "bsp.h"


void u2f_delay(uint16_t ms) {
	uint32_t ms_now = get_ms();
	while((get_ms() - ms_now) < ms)
	{}
}

#ifdef U2F_PRINT

xdata struct debug_msg dbg;

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
// not reentrant
void dump_hex(uint8_t* hex, uint8_t len)
{
	uint8_t i;
	flush_messages();
	for (i=0 ; i < len ; i++)
	{
		u2f_print("%02bx",hex[i]);
		//if ((i%4) == 3)
		//	u2f_print("\r\n");
		flush_messages();

	}
	u2f_print("\r\n");
	flush_messages();
}

void flush_messages()
{
	while(debug_fifo_get(&dbg) == 0)
	{
		u2f_write_s(dbg.buf);
	}
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

void usb_write(uint8_t* buf, uint8_t len)
{
	USBD_Write(EP1IN, buf, len, false);
}








