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

 */
#include <SI_EFM8UB1_Register_Enums.h>
#include <string.h>
#include <stdint.h>

#include "i2c.h"

#include "bsp.h"
#include "app.h"

uint8_t smb_read (uint8_t addr, uint8_t* dest, uint8_t count)
{
	while(SMB_IS_BUSY()){}

	SMB_crc = 0;
	SMB_crc_offset = 0;
	SMB_FLAGS = SMB_READ | SMB_BUSY | SMB_preflags;
	SMB_preflags = 0;

	SMB_read_offset = 0;
	SMB_addr = addr;
	SMB_read_len = count;
	SMB_read_buf = dest;
	SMB0CN0_STA = 1;

	while(SMB_IS_BUSY()){}
	return SMB_read_len;
}


void smb_write (uint8_t addr, uint8_t* buf, uint8_t len)
{
	while(SMB_IS_BUSY()){}

	SMB_crc = 0;
	SMB_crc_offset = 0;
	SMB_FLAGS = SMB_WRITE | SMB_BUSY | SMB_preflags;
	SMB_preflags = 0;

	SMB_write_len = len;
	SMB_write_buf = buf;
	SMB_write_offset = 0;
	SMB_addr = addr;

	SMB0CN0_STA = 1;
	while(SMB_IS_BUSY()){}
}

void smb_set_ext_write( uint8_t* extbuf, uint8_t extlen)
{
	while(SMB_IS_BUSY()){}
	SMB_write_ext_len = extlen;
	SMB_write_ext_buf = extbuf;
	SMB_write_ext_offset = 0;
	SMB_preflags |= SMB_WRITE_EXT;
}

// CRC-16 appropriate for a byte model interrupt routine.
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

// Resulting CRC should be reversed to be correct CRC-16
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
	SMB_FLAGS = 0;
}
