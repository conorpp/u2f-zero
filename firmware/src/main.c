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
#include "custom.h"
#include "u2f_hid.h"
#include "u2f.h"
#include "tests.h"

data struct APP_DATA appdata;


uint8_t error;
uint8_t state;
uint8_t winkr,winkb,winkg;
struct u2f_hid_msg * hid_msg;

static void init(struct APP_DATA* ap)
{
	memset(ap,0, sizeof(struct APP_DATA));
	u2f_hid_init();
	smb_init();
	//atecc_idle();
#ifndef ATECC_SETUP_DEVICE
	eeprom_init();
	u2f_init();
#endif
	U2F_BUTTON_VAL = 1;
	state = APP_NOTHING;
	error = ERROR_NOTHING;
}

void set_app_error(APP_ERROR_CODE ec)
{
	error = ec;
}

uint8_t get_app_error()
{
	return error;
}

uint8_t get_app_state()
{
	return state;
}

void set_app_state(APP_STATE s)
{
	state = s;
}

void app_wink(uint8_t r, uint8_t g, uint8_t b)
{
	winkr = r;
	winkb = g;
	winkg = b;
	set_app_state(APP_WINK);
}

void set_app_u2f_hid_msg(struct u2f_hid_msg * msg )
{
	state = APP_HID_MSG;
	hid_msg = msg;
}


void rgb(uint8_t r, uint8_t g, uint8_t b)
{
	LED_B(b);
	LED_G(g);
	LED_R(r);
}

xdata const char A_HIDDEN_JEWEL3[] = "AAAAAAAAAAAAAAAA#{1dont_f0rget_to_lock_y3r_progr@m}AAAAA";

#define ms_since(ms,num) (((uint16_t)get_ms() - (ms)) >= num ? ((ms=(uint16_t)get_ms())):0)

int16_t main(void) {
	uint16_t ms_heart;
	uint16_t ms_wink;
	uint16_t ms_grad;
	uint8_t winks = 0;
	uint8_t grad_dir = 0;
	uint8_t light = 0;
	uint8_t p= 0;

	enter_DefaultMode_from_RESET();

	// ~200 ms interval watchdog
	WDTCN = 4;

	watchdog();
	init(&appdata);

	// Enable interrupts
	IE_EA = 1;
	watchdog();

	u2f_prints("U2F ZERO\r\n");
	if (RSTSRC & RSTSRC_WDTRSF__SET)
	{
		error = ERROR_DAMN_WATCHDOG;
	}
	//run_tests();
	atecc_setup_init(appdata.tmp);

	while (1) {
		watchdog();

		if (ms_since(ms_heart,500))
		{
			u2f_printl("ms ", get_ms());
		}
		memmove(appdata.tmp,A_HIDDEN_JEWEL3, sizeof(A_HIDDEN_JEWEL3));

		if (!USBD_EpIsBusy(EP1OUT) && !USBD_EpIsBusy(EP1IN) && state != APP_HID_MSG)
		{
			USBD_Read(EP1OUT, hidmsgbuf, sizeof(hidmsgbuf), true);
		}

		switch(state)
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
				if (custom_command(hid_msg))
				{

				}
				else
				{
					u2f_hid_request(hid_msg);
				}

				if (state == APP_HID_MSG)
					state = APP_NOTHING;
				break;
			case APP_WINK:
				rgb(winkr,winkg,winkb);
				light = 1;
				ms_wink = get_ms();
				state = _APP_WINK;
				break;
			case _APP_WINK:

				if (ms_since(ms_wink,150))
				{
					if (light)
					{
						light = 0;
						rgb(winkr,winkg,winkb);
					}
					else
					{
						light = 1;
						rgb(0,0,0);
					}
					winks++;
				}
				if (winks == 5)
				{
					winks = 0;
					state = APP_NOTHING;
				}
				break;
		}

		if (error)
		{
			u2f_printb("error: ", 1, error);
			error = 0;
			rgb(200,0,0);
			while(!ms_since(ms_heart,2000))
			{
				watchdog();
			}
		}


	}
}


