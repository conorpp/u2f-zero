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


void putf(char c)
{
	uint8_t i;
	SBUF0 = c;
	for (i=0; i<200; i++){}
	for (i=0; i<200; i++){}
	for (i=0; i<200; i++){}
}

void u2f_write_s(char* d)
{
	static SI_SEG_DATA uint16_t i;
	while(*d)
	{
		// UART0 output queue
		putf(*d++);

	}
}

static void int2str_reduce_10(char ** snum, uint32_t copy)
{
    do
    {
        *snum++;
        copy /= 10;
    }while(copy);
}

static void int2str_reduce_16(char ** snum, uint32_t copy)
{
    do
    {
        *snum++;
        copy >>= 4;
    }while(copy);
}

static const char * __digits = "0123456789abcdef";
static char __int2str_buf[9];

static void int2str_map_10(char ** snum, uint32_t i)
{
    do
    {
        *--*snum = __digits[i % 10];
        i /= 10;
    }while(i);
}

static void int2str_map_16(char ** snum, uint32_t i)
{
    do
    {
        *--*snum = __digits[i & 0xf];
        i >>= 4;
    }while(i);
}

#define dint2str(i)     __int2str10(i)
#define xint2str(i)     __int2str16(i)

char * __int2str10(int32_t i)
{
    char * snum = __int2str_buf;
    if (i<0) *snum++ = '-';
    int2str_reduce_10(&snum, i);
    *snum = '\0';
    int2str_map_10(&snum, i);
    return snum;
}

char * __int2str16(int32_t i)
{
    char * snum = __int2str_buf;
    if (i<0) *snum++ = '-';
    int2str_reduce_16(&snum, i);
    *snum = '\0';
    int2str_map_16(&snum, i);
    return snum;
}


void u2f_putb(uint8_t i)
{
    u2f_write_s(xint2str((uint32_t)i));
}

void u2f_putd(int16_t i)
{
    u2f_write_s(dint2str((int32_t)i));
}

void u2f_putx(uint16_t i)
{
    u2f_write_s(xint2str((int32_t)i));
}

void u2f_putl(int32_t i)
{
	u2f_write_s(dint2str((int32_t)i));
}
void u2f_putlx(int32_t i)
{
	u2f_write_s(xint2str((int32_t)i));
}

void u2f_printd(const char * tag, uint8_t c, ...)
{
	va_list args;
    u2f_write_s(tag);
    va_start(args,c);
    while(c--)
    {
        u2f_putd(va_arg(args, int16_t));
        u2f_write_s(" ");
    }
    u2f_write_s("\r\n");
    va_end(args);
}

void u2f_printl(const char * tag, uint8_t c, ...)
{
    va_list args;
    u2f_write_s(tag);
    va_start(args,c);
    while(c--)
    {
        u2f_putl(va_arg(args, int32_t));
        u2f_write_s(" ");
    }
    u2f_write_s("\r\n");
    va_end(args);
}

void u2f_printx(const char * tag, uint8_t c, ...)
{
    va_list args;
    u2f_write_s(tag);
    va_start(args,c);
    while(c--)
    {
        u2f_putx(va_arg(args, uint16_t));
        u2f_write_s(" ");
    }
    u2f_write_s("\r\n");
    va_end(args);
}

void u2f_printb(const char * tag, uint8_t c, ...)
{
	va_list args;
    u2f_write_s(tag);
    va_start(args,c);
    while(c--)
    {
        u2f_putb(va_arg(args, uint8_t));
        u2f_write_s(" ");
    }
    u2f_write_s("\r\n");
    va_end(args);
}

void u2f_printlx(const char * tag, uint8_t c, ...)
{
    va_list args;
    u2f_write_s(tag);
    va_start(args,c);
    while(c--)
    {
        u2f_putlx(va_arg(args, int32_t));
        u2f_write_s(" ");
    }
    u2f_write_s("\r\n");
    va_end(args);
}

void usb_write(uint8_t* buf, uint8_t len)
{
	int16_t ec;
	if (USB_STATUS_OK != (ec=USBD_Write(EP1IN, buf, len, false)))
	{
		u2f_printd("USB error",1, -ec);
	}
}








