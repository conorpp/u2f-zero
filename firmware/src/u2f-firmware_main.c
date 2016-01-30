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


#ifdef U2F_PRINT
FIFO_CREATE(debug,struct debug_msg, 5)
#endif

static void init(struct APP_DATA* ap)
{
	memset(ap,0, sizeof(struct APP_DATA));

	ap->state = APP_NOTHING;
	debug_fifo_init();
	u2f_hid_init();
}

void listen_for_pkt(struct APP_DATA* ap)
{
	USBD_Read(EP1OUT, ap->hidmsgbuf, sizeof(ap->hidmsgbuf), true);
}

#define ms_since(ms,num) ((get_ms() - (ms)) >= num ? (1|(ms=get_ms())):0)

int16_t main(void) {

	data uint8_t i = 0;
	data uint16_t last_ms = get_ms();
	data uint16_t ms_heart;
	data uint16_t ms_wink;
	uint8_t winks = 0;

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

		if (ms_since(ms_heart,500))
		{
			u2f_print("ms %u\r\n", get_ms());
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

		switch(appdata.state)
		{
			case APP_NOTHING:
				break;
			case APP_WINK:
				LED_B = 0;
				ms_wink = get_ms();
				appdata.state = _APP_WINK;
				break;
			case _APP_WINK:
				if (ms_since(ms_wink,150))
				{
					LED_B = !LED_B;
					winks++;
				}
				if (winks == 5)
				{
					winks = 0;
					appdata.state = APP_NOTHING;
				}

				break;
		}

#ifdef U2F_PRINT
		while(debug_fifo_get(&dbg) == 0)
		{
			u2f_write_s(dbg.buf);
		}
#endif

	}
}


