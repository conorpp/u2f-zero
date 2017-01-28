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
#include <stdint.h>

#include "eeprom.h"

void eeprom_init()
{
	uint8_t secbyte;
	eeprom_read(0xFBFF,&secbyte,1);
	if (secbyte == 0xff)
	{
		eeprom_erase(0xFBC0);
		secbyte = -32;
		eeprom_write(0xFBFF, &secbyte, 1);
	}
}

void eeprom_read(uint16_t addr, uint8_t * buf, uint8_t len)
{
	uint8_t code * eepaddr =  (uint8_t code *) addr;
	bit old_int;

	while(len--)
	{
		old_int = IE_EA;
		IE_EA = 0;
		*buf++ = *eepaddr++;
		IE_EA = old_int;
	}
}

void _eeprom_write(uint16_t addr, uint8_t * buf, uint8_t len, uint8_t flags)
{
	uint8_t xdata * data eepaddr = (uint8_t xdata *) addr;
	bit old_int;

	while(len--)
	{
		old_int = IE_EA;
		IE_EA = 0;
		// Enable VDD monitor
		VDM0CN = 0x80;
		RSTSRC = 0x02;

		// unlock key
		FLKEY  = 0xA5;
		FLKEY  = 0xF1;
		PSCTL |= flags;

		*eepaddr = *buf;
		PSCTL &= ~flags;
		IE_EA = old_int;

		eepaddr++;
		buf++;
	}
}
