/*
 * i2c.c
 *
 *  Created on: Feb 14, 2016
 *      Author: Conor
 */
#include <SI_EFM8UB1_Register_Enums.h>
#include <string.h>
#include <stdint.h>

#include "i2c.h"

#include "bsp.h"
#include "app.h"



uint8_t smb_read (uint8_t addr, uint8_t* dest, uint8_t count)
{
	while(SMB_IS_BUSY())
	{
		if (SMB_ERRORS_EXCEEDED(&SMB))
		{
			set_app_error(ERROR_I2C_ERRORS_EXCEEDED);
			return -1;
		}
	}
	SMB_FLAGS = SMB_READ | SMB_BUSY| SMB.preflags;
	SMB.preflags = 0;

	SMB.read_offset = 0;
	SMB.addr = addr;
	SMB.read_len = count;
	SMB.read_buf = dest;
	SMB0CN0_STA = 1;

	while(SMB_IS_BUSY())
	{
		if (SMB_ERRORS_EXCEEDED(&SMB))
		{
			set_app_error(ERROR_I2C_ERRORS_EXCEEDED);
			return -1;
		}
	}
	return SMB.read_len;
}


void smb_write (uint8_t addr, uint8_t* buf, uint8_t len)
{
   while(SMB_IS_BUSY())
   {
	   if (SMB_ERRORS_EXCEEDED(&SMB))
	   {
		   set_app_error(ERROR_I2C_ERRORS_EXCEEDED);
		   return;
	   }
   }

   SMB_FLAGS = SMB_WRITE | SMB_BUSY | SMB.preflags;
   SMB.preflags = 0;

   SMB.write_len = len;
   SMB.write_buf = buf;
   SMB.write_offset = 0;
   SMB.addr = addr;

   SMB0CN0_STA = 1;
}

void smb_set_ext_write( uint8_t* extbuf, uint8_t extlen)
{
	while(SMB_IS_BUSY())
	{
		if (SMB_ERRORS_EXCEEDED(&SMB))
		{
			set_app_error(ERROR_I2C_ERRORS_EXCEEDED);
			return;
		}
	}
	SMB.write_ext_len = extlen;
	SMB.write_ext_buf = extbuf;
	SMB.write_ext_offset = 0;
	SMB.preflags |= SMB_WRITE_EXT;
}

void smb_init_crc()
{
	while(SMB_IS_BUSY())
	{
		if (SMB_ERRORS_EXCEEDED(&SMB))
		{
			set_app_error(ERROR_I2C_ERRORS_EXCEEDED);
			return;
		}
	}
	SMB.crc = 0;
	SMB.crc_offset = 0;
	SMB.preflags |= SMB_COMPUTE_CRC;
}

uint16_t feed_crc(uint16_t crc, uint8_t b)
{
	crc ^= b;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	return crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
}

uint16_t reverse_bits(uint16_t crc)
{
	// efficient bit reversal for 16 bit int
	crc = (((crc & 0xaaaa) >> 1) | ((crc & 0x5555) << 1));
	crc = (((crc & 0xcccc) >> 2) | ((crc & 0x3333) << 2));
	crc = (((crc & 0xf0f0) >> 4) | ((crc & 0x0f0f) << 4));
	return (((crc & 0xff00) >> 8) | ((crc & 0x00ff) << 8));
}

void smb_init()
{
	memset(&SMB,0,sizeof(SMB));
	SMB_FLAGS = 0;
}
