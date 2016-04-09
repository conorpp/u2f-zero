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

uint8_t custom_command(struct u2f_hid_msg * msg)
{
	struct atecc_response res;
	uint8_t ec;

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

		case U2F_CUSTOM_IDLE_COLOR:

			U2FHID_SET_LEN(msg, 1);
			appconf.idle_color = *((uint32_t*)msg->pkt.init.payload);
			flush_app_conf();
			msg->pkt.init.payload[0] = 1;
			usb_write((uint8_t*)msg, 64);

			break;
		case U2F_CUSTOM_IDLE_COLORP:

			U2FHID_SET_LEN(msg, 1);
			appconf.idle_color_prime = *((uint32_t*)msg->pkt.init.payload);
			flush_app_conf();
			msg->pkt.init.payload[0] = 1;
			usb_write((uint8_t*)msg, 64);

			break;
		case U2F_CUSTOM_PULSE:

			U2FHID_SET_LEN(msg, 1);
			appconf.pulse_period = *((uint16_t*)msg->pkt.init.payload);
			flush_app_conf();
			msg->pkt.init.payload[0] = 1;
			usb_write((uint8_t*)msg, 64);


			break;
		default:
			return 0;
	}
	return 1;
}
