#include <SI_EFM8UB1_Register_Enums.h>
#include <stdio.h>

#include "efm8_usb.h"
#include "usb_0.h"
#include "InitDevice.h"
#include "descriptors.h"
#include "idle.h"
#include "bsp.h"
#include "app.h"


uint8_t keySeqNo = 0;        // Current position in report table.
bool keyPushed = 0;          // Current pushbutton status.

bool readpacket = 1;

SI_SEGMENT_VARIABLE(appdata, struct APP_DATA, SI_SEG_XDATA);

static void init(struct APP_DATA* ap)
{
	memset(ap,0, sizeof(struct APP_DATA));

	debug_fifo_init();
}



void listen_for_pkt(struct APP_DATA* ap)
{
	USBD_Read(EP1OUT, ap->hidmsgbuf, sizeof(ap->hidmsgbuf), true);
}


int16_t main(void) {

	uint16_t i = 0;
	uint16_t last_ms = get_ms();
	uint16_t ms_since;
	struct debug_msg dbg;

	init(&appdata);

	enter_DefaultMode_from_RESET();

	// STDIO library requires TI to print
	SCON0_TI = 1;

	// Enable interrupts
	IE_EA = 1;

	printf("Welcome\r\n");

	while (1) {
		ms_since = get_ms() - last_ms;

		if (ms_since > 499)
		{
			printf("%u ms\r\n", get_ms());
			last_ms = get_ms();
			LED_G = !LED_G;
		}


		if ( USBD_GetUsbState() == USBD_STATE_CONFIGURED)
		{
			if (!USBD_EpIsBusy(EP1OUT))
			{
				listen_for_pkt(&appdata);
				printf("read added\r\n");
			}

		}

		while(debug_fifo_get(&dbg)==0)
		{
			va_start(dbg.arglist,dbg.fmt);
			vprintf(dbg.fmt,dbg.arglist);
			va_end(dbg.arglist);
		}

	}
}


