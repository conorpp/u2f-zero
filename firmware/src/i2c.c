/*
 * i2c.c
 *
 *  Created on: Feb 14, 2016
 *      Author: Conor
 */
#include <SI_EFM8UB1_Register_Enums.h>
#include <stdint.h>

#include "i2c.h"

extern uint8_t* SMB_WRITE_BUF;
extern uint8_t SMB_WRITE_BUF_LEN;
extern uint8_t SMB_WRITE_BUF_OFFSET;
extern uint8_t SMB_READ_LEN;
extern uint8_t SMB_READ_OFFSET;
extern uint8_t* SMB_READ_BUF;
volatile bit SMB_RW;
volatile bit SMB_BUSY;

uint8_t SMB_DATA_IN;                   // Global holder for SMBus data
                                       // All receive data is written here

uint8_t SMB_DATA_OUT;                  // Global holder for SMBus data.
                                       // All transmit data is read from here

uint8_t TARGET;                        // Target SMBus slave address

void smb_read (uint8_t addr, uint8_t* dest, uint8_t count)
{
   while(SMB_BUSY != 0);               // Wait for transfer to complete

   SMB_BUSY = 1;                       // Claim SMBus (set to busy)

   SMB_RW = 1;                         // Mark this transfer as a READ

   SMB_READ_OFFSET = 0;
   TARGET = addr;
   SMB_READ_LEN = count;
   SMB_READ_BUF = dest;
   SMB0CN0_STA = 1;                    // Start transfer


   while(SMB_BUSY);                    // Wait for transfer to complete
}


void smb_write (uint8_t addr, uint8_t* buf, uint8_t len)
{
   while(SMB_BUSY);                    // Wait for SMBus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 0;                         // Mark this transfer as a WRITE
   SMB_WRITE_BUF_LEN = len;
   SMB_WRITE_BUF = buf;
   SMB_WRITE_BUF_OFFSET = 0;
   TARGET = addr;
   SMB0CN0_STA = 1;                    // Start transfer
}

void smb_init()
{
	SMB_WRITE_BUF = NULL;
	SMB_READ_BUF = NULL;
	SMB_READ_OFFSET = 0;
	SMB_WRITE_BUF_LEN = 0;
	SMB_WRITE_BUF_OFFSET = 0;
	SMB_READ_LEN = 0;
}
