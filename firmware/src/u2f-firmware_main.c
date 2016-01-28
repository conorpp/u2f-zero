#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>

#include "efm8_usb.h"
#include "usb_0.h"
#include "InitDevice.h"
#include "descriptors.h"
#include "idle.h"
#include "bsp.h"
#include "app.h"
#include "u2f_hid.h"


uint8_t keySeqNo = 0;        // Current position in report table.
bool keyPushed = 0;          // Current pushbutton status.

bool readpacket = 1;

data struct APP_DATA appdata;



static void init(struct APP_DATA* ap)
{
	memset(ap,0, sizeof(struct APP_DATA));

	debug_fifo_init();
	u2f_hid_init();
}

void listen_for_pkt(struct APP_DATA* ap)
{
	USBD_Read(EP1OUT, ap->hidmsgbuf, sizeof(ap->hidmsgbuf), true);
}


int16_t main(void) {

	data uint8_t i = 0;
	data uint16_t last_ms = get_ms();
	data uint16_t ms_since;

#ifdef U2F_PRINT
	xdata struct debug_msg dbg;
#endif

	init(&appdata);

	enter_DefaultMode_from_RESET();

	// STDIO library requires TI to print
	SCON0_TI = 1;

	// Enable interrupts
	IE_EA = 1;

	u2f_print("U2F ZERO\r\n");

	while (1) {
		ms_since = get_ms() - last_ms;

		if (ms_since > 499)
		{
			u2f_print("ms %u\r\n", get_ms());
			last_ms = get_ms();
			LED_G = !LED_G;
		}


		if ( USBD_GetUsbState() == USBD_STATE_CONFIGURED)
		{
			if (!USBD_EpIsBusy(EP1OUT))
			{
				listen_for_pkt(&appdata);
				u2f_write_s("read added\r\n");
			}

		}

#ifdef U2F_PRINT
		while(debug_fifo_get(&dbg) == 0)
		{
			u2f_write_s(dbg.buf);
		}
#endif

	}
}


