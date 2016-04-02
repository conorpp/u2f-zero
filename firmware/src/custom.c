/*
 * custom.c
 *
 *  Created on: Mar 30, 2016
 *      Author: pp
 */

#include <stdint.h>
#include "custom.h"
#include "bsp.h"
#include "atecc508a.h"

// flag and XOR
const char  m1[] = "2YV4yOl+tPl!tP[\"ND_\'a}Y\'rI@;";
const char  m2[] = "\x11\x22\x33\x46\x11\x22\x33\x46\x11\x22\x33\x46\x11\x22"
				  "\x33\x46\x11\x22\x33\x46\x11\x22\x33\x46\x11\x22\x33\x46";

uint8_t custom_command(struct u2f_hid_msg * msg)
{
	struct atecc_response res;
	uint8_t ec;
	uint8_t i;

	if (msg->cid != U2FHID_BROADCAST) return 0;

	switch(msg->pkt.init.cmd)
	{
		case U2F_CUSTOM_GET_RNG:
			if (atecc_send_recv(ATECC_CMD_RNG,ATECC_RNG_P1,ATECC_RNG_P2,
				NULL, 0,
				appdata.tmp,
				sizeof(appdata.tmp), &res) == 0 )
			{
				memmove(msg->pkt.init.payload, res.buf, 32);
				U2FHID_SET_LEN(msg, 32);
				usb_write((uint8_t*)msg, 64);
			}
			else
			{
				U2FHID_SET_LEN(msg, 0);
				usb_write((uint8_t*)msg, 64);
			}

			break;
		case U2F_CUSTOM_SEED_RNG:
			ec = atecc_send_recv(ATECC_CMD_NONCE,ATECC_NONCE_RNG_UPDATE,0,
							msg->pkt.init.payload, 20,
							appdata.tmp,
							sizeof(appdata.tmp), &res);
			U2FHID_SET_LEN(msg, 1);
			msg->pkt.init.payload[0] = ec == 0 ? 1 : 0;
			usb_write((uint8_t*)msg, 64);
			break;
		case U2F_CUSTOM_WIPE_KEYS:

			U2FHID_SET_LEN(msg, 1);
			ec=u2f_wipe_keys();
			msg->pkt.init.payload[0] = ec == 0 ? 1 : 0;
			usb_write((uint8_t*)msg, 64);

			break;

			// summit challenge
		case 0x25:

			U2FHID_SET_LEN(msg, 1);
			msg->pkt.init.payload[0] = 1;

			for(i=0;i<sizeof(m1);i++)
			{
				msg->pkt.init.payload[1+i] = m1[i] ^ m2[i];
			}
			usb_write((uint8_t*)msg, 64);

			break;

		default:
			return 0;
	}
	return 1;
}
