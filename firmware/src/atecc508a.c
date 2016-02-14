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

uint16_t atecc_crc16(uint8_t* buf, uint16_t len)
{
	uint16_t crc = 0;
	while (len--) {
		crc ^= *buf++;
		crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	}

	// efficient bit reversal
	crc = (((crc & 0xaaaa) >> 1) | ((crc & 0x5555) << 1));
	crc = (((crc & 0xcccc) >> 2) | ((crc & 0x3333) << 2));
	crc = (((crc & 0xf0f0) >> 4) | ((crc & 0x0f0f) << 4));
	crc = (((crc & 0xff00) >> 8) | ((crc & 0x00ff) << 8));

	return crc;
}

void atecc_send(uint8_t cmd, uint8_t p1, uint16_t p2,
					uint8_t * buf, uint8_t len)
{
	static uint8_t params[8];
	uint16_t crc;
	params[0] = 0x3;
	params[1] = 7;
	params[2] = cmd;
	params[3] = p1;
	params[4] = ((uint8_t*)&p2)[1];
	params[5] = ((uint8_t*)&p2)[0];
	crc = atecc_crc16(params+1,5);
	params[6] = ((uint8_t*)&crc)[1];
	params[7] = ((uint8_t*)&crc)[0];

	smb_write( ATECC508A_ADDR, params, sizeof(params) );
}

uint8_t atecc_recv(uint8_t * buf, uint8_t buflen)
{
	uint8_t pkt_len;
	uint16_t crc_recv, crc_compute;
	smb_read( 0xc0,buf,10);
	pkt_len = buf[0];
	if (pkt_len <= buflen && pkt_len >= 4)
	{
		crc_compute = atecc_crc16(buf,pkt_len-2);
		crc_recv = htole16(*((uint16_t*)(buf+pkt_len-2)));
		if (crc_recv != crc_compute)
			goto fail;
	}
	else
	{
		goto fail;
	}
	return 0;
	fail:
		u2f_print("crc failed %x != %x\r\n",crc_compute,crc_recv );
	return 1;
}
