/*
 * eeprom.c
 *
 *  Created on: Mar 4, 2016
 *      Author: pp
 */
#include <SI_EFM8UB1_Register_Enums.h>
#include <stdint.h>

#include "eeprom.h"



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
	bit old_int;// IE_EA;

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
