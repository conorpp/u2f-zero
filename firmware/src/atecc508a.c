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
 * atecc508.c
 * 		Implementation for ATECC508 peripheral.
 *
 */
#include <endian.h>
#include <stdint.h>
#include "app.h"
#include "atecc508a.h"
#include "i2c.h"
#include "eeprom.h"

#include "bsp.h"



int8_t atecc_send(uint8_t cmd, uint8_t p1, uint16_t p2,
					uint8_t * buf, uint8_t len)
{
	static data uint8_t params[6];
	params[0] = 0x3;
	params[1] = 7+len;
	params[2] = cmd;
	params[3] = p1;
	params[4] = ((uint8_t*)&p2)[1];
	params[5] = ((uint8_t* )&p2)[0];

	smb_set_ext_write(buf, len);
	smb_write( ATECC508A_ADDR, params, sizeof(params));
	if (SMB_WAS_NACKED())
	{
		return -1;
	}
	return 0;
}

void atecc_idle()
{
	smb_write( ATECC508A_ADDR, "\x02", 1);
}

void atecc_sleep()
{
	smb_write( ATECC508A_ADDR, "\x01", 1);
}

void atecc_wake()
{
	smb_write( ATECC508A_ADDR, "\0\0", 2);
}

#define PKT_CRC(buf, pkt_len) (htole16(*((uint16_t*)(buf+pkt_len-2))))

int8_t atecc_recv(uint8_t * buf, uint8_t buflen, struct atecc_response* res)
{
	uint8_t pkt_len;
	pkt_len = smb_read( ATECC508A_ADDR,buf,buflen);
	if (SMB_WAS_NACKED())
	{
		return -1;
	}

	if (SMB_FLAGS & SMB_READ_TRUNC)
	{
		set_app_error(ERROR_READ_TRUNCATED);
		return -1;
	}

	if (pkt_len <= buflen && pkt_len >= 4)
	{
		if (PKT_CRC(buf,pkt_len) != SMB_crc)
		{
			set_app_error(ERROR_I2C_CRC);
			return -1;
		}
	}
	else
	{
		set_app_error(ERROR_I2C_BAD_LEN);
		return -1;
	}

	if (pkt_len == 4 && buf[1] != 0)
	{
		set_app_error(buf[1]);
		return -1;
	}

	if (res != NULL)
	{
		res->len = pkt_len - 3;
		res->buf = buf+1;
	}
	return pkt_len;
}

static void delay_cmd(uint8_t cmd)
{
	uint8_t d = 0;
	switch(cmd)
	{
		case ATECC_CMD_SIGN:
			d = 50;
			break;
		case ATECC_CMD_GENKEY:
			d = 100;
			break;
		default:
			d = 32;
			break;
	}
	u2f_delay(d);
}

int8_t atecc_send_recv(uint8_t cmd, uint8_t p1, uint16_t p2,
							uint8_t* tx, uint8_t txlen, uint8_t * rx,
							uint8_t rxlen, struct atecc_response* res)
{
	uint8_t errors = 0;
	atecc_wake();
	resend:
	while(atecc_send(cmd, p1, p2, tx, txlen) == -1)
	{
		u2f_delay(10);
		errors++;
		if (errors > 8)
		{
			return -1;
		}
	}
	while(atecc_recv(rx,rxlen, res) == -1)
	{
		errors++;
		if (errors > 5)
		{
			return -2;
		}
		switch(get_app_error())
		{
			case ERROR_NOTHING:
				delay_cmd(cmd);
				break;
			default:
				u2f_delay(cmd);
				goto resend;
				break;
		}

	}
	atecc_idle();
	return 0;
}


#ifdef ATECC_SETUP_DEVICE

int8_t atecc_write_eeprom(uint8_t base, uint8_t offset, uint8_t* srcbuf, uint8_t len)
{
	uint8_t buf[7];
	struct atecc_response res;

	uint8_t * dstbuf = srcbuf;
	if (offset + len > 4)
		return -1;
	if (len < 4)
	{
		atecc_send_recv(ATECC_CMD_READ,
				ATECC_RW_CONFIG, base, NULL, 0,
				buf, sizeof(buf), &res);

		dstbuf = res.buf;
		memmove(res.buf + offset, srcbuf, len);
	}

	atecc_send_recv(ATECC_CMD_WRITE,
			ATECC_RW_CONFIG, base, dstbuf, 4,
			buf, sizeof(buf), &res);

	if (res.buf[0])
	{
		set_app_error(-res.buf[0]);
		return -1;
	}
	return 0;
}



static uint8_t shabuf[70];
static uint8_t shaoffset = 0;
uint8_t SHA_FLAGS = 0;
uint8_t SHA_HMAC_KEY = 0;
static struct atecc_response res_digest;

void u2f_sha256_start()
{
	shaoffset = 0;
	atecc_send_recv(ATECC_CMD_SHA,
			SHA_FLAGS, SHA_HMAC_KEY,NULL,0,
			shabuf, sizeof(shabuf), NULL);
	SHA_HMAC_KEY = 0;
}


void u2f_sha256_update(uint8_t * buf, uint8_t len)
{
	uint8_t i = 0;
	watchdog();
	while(len--)
	{
		shabuf[shaoffset++] = *buf++;
		if (shaoffset == 64)
		{
			atecc_send_recv(ATECC_CMD_SHA,
					ATECC_SHA_UPDATE, 64,shabuf,64,
					shabuf, sizeof(shabuf), NULL);
			shaoffset = 0;
		}
	}
}


void u2f_sha256_finish()
{
	if (SHA_FLAGS == 0) SHA_FLAGS = ATECC_SHA_END;
	atecc_send_recv(ATECC_CMD_SHA,
			SHA_FLAGS, shaoffset,shabuf,shaoffset,
			shabuf, sizeof(shabuf), &res_digest);
	SHA_FLAGS = 0;
}

static uint8_t get_signature_length(uint8_t * sig)
{
	return 0x46 + ((sig[32] & 0x80) == 0x80) + ((sig[0] & 0x80) == 0x80);
}

static void dump_signature_der(uint8_t * sig)
{
    uint8_t pad_s = (sig[32] & 0x80) == 0x80;
    uint8_t pad_r = (sig[0] & 0x80) == 0x80;
    uint8_t i[] = {0x30, 0x44};
    uint8_t sigbuf[120];
    i[1] += (pad_s + pad_r);


    // DER encoded signature
    // write der sequence
    // has to be minimum distance and padded with 0x00 if MSB is a 1.
    memmove(sigbuf,i,2);
    i[1] = 0;

    // length of R value plus 0x00 pad if necessary
    memmove(sigbuf+2,"\x02",1);
    i[0] = 0x20 + pad_r;
    memmove(sigbuf+3,i,1 + pad_r);

    // R value
    memmove(sigbuf+3+1+pad_r,sig, 32);

    // length of S value plus 0x00 pad if necessary
    memmove(sigbuf+3+1+pad_r+32,"\x02",1);
    i[0] = 0x20 + pad_s;
    memmove(sigbuf+3+1+pad_r+32+1,i,1 + pad_s);

    // S value
    memmove(sigbuf+3+1+pad_r+32+1+1+pad_s,sig+32, 32);
    u2f_prints("signature-der: "); dump_hex(sigbuf, get_signature_length(sig));
}


static int is_config_locked(uint8_t * buf)
{
	struct atecc_response res;
	atecc_send_recv(ATECC_CMD_READ,
					ATECC_RW_CONFIG,87/4, NULL, 0,
					buf, 36, &res);
	dump_hex(res.buf, res.len);
	if (res.buf[87 % 4] == 0)
		return 1;
	else
		return 0;
}
static int is_data_locked(uint8_t * buf)
{
	struct atecc_response res;
	atecc_send_recv(ATECC_CMD_READ,
					ATECC_RW_CONFIG,86/4, NULL, 0,
					buf, 36, &res);
	dump_hex(res.buf, res.len);
	if (res.buf[86 % 4] == 0)
		return 1;
	else
		return 0;
}

static void dump_config(uint8_t* buf)
{
	uint8_t i,j;
	uint16_t crc = 0;
	struct atecc_response res;
	for (i=0; i < 4; i++)
	{
		if ( atecc_send_recv(ATECC_CMD_READ,
				ATECC_RW_CONFIG | ATECC_RW_EXT, i << 3, NULL, 0,
				buf, 40, &res) != 0)
		{
			u2f_prints("read failed\r\n");
		}
		for(j = 0; j < res.len; j++)
		{
			crc = feed_crc(crc,res.buf[j]);
		}
		dump_hex(res.buf,res.len);
	}

	u2f_printx("config crc:", 1,reverse_bits(crc));
}

static void atecc_setup_config(uint8_t* buf)
{
	uint8_t i;


	uint8_t * slot_configs = "\x83\x71\x81\x01\x83\x71\xC1\x01\x83\x71"
							 "\x83\x71\x83\x71\xC1\x71\x01\x01\x83\x71"
							 "\x83\x71\xC1\x71\x83\x71\x83\x71\x83\x71"
							 "\x83\x71";

	uint8_t * key_configs = "\x13\x00\x3C\x00\x13\x00\x3C\x00\x13\x00"
							"\x3C\x00\x13\x00\x3C\x00\x3C\x00\x3C\x00"
							"\x13\x00\x3C\x00\x13\x00\x3C\x00\x13\x00"
							"\x33\x00";

	// write configuration
	for (i = 0; i < 16; i++)
	{
		if ( atecc_write_eeprom(ATECC_EEPROM_SLOT(i), ATECC_EEPROM_SLOT_OFFSET(i), slot_configs+i*2, ATECC_EEPROM_SLOT_SIZE) != 0)
		{
			u2f_printb("1 atecc_write_eeprom failed ",1, i);
		}

		if ( atecc_write_eeprom(ATECC_EEPROM_KEY(i), ATECC_EEPROM_KEY_OFFSET(i), key_configs+i*2, ATECC_EEPROM_KEY_SIZE) != 0)
		{
			u2f_printb("2 atecc_write_eeprom failed " ,1,i);
		}

	}


	dump_config(buf);
}

static uint8_t trans_key[36];
static uint8_t write_key[36];


void atecc_test_enc_read(uint8_t * buf)
{
	struct atecc_response res;
	memset(trans_key,0x5a,sizeof(trans_key));
	u2f_prints("plaintext: "); dump_hex(trans_key, 32);

	if( atecc_send_recv(ATECC_CMD_WRITE,
			ATECC_RW_DATA|ATECC_RW_EXT, ATECC_EEPROM_DATA_SLOT(3), trans_key, 32,
			buf, 40, &res) != 0)
	{
		u2f_prints("writing test key failed\r\n");
		return;
	}

	atecc_prep_encryption();

	if (atecc_send_recv(ATECC_CMD_READ,
			ATECC_RW_DATA | ATECC_RW_EXT, ATECC_EEPROM_DATA_SLOT(3), NULL, 0,
			buf, 40, &res) != 0)
	{
		u2f_prints("READ slot 3 failed\r\n");
		return;
	}
	else
	{
		u2f_prints("ciphertext: "); dump_hex(res.buf, 32);
	}
}

void atecc_test_signature(int keyslot, uint8_t * buf)
{
	struct atecc_response res;
	if ( atecc_send_recv(ATECC_CMD_GENKEY,
			ATECC_GENKEY_PUBLIC, keyslot, NULL, 0,
			appdata.tmp, 70, &res) != 0)
	{
		u2f_prints("GENKEY public failed\r\n");
		return;
	}

	u2f_prints("pubkey: "); dump_hex(res.buf, 64);

	u2f_prints("signing input: "); dump_hex(res_digest.buf, 32);

	if( atecc_send_recv(ATECC_CMD_NONCE,ATECC_NONCE_TEMP_UPDATE,0,
								res_digest.buf, 32,
								buf, 40, &res) != 0 )
	{
		u2f_prints("signing pass through to tempkey failed\r\n");
		return;
	}

	if( atecc_send_recv(ATECC_CMD_SIGN,
			ATECC_SIGN_EXTERNAL, keyslot, NULL, 0,
			appdata.tmp, 70, &res) != 0)
	{
		u2f_prints("signing failed\r\n");
		return;
	}

	dump_signature_der(res.buf);
}


int atecc_prep_encryption()
{
	struct atecc_response res;
	memset(appdata.tmp,0,32);
	if( atecc_send_recv(ATECC_CMD_NONCE,ATECC_NONCE_TEMP_UPDATE,0,
								appdata.tmp, 32,
								appdata.tmp, 40, &res) != 0 )
	{
		u2f_prints("pass through to tempkey failed\r\n");
		return -1;
	}
	if( atecc_send_recv(ATECC_CMD_GENDIG,
			ATECC_RW_DATA, U2F_MASTER_KEY_SLOT, NULL, 0,
			appdata.tmp, 40, &res) != 0)
	{
		u2f_prints("GENDIG failed\r\n");
		return -1;
	}

	return 0;
}

int atecc_privwrite(uint16_t keyslot, uint8_t * key, uint8_t * mask, uint8_t * digest)
{
	struct atecc_response res;
	uint8_t i;



	atecc_prep_encryption();

	for (i=0; i<36; i++)
	{
		appdata.tmp[i] = key[i] ^ mask[i];
	}
	memmove(appdata.tmp+36, digest, 32);

	if( atecc_send_recv(ATECC_CMD_PRIVWRITE,
			ATECC_PRIVWRITE_ENC, keyslot, appdata.tmp, 68,
			appdata.tmp, 40, &res) != 0)
	{
		u2f_prints("PRIVWRITE failed\r\n");
		return -1;
	}
	return 0;
}

static void compute_key_hash(uint8_t * key, uint8_t * mask, int slot)
{
	// key must start with 4 zeros
	memset(appdata.tmp,0,28);
	memmove(appdata.tmp + 28, key, 36);

	u2f_sha256_start();

	u2f_sha256_update(mask,32);


	appdata.tmp[0] = ATECC_CMD_PRIVWRITE;
	appdata.tmp[1] = ATECC_PRIVWRITE_ENC;
	appdata.tmp[2] = slot;
	appdata.tmp[3] = 0;
	appdata.tmp[4] = 0xee;
	appdata.tmp[5] = 0x01;
	appdata.tmp[6] = 0x23;

	u2f_sha256_update(appdata.tmp,28 + 36);
	u2f_sha256_finish();
}

void atecc_setup_init(uint8_t * buf)
{
	// 13s watchdog
	WDTCN = 7;
	dump_config(buf);
	if (!is_config_locked(buf))
	{
		u2f_prints("setting up config...\r\n");
		atecc_setup_config(buf);
	}
	else
	{
		u2f_prints("already locked\r\n");
	}

}

// buf should be at least 40 bytes
void atecc_setup_device(struct config_msg * msg)
{
	struct atecc_response res;
	struct config_msg usbres;

	static uint16_t crc = 0;
	int i;
	uint8_t buf[40];

	memset(&usbres, 0, sizeof(struct config_msg));
	usbres.cmd = msg->cmd;
	dump_hex(msg,64);

	switch(msg->cmd)
	{
		case U2F_CONFIG_GET_SERIAL_NUM:

			u2f_prints("U2F_CONFIG_GET_SERIAL_NUM\r\n");
			atecc_send_recv(ATECC_CMD_READ,
					ATECC_RW_CONFIG | ATECC_RW_EXT, 0, NULL, 0,
					buf, 40, &res);
			memmove(usbres.buf+1, res.buf, 15);
			usbres.buf[0] = 15;
			break;

		case U2F_CONFIG_LOAD_TRANS_KEY:
			u2f_prints("U2F_CONFIG_LOAD_TRANS_KEY\r\n");



			memmove(trans_key,msg->buf,32);
			u2f_prints("master key: "); dump_hex(trans_key,32);

			usbres.buf[0] = 1;

			if( atecc_send_recv(ATECC_CMD_WRITE,
					ATECC_RW_DATA|ATECC_RW_EXT, ATECC_EEPROM_DATA_SLOT(1), trans_key, 32,
					buf, sizeof(buf), &res) != 0)
			{
				usbres.buf[0] = 0;
				u2f_prints("writing master key failed\r\n");
			}

			memset(appdata.tmp,0,32);
			memset(trans_key,0,32);

			break;

		case U2F_CONFIG_IS_BUILD:
			u2f_prints("U2F_CONFIG_IS_BUILD\r\n");
			usbres.buf[0] = 1;
			break;
		case U2F_CONFIG_IS_CONFIGURED:
			u2f_prints("U2F_CONFIG_IS_CONFIGURED\r\n");
			usbres.buf[0] = 1;
			break;
		case U2F_CONFIG_LOCK:
			crc = *(uint16_t*)msg->buf;
			usbres.buf[0] = 1;
			u2f_printx("got crc: ",1,crc);

			if (!is_config_locked(buf))
			{
				if (atecc_send_recv(ATECC_CMD_LOCK,
						ATECC_LOCK_CONFIG, crc, NULL, 0,
						buf, sizeof(buf), NULL))
				{
					u2f_prints("ATECC_CMD_LOCK config failed\r\n");
					return;
				}
			}
			else
			{
				u2f_prints("config already locked\r\n");
			}

			if (!is_data_locked(buf))
			{
				if (atecc_send_recv(ATECC_CMD_LOCK,
						ATECC_LOCK_DATA_OTP | 0x80, crc, NULL, 0,
						buf, sizeof(buf), NULL))
				{
					u2f_prints("ATECC_CMD_LOCK data failed\r\n");
					return;
				}
			}
			else
			{
				u2f_prints("data already locked\r\n");
			}
			break;
		case U2F_CONFIG_GENKEY:
			u2f_prints("U2F_CONFIG_GENKEY\r\n");

			atecc_send_recv(ATECC_CMD_GENKEY,
					ATECC_GENKEY_PRIVATE, U2F_ATTESTATION_KEY_SLOT, NULL, 0,
					appdata.tmp, sizeof(appdata.tmp), &res);

			u2f_printb("key is bytes ",1,res.len);

			memmove((uint8_t*)&usbres, res.buf, 64);

			break;
		case U2F_CONFIG_LOAD_WRITE_KEY:
			u2f_prints("U2F_CONFIG_LOAD_WRITE_KEY\r\n");
			memmove(write_key,msg->buf,36);
			usbres.buf[0] = 1;

			break;
		case U2F_CONFIG_LOAD_ATTEST_KEY:
			u2f_prints("U2F_CONFIG_LOAD_ATTEST_KEY\r\n");

			memset(trans_key,0,36);
			memmove(trans_key+4,msg->buf,32);
			usbres.buf[0] = 1;
			compute_key_hash(trans_key,  write_key, U2F_ATTESTATION_KEY_SLOT);

			dump_hex(write_key,36);

			if (atecc_privwrite(U2F_ATTESTATION_KEY_SLOT, trans_key, write_key, res_digest.buf) != 0)
			{
				u2f_prints("load attest key failed\r\n");
				usbres.buf[0] = 0;
			}

			break;
		case U2F_CONFIG_BOOTLOADER:
			u2f_prints("U2F_CONFIG_BOOTLOADER\r\n");

			memset(trans_key,0xff,4);
			if( atecc_send_recv(ATECC_CMD_WRITE,
					ATECC_RW_DATA, ATECC_EEPROM_DATA_SLOT(8), trans_key, 4,
					buf, sizeof(buf), &res) != 0)
			{
				usbres.buf[0] = 0;
				u2f_prints("writing unlocked bootloader failed\r\n");
			}
			else
			{
				usbres.buf[0] = 1;
				usb_write((uint8_t*)&usbres, HID_PACKET_SIZE);
				u2f_delay(20);
				 // Write R0 and issue a software reset
				 *((uint8_t SI_SEG_DATA *)0x00) = 0xA5;
				 RSTSRC = RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET;
			}
			break;
		default:
			u2f_printb("invalid command: ",1,msg->cmd);
			usbres.buf[0] = 0;
	}

	usb_write((uint8_t*)&usbres, HID_PACKET_SIZE);

}
#endif
