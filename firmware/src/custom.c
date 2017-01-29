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

 *
 *
 * These are commands are "middleware" for HID messages.  So they don't need
 * the HID U2F layer to be called.  Thus they are custom commands not necessary for U2F.
 *
 */

#include <stdint.h>
#include "custom.h"
#include "bsp.h"
#include "atecc508a.h"

uint8_t custom_command(struct u2f_hid_msg * msg)
{
	struct atecc_response res;
	uint8_t ec;

	switch(msg->pkt.init.cmd)
	{
#ifdef U2F_SUPPORT_RNG_CUSTOM
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
#endif
#ifdef U2F_SUPPORT_SEED_CUSTOM
		case U2F_CUSTOM_SEED_RNG:
			ec = atecc_send_recv(ATECC_CMD_NONCE,ATECC_NONCE_RNG_UPDATE,0,
							msg->pkt.init.payload, 20,
							appdata.tmp,
							sizeof(appdata.tmp), &res);
			U2FHID_SET_LEN(msg, 1);
			msg->pkt.init.payload[0] = ec == 0 ? 1 : 0;
			usb_write((uint8_t*)msg, 64);
			break;
#endif
#ifdef U2F_SUPPORT_WINK
		case U2F_CUSTOM_WINK:

			app_wink(U2F_COLOR_WINK);

			break;
#endif
		case U2F_CONFIG_BOOTLOADER:

			atecc_send_recv(ATECC_CMD_READ,
					ATECC_RW_DATA, ATECC_EEPROM_DATA_SLOT(8), NULL, 0,
					appdata.tmp, sizeof(appdata.tmp), &res);

			if (res.buf[0] == 0xff)
			{
				*((uint8_t SI_SEG_DATA *)0x00) = 0xA5;
				RSTSRC = RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET;
			}


			break;
		case U2F_CONFIG_BOOTLOADER_DESTROY:

			memset(appdata.tmp,0,4);

			atecc_send_recv(ATECC_CMD_WRITE,
					ATECC_RW_DATA, ATECC_EEPROM_DATA_SLOT(8), appdata.tmp, 4,
					appdata.tmp, sizeof(appdata.tmp), &res);

			break;
		default:
			return 0;
	}
	return 1;
}
