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
data struct APP_CONF appconf;

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

void flush_app_conf()
{
	eeprom_erase(U2F_APP_CONFIG);
	eeprom_write(U2F_APP_CONFIG, (uint8_t* )&appconf, sizeof(struct APP_CONF));
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


void rgb(uint8_t * c)
{

	if (c[0])
	{
		PCA0CPM2 |= PCA0CPM2_PWM__ENABLED;
		LED_R(c[0]);
	}
	else
	{
		PCA0CPM2 &= ~PCA0CPM2_PWM__ENABLED;
	}

	if (c[2])
	{
		PCA0CPM0 |= PCA0CPM0_PWM__ENABLED;
		LED_B(c[2]);
	}
	else
	{
		PCA0CPM0 &= ~PCA0CPM0_PWM__ENABLED;
	}

	if (c[1])
	{
		PCA0CPM1 |= PCA0CPM1_PWM__ENABLED;
		LED_G(c[1]);
	}
	else
	{
		PCA0CPM1 &= ~PCA0CPM1_PWM__ENABLED;
	}

}

void rgb_hex(uint32_t c)
{
	color[0] = c;
	color[1] = c>>8;
	color[2] = c>>16;
	current_color = c;
	rgb(color);
	color_max = color;
	if (*color_max < color[1]) color_max++;
	if (*color_max < color[2]) color_max = color + 2;
	brightness = *color_max;
}


#define ms_since(ms,num) (((uint16_t)get_ms() - (ms)) >= num ? ((ms=(uint16_t)get_ms())):0)

int16_t main(void) {

	uint16_t ms_heart;
	uint16_t ms_wink;
	uint16_t ms_grad;
	uint8_t winks = 0, light;
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

	rgb_hex(appconf.idle_color);


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
				if (ms_since(ms_grad, appconf.pulse_period))
				{
					if (U2F_BUTTON_IS_PRESSED())
					{
						if (appconf.idle_color_prime != current_color)
						{
							rgb_hex(appconf.idle_color_prime);

						}
					}
					else
					{
						if (appconf.idle_color != current_color)
						{
							rgb_hex(appconf.idle_color);
						}
					}

					if (*color_max >= brightness)
					{
						grad_inc = -1;
					}
					else if (*color_max == 0)
					{
						grad_inc = 1;
					}
					if (brightness != 0)
					{
						*color_max += grad_inc;
					}
					rgb(color);

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
			u2f_printb("error: ", 1, error);
			error = 0;
			rgb_hex(U2F_DEFAULT_COLOR_ERROR);
			while(!ms_since(ms_heart,2000))
			{
				watchdog();
			}
		}


	}
}


