/*
 * atecc508a.c
 *
 *  Created on: Feb 14, 2016
 *      Author: Conor
 */
#include <endian.h>
#include <stdint.h>
#include "atecc508a.h"
#include "i2c.h"

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

	smb_init_crc();
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
	static data uint8_t params = 0x2;
	smb_write( ATECC508A_ADDR, &params, sizeof(params));
}

void atecc_sleep()
{
	static data uint8_t params = 0x1;
	smb_write( ATECC508A_ADDR, &params, sizeof(params));
}

void atecc_wake()
{
	static uint8_t params[] = {0,0};
	smb_write( ATECC508A_ADDR, params, sizeof(params));
}

#define PKT_CRC(buf, pkt_len) (htole16(*((uint16_t*)(buf+pkt_len-2))))

int8_t atecc_recv(uint8_t * buf, uint8_t buflen, struct atecc_response* res)
{
	data uint8_t pkt_len;
	smb_init_crc();
	pkt_len = smb_read( ATECC508A_ADDR,buf,buflen);
	if (SMB_WAS_NACKED())
	{
		return -1;
	}

	if (SMB_FLAGS & SMB_READ_TRUNC)
	{
		set_app_error(ERROR_READ_TRUNCATED);
	}

	if (pkt_len <= buflen && pkt_len >= 4)
	{
		if (PKT_CRC(buf,pkt_len) != SMB.crc)
		{
			goto fail;
		}
	}
	else
	{
		goto fail;
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

	fail:
	return -1;
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
		case ATECC_CMD_LOCK:
			d = 32;
			break;
		default:
			d = 26;
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
	}

	while(atecc_recv(rx,rxlen, res) == -1)
	{
		errors++;
		if (errors > 5)
		{
			u2f_print("fail recv %bx\r\n", appdata.error);
			flush_messages();
		}
		switch(appdata.error)
		{
			case ERROR_NOTHING:
				delay_cmd(cmd);
				break;
			case ERROR_ATECC_EXECUTION:
				delay_cmd(cmd);
			case ERROR_ATECC_PARSE:
			case ERROR_ATECC_FAULT:
			case ERROR_ATECC_WAKE:
			case ERROR_ATECC_WATCHDOG:
				goto resend;
				break;
			default:
				u2f_delay(10);
				break;
		}

	}

	atecc_idle();
	return 0;
}

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

#ifdef SETUP_DEVICE

static void dump_config(uint8_t* buf)
{
	uint8_t i,j;
	uint16_t crc = 0;
	struct atecc_response res;
	for (i=0; i < 4; i++)
	{
		atecc_send_recv(ATECC_CMD_READ,
				ATECC_RW_CONFIG | ATECC_RW_EXT, i << 3, NULL, 0,
				buf, 36, &res);
		for(j = 0; j < res.len; j++)
		{
			crc = feed_crc(crc,res.buf[j]);
		}
		dump_hex(res.buf,res.len);
	}

	u2f_print("config crc: %x\r\n", reverse_bits(crc));
}

static void atecc_setup_config(uint8_t* buf)
{
	struct atecc_response res;
	int i;

	struct atecc_slot_config sc;
	struct atecc_key_config kc;
	memset(&sc, 0, sizeof(struct atecc_slot_config));
	memset(&kc, 0, sizeof(struct atecc_key_config));
	sc.readkey = 3;
	sc.secret = 1;
	sc.writeconfig = 0xa;

	// set up read/write permissions for keys
	for (i = 0; i < 15; i++)
	{
		if ( atecc_write_eeprom(ATECC_EEPROM_SLOT(i), ATECC_EEPROM_SLOT_OFFSET(i), &sc, ATECC_EEPROM_SLOT_SIZE) != 0)
		{
			u2f_print("1 atecc_write_eeprom failed %bd\r\n",i);
		}

	}

	memset(&sc, 0, sizeof(struct atecc_slot_config));
	sc.writeconfig = 0x2;
	if ( atecc_write_eeprom(ATECC_EEPROM_SLOT(15), ATECC_EEPROM_SLOT_OFFSET(15), &sc, ATECC_EEPROM_SLOT_SIZE) != 0)
	{
		u2f_print("2 atecc_write_eeprom failed %bd\r\n",i);
	}

	kc.private = 1;
	kc.pubinfo = 1;
	kc.keytype = 0x4;

	// set up config for keys
	for (i = 0; i < 16; i++)
	{
		if (i > 7)
		{
			kc.lockable = 1;
		}
		if (i==15)
		{
			kc.private = 0;
			kc.pubinfo = 0;
		}
		if ( atecc_write_eeprom(ATECC_EEPROM_KEY(i), ATECC_EEPROM_KEY_OFFSET(i), &kc, ATECC_EEPROM_KEY_SIZE) != 0)
		{
			u2f_print("3 atecc_write_eeprom failed %bd\r\n" ,i);
		}

	}

	// set otp to be read only
	buf[0] = 0xAA;
	if ( atecc_write_eeprom(ATECC_EEPROM_B2A(18), ATECC_EEPROM_B2O(18), buf, 1) != 0)
	{
		u2f_print("otp write failed\r\n");
	}

	dump_config(buf);
}

// write a message to the otp memory before locking
static void atecc_write_otp()
{
	char msg[] = "conorpp's u2f token.\r\n\0\0\0\0";
	char buf[7];
	int i;
	for (i=0; i<sizeof(msg); i+=4)
	{
		atecc_send_recv(ATECC_CMD_WRITE,
				ATECC_RW_OTP, ATECC_EEPROM_B2A(i), msg+i, 4,
				buf, sizeof(buf), NULL);
	}
}

void atecc_setup_device(uint8_t * buf)
{
	setup_config(buf);

	// lock config
	if (atecc_send_recv(ATECC_CMD_LOCK,
			ATECC_LOCK_CONFIG, 0xf2cd, NULL, 0,
			buf, sizeof(buf), NULL))
	{
		u2f_print("ATECC_CMD_LOCK failed\r\n");
		return;
	}

	write_otp();
}
#endif
