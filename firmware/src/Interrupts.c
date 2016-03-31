/*
 * i2c.c
 *
 *      Author: Conor
 */
#include <SI_EFM8UB1_Register_Enums.h>
#include <stdint.h>
#include "app.h"
#include "i2c.h"

#include "bsp.h"

uint32_t data _MS_ = 0;

SI_INTERRUPT (TIMER2_ISR, TIMER2_IRQn)
{
	TMR2CN0_TF2H = 0;
	++_MS_;
}

#define SMB_STATUS_START			0xE0
#define SMB_STATUS_MTX				0xC0
#define SMB_STATUS_MRX				0x80
#define SMB_STATE_MASK				0xF0


#define SMB_TX_STATE_MASK (SMB_WRITE_EXT|SMB_WRITE)
#define SMB_TX_EXT (SMB_WRITE|SMB_WRITE_EXT)
#define SMB_TX (SMB_WRITE)

data uint8_t SMB_addr = 0;
uint8_t * SMB_write_buf = NULL;
data uint8_t SMB_write_len = 0;
data uint8_t SMB_write_offset = 0;
data uint8_t SMB_read_len = 0;
data uint8_t SMB_read_offset = 0;
uint8_t * SMB_read_buf = NULL;
uint8_t * SMB_write_ext_buf = NULL;
data uint8_t  SMB_write_ext_len = 0;
data uint8_t  SMB_write_ext_offset = 0;
uint8_t SMB_preflags = 0;
uint16_t  SMB_crc = 0;
data uint8_t  SMB_crc_offset = 0;
data volatile uint8_t SMB_FLAGS = 0;

static void update_from_packet_length()
{
	if (SMB_read_buf[0] <= SMB_read_len)
	{
		SMB_read_len = SMB_read_buf[0];
	}
	else
	{
		// truncated read
		SMB_FLAGS |= SMB_READ_TRUNC;
	}
}

static void _feed_crc(uint8_t b)
{
	SMB_crc = feed_crc(SMB_crc,b);
}

static void restart_bus()
{
	SMB0CF &= ~0x80;
	SMB0CF |= 0x80;
	SMB0CN0_STA = 0;
	SMB0CN0_STO = 0;
	SMB0CN0_ACK = 0;
	SMB_BUSY_CLEAR();
}

SI_INTERRUPT (SMBUS0_ISR, SMBUS0_IRQn)
{
	data uint8_t bus = SMB0CN0 & SMB_STATE_MASK;
	data uint8_t c;
	if (SMB0CN0_ARBLOST != 0)
	{
		goto fail;
	}

	switch (bus)
	{
		case SMB_STATUS_START:
			SMB0DAT = SMB_addr | (SMB_FLAGS & SMB_READ);
			SMB0CN0_STA = 0;
			break;

		case SMB_STATUS_MTX:
			if (!SMB0CN0_ACK)
			{
				// NACK
				// end transaction
				SMB0CN0_STO = 1;
				SMB_FLAGS |= SMB_RECV_NACK;
				SMB_BUSY_CLEAR();
			}
			else if (!SMB_WRITING())
			{
				// do nothing and switch to receive mode
			}
			else if (SMB_write_offset < SMB_write_len)
			{
				// start writing first buffer
				// dont crc first byte for atecc508a
				c = SMB_write_buf[SMB_write_offset++];
				if (SMB_write_offset > 1) _feed_crc(c);
				SMB0DAT = c;

			}
			else if(SMB_WRITING_EXT() && SMB_write_ext_offset < SMB_write_ext_len)
			{
				// start writing second optional buffer
				c = SMB_write_ext_buf[SMB_write_ext_offset++];
				_feed_crc(c);
				SMB0DAT = c;
			}
			else
			{
				// write optional CRC
				switch(SMB_crc_offset++)
				{
					case 0:
						SMB_crc = reverse_bits(SMB_crc);
						SMB0DAT = (uint8_t)SMB_crc;
						break;
					case 1:
						SMB0DAT = (uint8_t)(SMB_crc>>8);
						break;
					case 2:
						SMB_CRC_CLEAR();
						SMB0CN0_STO = 1;
						SMB_BUSY_CLEAR();
				}
			}

			break;

		case SMB_STATUS_MRX:
			// read in buffer

			if (SMB_read_offset < SMB_read_len)
			{
				c = SMB0DAT;
				SMB_read_buf[SMB_read_offset] = c;

				// update with length from packet
				// warning this is device specific to atecc508a
				if (SMB_read_offset == 0)
				{
					update_from_packet_length();
				}

				if ((SMB_read_offset < (SMB_read_len - 2)))
				{
					SMB_crc = feed_crc(SMB_crc, c);
				}

				SMB_read_offset++;
				SMB0CN0_ACK = 1;
			}
			else
			{
				// end transaction
				if (SMB_HAS_CRC())
				{
					SMB_crc = reverse_bits(SMB_crc);
				}
				SMB_BUSY_CLEAR();
				SMB0CN0_ACK = 0;
				SMB0CN0_STO = 1;
			}

			break;

		default:
			goto fail;
			break;

	}


	// interrupt flag
	SMB0CN0_SI = 0;
	return;

	fail:
		u2f_printb("smbus fail ",1,bus);
		restart_bus();
		SMB0CN0_SI = 0;
}

//-----------------------------------------------------------------------------
// TIMER3_ISR
//-----------------------------------------------------------------------------
//
// TIMER3 ISR Content goes here. Remember to clear flag bits:
// TMR3CN::TF3H (Timer # High Byte Overflow Flag)
// TMR3CN::TF3L (Timer # Low Byte Overflow Flag)
//
//
// A Timer3 interrupt indicates an SMBus SCL low timeout.
// The SMBus is disabled and re-enabled here
//
//-----------------------------------------------------------------------------
SI_INTERRUPT (TIMER3_ISR, TIMER3_IRQn)
{
	restart_bus();
}




