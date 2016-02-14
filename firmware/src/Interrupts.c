// USER INCLUDES
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



// Status vector - top 4 bits only
#define  SMB_MTSTA                0xE0 // (MT) start transmitted
#define  SMB_MTDB                 0xC0 // (MT) data byte transmitted
#define  SMB_MRDB                 0x80 // (MR) data byte received


struct smb_interrupt_interface SMB;


SI_INTERRUPT (SMBUS0_ISR, SMBUS0_IRQn)
{
	uint8_t stat = SMB0CN0 & 0xF0;
	bit FAIL = 0;
	static bit ADDR_SEND = 0;

	if (SMB0CN0_ARBLOST == 0)
	{
	switch (stat)
	{
	 case SMB_MTSTA:
		SMB0DAT = SMB.addr;
		SMB0DAT &= 0xFE;

		SMB0DAT |= SMB_FLAGS & SMB_READ;
		SMB0CN0_STA = 0;
		ADDR_SEND = 1;
		break;

	 case SMB_MTDB:
		if (SMB0CN0_ACK)
		{
			if (SMB_RW == 0)
			{
				if (SMB.SMB_WRITE_BUF_OFFSET < SMB.write_buf_len)
				{
					SMB0DAT = SMB.write_buf[SMB.SMB_WRITE_BUF_OFFSET++];

				}
				else
				{
					SMB0CN0_STO = 1;
					SMB_FLAGS &= 0xff ^ SMB_BSSY;
				}
			}
			else
			{
				// do nothing and switch to receive mode
			}
		}
		else
		{
			// NACK
			// turn the bus around
			SMB0CN0_STO = 1;
			SMB0CN0_STA = 1;
		}
		break;

	 case SMB_MRDB:
		 if (SMB.SMB_READ_OFFSET < SMB.SMB_READ_LEN)
		 {
			 SMB.SMB_READ_BUF[SMB.SMB_READ_OFFSET++] = SMB0DAT;
			 SMB0CN0_ACK = 1;
		 }
		 else
		 {
			 SMB_FLAGS &= 0xff ^ SMB_BSSY;
			 SMB0CN0_ACK = 0;
			 SMB0CN0_STO = 1;
		 }

		break;

	 default:
		 u2f_print("i2c fail\r\n");
		FAIL = 1;                  // Indicate failed transfer
								   // and handle at end of ISR
		break;

	} // end switch
	}
	else
	{
		FAIL = 1;
	}

	if (FAIL)
	{
	   u2f_print("i2c fail\r\n");
	  SMB0CF &= ~0x80;
	  SMB0CF |= 0x80;
	  SMB0CN0_STA = 0;
	  SMB0CN0_STO = 0;
	  SMB0CN0_ACK = 0;
	  SMB_FLAGS &= 0xff ^ SMB_BSSY;

	  FAIL = 0;

	}

	// interrupt flag
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

	SMB_FLAGS &= 0xff ^ SMB_BSSY;
}




