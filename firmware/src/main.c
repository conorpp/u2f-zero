/*
 * Copyright (c) 2016, Conor Patrick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */
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
uint8_t winkc;
struct u2f_hid_msg * hid_msg;

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

void app_wink(uint32_t c)
{
	winkc = c;
	set_app_state(APP_WINK);
}

void set_app_u2f_hid_msg(struct u2f_hid_msg * msg )
{
	state = APP_HID_MSG;
	hid_msg = msg;
}

uint8_t * color_max;
static uint8_t color[3];
static uint32_t current_color;
static uint8_t brightness = 90;


void rgb(uint8_t r, uint8_t g, uint8_t b)
{

	if (r)
	{
		PCA0CPM2 |= PCA0CPM2_PWM__ENABLED;
		LED_R(r);
	}
	else
	{
		PCA0CPM2 &= ~PCA0CPM2_PWM__ENABLED;
	}

	if (b)
	{
		PCA0CPM0 |= PCA0CPM0_PWM__ENABLED;
		LED_B(b);
	}
	else
	{
		PCA0CPM0 &= ~PCA0CPM0_PWM__ENABLED;
	}

	if (g)
	{
		PCA0CPM1 |= PCA0CPM1_PWM__ENABLED;
		LED_G(g);
	}
	else
	{
		PCA0CPM1 &= ~PCA0CPM1_PWM__ENABLED;
	}

}

void rgb_hex(uint32_t c)
{
	rgb(c,c>>8,c>>16);
}


#define ms_since(ms,num) (((uint16_t)get_ms() - (ms)) >= num ? ((ms=(uint16_t)get_ms())):0)

int16_t main(void) {

	uint16_t ms_heart;
	uint16_t ms_wink;
	uint16_t ms_grad;
	uint8_t winks = 0, light, grad_dir = 0;
	int8_t grad_inc = 0;

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
	run_tests();
	atecc_setup_init(appdata.tmp);

	rgb_hex(U2F_COLOR);


	while (1) {
		watchdog();



		if (ms_since(ms_heart,500))
		{
			u2f_printl("ms ", 1, get_ms());
		}

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
				rgb_hex(winkc);
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
						rgb_hex(winkc);
					}
					else
					{
						light = 1;
						rgb_hex(0);
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
			u2f_printx("error: ", 1, (uint16_t)error);
			error = 0;
			rgb_hex(U2F_DEFAULT_COLOR_ERROR);
			while(!ms_since(ms_heart,2000))
			{
				watchdog();
			}
		}


	}
}


