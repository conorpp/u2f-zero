#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>

#include "efm8_usb.h"
#include "usb_0.h"
#include "atecc508a.h"
#include "InitDevice.h"
#include "descriptors.h"
#include "idle.h"
#include "bsp.h"
#include "app.h"
#include "i2c.h"
#include "u2f_hid.h"
#include "tests.h"


data struct APP_DATA appdata;


#ifdef U2F_PRINT

	FIFO_CREATE(debug,struct debug_msg, 5)



#endif

static void init(struct APP_DATA* ap)
{
	memset(ap,0, sizeof(struct APP_DATA));
	debug_fifo_init();
	u2f_hid_init();
	smb_init();
	atecc_idle();
}

static void listen_for_pkt(struct APP_DATA* ap)
{
	USBD_Read(EP1OUT, ap->hidmsgbuf, sizeof(ap->hidmsgbuf), true);
}


void set_app_error(APP_ERROR_CODE ec)
{
	appdata.state = APP_ERROR;
	appdata.error = ec;
}

int8_t test_ecc508a()
{
	struct atecc_response res;
	uint8_t buf[72];
	char msg[] = "wow a signature!\n";

	atecc_send_recv(ATECC_CMD_SHA,
			ATECC_SHA_START, 0, NULL, 0,
			buf, sizeof(buf), &res);

	atecc_send_recv(ATECC_CMD_SHA,
			ATECC_SHA_END, sizeof(msg)-1, msg, sizeof(msg)-1,
			buf, sizeof(buf), &res);

	dump_hex(res.buf, res.len);

	u2f_print("sig:\r\n");

	atecc_send_recv(ATECC_CMD_SIGN,
			ATECC_SIGN_EXTERNAL, 0, NULL, 0,
			buf, sizeof(buf), &res);
	dump_hex(res.buf, res.len);

	return 0;
}

#define ms_since(ms,num) (((uint16_t)get_ms() - (ms)) >= num ? (1|(ms=(uint16_t)get_ms())):0)

int16_t main(void) {

	uint8_t zeros[] = {0,0,0,0};
	data uint8_t i = 0;
	data uint16_t last_ms = get_ms();
	data uint16_t ms_heart;
	data uint16_t ms_wink;
	data uint8_t winks = 0;
	data uint8_t test = 0;

	enter_DefaultMode_from_RESET();
	init(&appdata);



	// STDIO library requires TI to print
	SCON0_TI = 1;

	// Enable interrupts
	IE_EA = 1;

	u2f_print("U2F ZERO\r\n");


	while (1) {

		if (!test)
		{
			run_tests();
			test_ecc508a();
			test = 1;
		}

		if (ms_since(ms_heart,500))
		{
			u2f_print("ms %lu\r\n", get_ms());
			LED_G = !LED_G;

		}


		if ( USBD_GetUsbState() == USBD_STATE_CONFIGURED)
		{
			if (!USBD_EpIsBusy(EP1OUT))
			{
				listen_for_pkt(&appdata);
				u2f_print("read added\r\n");
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
			case APP_ERROR:
				u2f_print("error: %bx\r\n", appdata.error);
				appdata.state = APP_NOTHING;
				break;
		}

		flush_messages();

	}
}


