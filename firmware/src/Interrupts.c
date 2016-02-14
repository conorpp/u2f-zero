// USER INCLUDES
#include <SI_EFM8UB1_Register_Enums.h>
#include <stdint.h>
#include "app.h"
#include "bsp.h"

uint32_t data _MS_ = 0;

SI_INTERRUPT (TIMER2_ISR, TIMER2_IRQn)
{
	TMR2CN0_TF2H = 0;
	++_MS_;
}

#define MASTER_TX_START 0xE0
#define MASTER_TX_STOP 0xD0
#define MASTER_TX_DATA 0xC0
#define MASTER_RX_READ 0x80

#define I2C_READ 1
#define I2C_WRITE 0

#define ECC508_ADDR 0xc0





#define LED_ON   0
#define LED_OFF  1

SI_SBIT (DISP_EN, SFR_P2, 3);          // Display Enable
#define DISP_BC_DRIVEN   0             // 0 = Board Controller drives display
#define DISP_EFM8_DRIVEN 1             // 1 = EFM8 drives display

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define  SYSCLK               24500000 // System clock frequency in Hz

#define  SMB_FREQUENCY           10000 // Target SCL clock rate
                                       // This example supports between 10kHz
                                       // and 100kHz

#define  WRITE                    0x00 // SMBus WRITE command
#define  READ                     0x01 // SMBus READ command

// Device addresses (7 bits, LSB is a don't care)
#define  SLAVE_ADDR               0xF0 // Device address for slave target

// Status vector - top 4 bits only
#define  SMB_MTSTA                0xE0 // (MT) start transmitted
#define  SMB_MTDB                 0xC0 // (MT) data byte transmitted
#define  SMB_MRDB                 0x80 // (MR) data byte received
// End status vector definition

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
extern uint8_t SMB_DATA_IN;            // Global holder for SMBus data
                                       // All receive data is written here

extern uint8_t SMB_DATA_OUT;           // Global holder for SMBus data.
                                       // All transmit data is read from here
int inter = 0;
extern uint8_t TARGET;                 // Target SMBus slave address

extern volatile bit SMB_BUSY;          // Software flag to indicate when the
                                       // SMB_Read() or SMB_Write() functions
                                       // have claimed the SMBus

extern volatile bit SMB_RW;            // Software flag to indicate the
                                       // direction of the current transfer

extern uint16_t NUM_ERRORS;            // Counter for the number of errors.

uint8_t* SMB_WRITE_BUF;
uint8_t SMB_WRITE_BUF_LEN;
uint8_t SMB_WRITE_BUF_OFFSET;
uint8_t SMB_READ_LEN;
uint8_t SMB_READ_OFFSET;
uint8_t* SMB_READ_BUF;



SI_INTERRUPT (SMBUS0_ISR, SMBUS0_IRQn)
{
	uint8_t stat = SMB0CN0 & 0xF0;
   bit FAIL = 0;                       // Used by the ISR to flag failed
                                       // transfers
   static bit ADDR_SEND = 0;           // Used by the ISR to flag byte
                                       // transmissions as slave addresses

   if (SMB0CN0_ARBLOST == 0)           // Check for errors
   {
      // Normal operation
      switch (stat)          // Status vector
      {
         // Master Transmitter/Receiver: START condition transmitted.
         case SMB_MTSTA:
            SMB0DAT = TARGET;          // Load address of the target slave
            SMB0DAT &= 0xFE;           // Clear the LSB of the address for the
                                       // R/W bit
            SMB0DAT |= SMB_RW;         // Load R/W bit
            SMB0CN0_STA = 0;           // Manually clear START bit
            ADDR_SEND = 1;
            break;

         // Master Transmitter: Data byte transmitted
         case SMB_MTDB:
            if (SMB0CN0_ACK)           // Slave SMB0CN0_ACK?
            {
            	//u2f_print("THERE WAS AN ACK %d %bx\r\n", ++inter, stat);
            	if (SMB_RW == 0)
            	{
					if (SMB_WRITE_BUF_OFFSET < SMB_WRITE_BUF_LEN)
					{
						SMB0DAT = SMB_WRITE_BUF[SMB_WRITE_BUF_OFFSET++];

					}
					else                    // If previous byte was not a slave
					{                       // address,
						SMB0CN0_STO = 1;     // Set SMB0CN0_STO to terminate transfer
						SMB_BUSY = 0;        // And free SMBus interface
					}
            	}
            	else
            	{
            		// do nothing and switch to receive mode
            	}
            }
            else                       // If slave NACK,
            {


				   SMB0CN0_STO = 1;        // Send STOP condition, followed
				   SMB0CN0_STA = 1;        // By a START
				   // NUM_ERRORS++;           // Indicate error

            	//u2f_print("NO ACK %d\r\n",++inter);
            }
            break;

         // Master Receiver: byte received
         case SMB_MRDB:
        	 //u2f_print("RECV\r\n");
        	 if (SMB_READ_OFFSET < SMB_READ_LEN)
        	 {
        		 SMB_READ_BUF[SMB_READ_OFFSET++] = SMB0DAT;
        		 SMB0CN0_ACK = 1;                   // Send ACK to indicate last byte
        	 }
        	 else
        	 {
                 SMB_BUSY = 0;              // Free SMBus interface
                 SMB0CN0_ACK = 0;                   // Send NACK to indicate last byte
                 SMB0CN0_STO = 1;           // Send STOP to terminate transfer
        	 }
                                       // of this transfer


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
      // SMB0CN0_ARBLOST = 1, error occurred... abort transmission
      FAIL = 1;
   } // end SMB0CN0_ARBLOST if

   if (FAIL)                           // If the transfer failed,
   {
	   u2f_print("i2c fail\r\n");
      SMB0CF &= ~0x80;                 // Reset communication
      SMB0CF |= 0x80;
      SMB0CN0_STA = 0;
      SMB0CN0_STO = 0;
      SMB0CN0_ACK = 0;

      SMB_BUSY = 0;                    // Free SMBus

      FAIL = 0;
      LED1 = LED_OFF;

      // NUM_ERRORS++;                    // Indicate an error occurred
   }

   SMB0CN0_SI = 0;                     // Clear interrupt flag
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

   SMB_BUSY = 0;                       // Free SMBus
}




