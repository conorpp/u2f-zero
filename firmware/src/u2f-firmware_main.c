#include <SI_EFM8UB1_Register_Enums.h>
#include <stdio.h>

#include "efm8_usb.h"
#include "usb_0.h"
#include "descriptors.h"
#include "idle.h"
#include "bsp.h"
#include "InitDevice.h"

uint8_t keySeqNo = 0;        // Current position in report table.
bool keyPushed = 0;          // Current pushbutton status.

bool readpacket = 1;

SI_SEGMENT_VARIABLE(appdata, struct APP_DATA, SI_SEG_XDATA);

static void init(struct APP_DATA* ap)
{
	memset(ap,0, sizeof(struct APP_DATA));
	ap->EP1_state = EP_FREE;
}

void write_s_tx(char* d)
{
	uint16_t i;
	while(*d)
	{
		// UART0 output queue
		SBUF0 = *d++;
		// 115200 baud , byte time ~ 7*10^-5 s * (48 MHz) ~ 3333 cycles
		for (i=0; i<200; i++);
	}
}

void listen_for_pkt(struct APP_DATA* ap)
{
	if (USBD_Read(EP1OUT, ap->hidmsgbuf, sizeof(ap->hidmsgbuf), true) == USB_STATUS_OK)
	{
		ap->EP1_state = EP_BUSY;
	}
}


int16_t main(void) {

	uint16_t i = 0;
	uint16_t last_ms = get_ms();
	uint16_t ms_since;

	init(&appdata);

	enter_DefaultMode_from_RESET();

	// STDIO library requires TI to print
	SCON0_TI = 1;

	// Enable interrupts
	IE_EA = 1;

	printf("Welcome\n");

	while (1) {
		ms_since = get_ms() - last_ms;

		if (ms_since > 499)
		{
			printf("%d\r\n", ++i);
			last_ms = get_ms();
		}


		if ( USBD_GetUsbState() == USBD_STATE_CONFIGURED)
		{
			if (!USBD_EpIsBusy(EP1OUT))
			{
				listen_for_pkt(&appdata);
				write_s_tx("read added\n");
			}

		}

	}
}


