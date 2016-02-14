/*
 * i2c.h
 *
 *  Created on: Feb 14, 2016
 *      Author: Conor
 */

#ifndef I2C_H_
#define I2C_H_

struct smb_interrupt_interface
{
	uint8_t* SMB_WRITE_BUF;
	uint8_t SMB_WRITE_BUF_LEN;
	uint8_t SMB_WRITE_BUF_OFFSET;
	uint8_t SMB_READ_LEN;
	uint8_t SMB_READ_OFFSET;
	uint8_t* SMB_READ_BUF;
};

extern struct smb_interrupt_interface SMB;
extern volatile uint8_t SMB_FLAGS;

extern volatile bit SMB_RW;
extern volatile bit SMB_BSSY;

//#define SMB_WRITE 	0x0
//#define SMB_READ 	0x1
//#define SMB_BUSY 	0x2


void smb_init();

void smb_read (uint8_t addr, uint8_t* dest, uint8_t count);

void smb_write (uint8_t addr, uint8_t* buf, uint8_t len);

#endif /* I2C_H_ */
