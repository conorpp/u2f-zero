#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>

#include "efm8_usb.h"
#include "usb_0.h"
#include "atecc508a.h"
#include "InitDevice.h"
#include "descriptors.h"
#include "eeprom.h"
#include "idle.h"
#include "bsp.h"
#include "app.h"
#include "i2c.h"
#include "u2f_hid.h"
#include "u2f.h"
#include "tests.h"

data struct APP_DATA appdata;

static void init(struct APP_DATA* ap)
{
	memset(ap,0, sizeof(struct APP_DATA));
	u2f_hid_init();
	smb_init();
	atecc_idle();
#ifndef ATECC_SETUP_DEVICE
	eeprom_init();
	u2f_init();
#endif
	U2F_BUTTON_VAL = 1;
}

void set_app_error(APP_ERROR_CODE ec)
{
	appdata.error = ec;
}

void set_app_u2f_hid_msg(struct u2f_hid_msg * msg )
{
	appdata.state = APP_HID_MSG;
	appdata.hid_msg = msg;
}


void rgb(uint8_t r, uint8_t g, uint8_t b)
{
	LED_B(b);
	LED_G(g);
	LED_R(r);
}


#define ms_since(ms,num) (((uint16_t)get_ms() - (ms)) >= num ? (1|(ms=(uint16_t)get_ms())):0)



int16_t main(void) {

	uint16_t ms_heart;
	uint16_t ms_wink;
	uint16_t ms_grad;
	uint8_t winks = 0;
	uint8_t grad_dir = 0;
	uint8_t light = 0;

	enter_DefaultMode_from_RESET();
	init(&appdata);

	// STDIO library requires TI to print
	SCON0_TI = 1;

	// Enable interrupts
	IE_EA = 1;

	u2f_prints("U2F ZERO\r\n");

	run_tests();
	atecc_setup_init(appdata.tmp);


	while (1) {

		if (ms_since(ms_heart,500))
		{
			u2f_printl("ms ", 1, get_ms());
		}


		if (!USBD_EpIsBusy(EP1OUT) && !USBD_EpIsBusy(EP1IN) && appdata.state != APP_HID_MSG)
		{
			USBD_Read(EP1OUT, hidmsgbuf, sizeof(hidmsgbuf), true);
		}

		switch(appdata.state)
		{
			case APP_NOTHING:
				if (ms_since(ms_grad, 10))
				{
					if (light == 90)
					{
						grad_dir = 0;
					}
					else if (light == 0)
					{
						grad_dir = 1;
					}
					if (grad_dir)
						if (U2F_BUTTON_IS_PRESSED())
							rgb(0,0,light++);
						else
							rgb(0,light++,0);
					else
						if (U2F_BUTTON_IS_PRESSED())
							rgb(0,0,light--);
						else
							rgb(0,light--,0);
				}
				break;
			case APP_HID_MSG:
#ifndef ATECC_SETUP_DEVICE
				u2f_hid_request(appdata.hid_msg);
#else
				atecc_setup_device((struct config_msg*)appdata.hid_msg);
#endif
				if (appdata.state == APP_HID_MSG)
					appdata.state = APP_NOTHING;
				break;
			case APP_WINK:
				rgb(0,0,150);
				light = 150;
				ms_wink = get_ms();
				appdata.state = _APP_WINK;
				break;
			case _APP_WINK:

				if (ms_since(ms_wink,150))
				{
					rgb(0,0,light);
					light = light == 0 ? 150 : 0;
					winks++;
				}
				if (winks == 5)
				{
					winks = 0;
					appdata.state = APP_NOTHING;
				}
				break;
		}

		if (appdata.error)
		{
			u2f_printb("error: ", 1, appdata.error);
			appdata.error = 0;
			rgb(200,0,0);
		}


	}
}


