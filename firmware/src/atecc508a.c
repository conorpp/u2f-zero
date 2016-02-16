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
}

#define PKT_CRC(buf, pkt_len) (htole16(*((uint16_t*)(buf+pkt_len-2))))

uint8_t atecc_recv(uint8_t * buf, uint8_t buflen, struct atecc_response* res)
{
	data uint8_t pkt_len;
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

	if (res != NULL)
	{
		res->len = pkt_len - 3;
		res->buf = buf+1;
	}
	return pkt_len;

	fail:
	u2f_print("crc failed %x\r\n",SMB.crc );
	return -1;
}

uint8_t atecc_send_recv(uint8_t cmd, uint8_t p1, uint16_t p2,
							uint8_t* tx, uint8_t txlen, uint8_t * rx,
							uint8_t rxlen, struct atecc_response* res)
{
	do{
		atecc_send(cmd, p1, p2, tx, txlen);
	}while(atecc_recv(rx,rxlen, res) < 0);
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

	return res.buf[0] == 0 ? 0 : -1;
}
