/*
 * app.h
 *
 */

#ifndef APP_H_
#define APP_H_

#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>

#include "fifo.h"
#include "bsp.h"

typedef enum
{
	EP_BUSY = 0,
	EP_FREE,
}
ENDPOINT_STATE;


struct APP_DATA
{
	uint8_t hidmsgbuf[64];

	// ms
	uint16_t usb_read_timeout;
};

struct debug_msg
{
	const char* fmt;
	va_list arglist;
};

#endif /* APP_H_ */
