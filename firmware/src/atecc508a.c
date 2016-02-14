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



void atecc_send(uint8_t cmd, uint8_t p1, uint16_t p2,
					uint8_t * buf, uint8_t len)
{
	static uint8_t params[6];
	params[0] = 0x3;
	params[1] = 7;
	params[2] = cmd;
	params[3] = p1;
	params[4] = ((uint8_t*)&p2)[1];
	params[5] = ((uint8_t* )&p2)[0];

	smb_init_crc();
	smb_set_ext_write(buf, len);
	smb_write( ATECC508A_ADDR, params, sizeof(params));
}

#define PKT_CRC(buf, pkt_len) (htole16(*((uint16_t*)(buf+pkt_len-2))))

uint8_t atecc_recv(uint8_t * buf, uint8_t buflen)
{
	uint8_t pkt_len;
	smb_init_crc();
	pkt_len = smb_read( 0xc0,buf,buflen);

	if (SMB_FLAGS & SMB_READ_TRUNC)
	{
		u2f_print("error read truncated\r\n");
	}

	if (pkt_len <= buflen && pkt_len >= 4)
	{
		if (PKT_CRC(buf,pkt_len) != SMB.crc)
			goto fail;
	}
	else
	{
		goto fail;
	}

	return pkt_len;

	fail:
	u2f_print("crc failed %x\r\n",SMB.crc );
	return -1;
}
