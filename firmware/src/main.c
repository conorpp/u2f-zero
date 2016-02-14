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

data struct APP_DATA appdata;


#ifdef U2F_PRINT

	FIFO_CREATE(debug,struct debug_msg, 5)

	static xdata struct debug_msg dbg;

	void flush_messages()
	{
		while(debug_fifo_get(&dbg) == 0)
		{
			u2f_write_s(dbg.buf);
		}
	}

#else

	#define flush_messages(x)

#endif

static void init(struct APP_DATA* ap)
{
	memset(ap,0, sizeof(struct APP_DATA));

	ap->state = APP_NOTHING;
	debug_fifo_init();
	u2f_hid_init();

	smb_init();

}

static void listen_for_pkt(struct APP_DATA* ap)
{
	USBD_Read(EP1OUT, ap->hidmsgbuf, sizeof(ap->hidmsgbuf), true);
}




// not reentrant
static void dump_hex(uint8_t* hex, uint8_t len)
{
	uint8_t i;
	flush_messages();
	for (i=0 ; i < len ; i++)
	{
		u2f_print(" %02bx",hex[i]);
		flush_messages();
	}
	u2f_print("\r\n");
	flush_messages();
}

void test_ecc508a()
{
	uint8_t buf[40];
	uint8_t len;
//	do{
//		atecc_send(ATECC_CMD_COUNTER,
//				ATECC_COUNTER_READ,
//				ATECC_COUNTER1,NULL,0);
//	}while((len = atecc_recv(buf,sizeof(buf))) < 0);

//	do{
//		atecc_send(ATECC_CMD_RNG,
//				ATECC_RNG_P1,
//				ATECC_RNG_P2,NULL,0);
//	}while((len = atecc_recv(buf,sizeof(buf))) < 0);
	char pw[] = "2pckJ4IkT3PwdGMwuCygPpxD6+lObNGORiLGPQxM4ef4YoNvx9/k0xskZl84rCd3TllCvitepe+B";
	do{
		atecc_send(ATECC_CMD_SHA,
				ATECC_SHA_START,
				0,NULL,0);
	}while((len = atecc_recv(buf,sizeof(buf))) < 0);
	dump_hex(buf,len);

	do{
		atecc_send(ATECC_CMD_SHA,
				ATECC_SHA_UPDATE,
				64,pw,64);
	}while((len = atecc_recv(buf,sizeof(buf))) < 0);
	dump_hex(buf,len);

	do{
		atecc_send(ATECC_CMD_SHA,
				ATECC_SHA_END,
				sizeof(pw)-65,pw+64,sizeof(pw)-65);
	}while((len = atecc_recv(buf,sizeof(buf))) < 0);
	dump_hex(buf,len);

	// sha256 sum should be bcddd71b48f8a31d1374ad51c2e4138a871cb7f1eb3f2bdab49bc9bc60afc3a5

}

#define ms_since(ms,num) (((uint16_t)get_ms() - (ms)) >= num ? (1|(ms=(uint16_t)get_ms())):0)

int16_t main(void) {

	data uint8_t i = 0;
	data uint16_t last_ms = get_ms();
	data uint16_t ms_heart;
	data uint16_t ms_wink;
	uint8_t winks = 0;
	uint8_t test = 0;


	init(&appdata);

	enter_DefaultMode_from_RESET();

	// STDIO library requires TI to print
	SCON0_TI = 1;

	// Enable interrupts
	IE_EA = 1;

	u2f_print("U2F ZERO\r\n");


	while (1) {

		if (!test)
		{
			test_ecc508a();
			test_ecc508a();
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
		}

		flush_messages();

	}
}


