/*
 * app.h
 *
 */

#ifndef APP_H_
#define APP_H_

#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>

#include "fifo.h"

// debug options
#define U2F_PRINT


typedef enum
{
	EP_BUSY = 0,
	EP_FREE,
}
ENDPOINT_STATE;


struct APP_DATA
{
	uint8_t hidmsgbuf[64];
};

struct debug_msg
{
	char buf[40];
};

#endif /* APP_H_ */
