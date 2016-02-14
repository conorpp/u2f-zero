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

volatile bit SMB_RW;
volatile bit SMB_BUSY;
volatile uint8_t SMB_FLAGS;



void smb_read (uint8_t addr, uint8_t* dest, uint8_t count)
{
   while(SMB_FLAGS & SMB_BSSY);

   SMB_RW = 1;
   SMB_FLAGS = SMB_READ | SMB_BSSY;

   SMB.SMB_READ_OFFSET = 0;
   SMB.addr = addr;
   SMB.SMB_READ_LEN = count;
   SMB.SMB_READ_BUF = dest;
   SMB0CN0_STA = 1;


   while(SMB_FLAGS & SMB_BSSY);
}


void smb_write (uint8_t addr, uint8_t* buf, uint8_t len)
{
   while(SMB_FLAGS & SMB_BSSY);
   SMB_RW = 0;
   SMB_FLAGS = SMB_WRITE | SMB_BSSY;
   SMB.write_buf_len = len;
   SMB.write_buf = buf;
   SMB.SMB_WRITE_BUF_OFFSET = 0;
   SMB.addr = addr;
   SMB0CN0_STA = 1;
}

void smb_init()
{
	memset(&SMB,0,sizeof(SMB));
	SMB_FLAGS = 0;
}
