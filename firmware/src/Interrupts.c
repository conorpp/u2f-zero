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

struct smb_interrupt_interface SMB;
data volatile uint8_t SMB_FLAGS;

static void update_from_packet_length()
{
	if (SMB.read_buf[0] <= SMB.read_len)
	{
		SMB.read_len = SMB.read_buf[0];
	}
	else
	{
		// truncated read
		SMB_FLAGS |= SMB_READ_TRUNC;
	}
}

static void _feed_crc(uint8_t b)
{
	if (SMB_HAS_CRC())
	{
		SMB.crc = feed_crc(SMB.crc,b);
	}
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
	if (SMB0CN0_ARBLOST != 0)
	{
		goto fail;
	}

	switch (bus)
	{
		case SMB_STATUS_START:
			SMB0DAT = SMB.addr | (SMB_FLAGS & SMB_READ);
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
			else if (SMB.write_offset < SMB.write_len)
			{
				// start writing first buffer
				// dont crc first byte for atecc508a
				if (SMB.write_offset) _feed_crc(SMB.write_buf[SMB.write_offset]);
				SMB0DAT = SMB.write_buf[SMB.write_offset++];

			}
			else if(SMB_WRITING_EXT() && SMB.write_ext_offset < SMB.write_ext_len)
			{
				// start writing second optional buffer
				_feed_crc(SMB.write_ext_buf[SMB.write_ext_offset]);
				SMB0DAT = SMB.write_ext_buf[SMB.write_ext_offset++];
			}
			else if (SMB_HAS_CRC())
			{
				// write optional CRC
				switch(SMB.crc_offset++)
				{
					case 0:
						SMB.crc = reverse_bits(SMB.crc);
						SMB0DAT = (uint8_t)SMB.crc;
						break;
					case 1:
						SMB0DAT = (uint8_t)(SMB.crc>>8);
						SMB_CRC_CLEAR();
						break;
				}
			}
			else
			{
				// end transaction
				SMB0CN0_STO = 1;
				SMB_BUSY_CLEAR();
			}


			break;

		case SMB_STATUS_MRX:
			// read in buffer

			if (SMB.read_offset < SMB.read_len)
			{
				SMB.read_buf[SMB.read_offset] = SMB0DAT;

				// update with length from packet
				// warning this is device specific to atecc508a
				if (SMB.read_offset == 0)
				{
					update_from_packet_length();
				}

				if ((SMB.read_offset < (SMB.read_len - 2)) && SMB_HAS_CRC())
				{
					SMB.crc = feed_crc(SMB.crc, SMB.read_buf[SMB.read_offset]);
				}

				SMB.read_offset++;
				SMB0CN0_ACK = 1;
			}
			else
			{
				// end transaction
				if (SMB_HAS_CRC())
				{
					SMB.crc = reverse_bits(SMB.crc);
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




