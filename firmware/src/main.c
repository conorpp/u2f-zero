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
	// eeprom_init();
	u2f_init();
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


//void dump_eeprom()
//{
//	// 0xF800 - 0xFB7F
//	uint16_t i = 0xF800;
//	uint8_t eep;
//	for (; i <= 0xF800 + 4 * 16; i++)
//	{
//		eeprom_read(i,&eep,1);
//		u2f_putb(eep);
//		u2f_prints(" ");
//	}
//	u2f_prints("\r\n");
//}

int8_t test_app()
{
	struct atecc_response res;
	u2f_prints("atecc_send_recv\r\n");
	atecc_send_recv(ATECC_CMD_COUNTER,
			ATECC_COUNTER_INC, ATECC_COUNTER0,NULL,0,
			appdata.tmp, 12, &res);
	dump_hex(appdata.tmp, res.len+3);
	return 0;
}

#define ms_since(ms,num) (((uint16_t)get_ms() - (ms)) >= num ? (1|(ms=(uint16_t)get_ms())):0)



int16_t main(void) {

	data uint8_t i = 0;
	data uint16_t last_ms = get_ms();
	data uint16_t ms_heart;
	data uint16_t ms_wink;
	data uint8_t winks = 0;
	uint8_t leds = 0;
	uint16_t button = 0;

	enter_DefaultMode_from_RESET();
	init(&appdata);

	// STDIO library requires TI to print
	SCON0_TI = 1;

	// Enable interrupts
	IE_EA = 1;

	u2f_prints("U2F ZERO\r\n");

	run_tests();
	atecc_setup_device(appdata.tmp);
	test_app();

	while (1) {

		if (ms_since(ms_heart,500))
		{
			u2f_printl("ms ", 1, get_ms());
			button = U2F_BUTTON;
			u2f_printx("button: ",1,button);
			LED_G = !LED_G;
		}


		if (!USBD_EpIsBusy(EP1OUT) && !USBD_EpIsBusy(EP1IN) && appdata.state != APP_HID_MSG)
		{
			USBD_Read(EP1OUT, hidmsgbuf, sizeof(hidmsgbuf), true);
			// u2f_prints("read added\r\n");
		}

		switch(appdata.state)
		{
			case APP_NOTHING:
				break;
			case APP_HID_MSG:
				u2f_hid_request(appdata.hid_msg);
				if (appdata.state == APP_HID_MSG)
					appdata.state = APP_NOTHING;
				break;
			case APP_WINK:
				LED_B = 0;
				ms_wink = get_ms();
				appdata.state = _APP_WINK;
				break;
			case _APP_WINK:
				LED_G = 1;
				LED_R = 1;
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

		if (appdata.error)
		{
			u2f_printb("error: ", 1, appdata.error);
			appdata.error = 0;
		}


	}
}


