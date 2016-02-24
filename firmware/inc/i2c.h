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
	uint8_t addr;
	uint16_t crc;
	uint8_t crc_offset;
	uint8_t errors;

	uint8_t* write_buf;
	uint8_t write_len;
	uint8_t write_offset;

	uint8_t read_len;
	uint8_t read_offset;
	uint8_t* read_buf;

	uint8_t* write_ext_buf;
	uint8_t write_ext_len;
	uint8_t write_ext_offset;

	uint8_t preflags;

};

extern struct smb_interrupt_interface SMB;
extern volatile uint8_t SMB_FLAGS;

#define SMB_MAX_ERRORS 15
#define SMB_ERRORS_EXCEEDED(inter) ((inter)->errors > SMB_MAX_ERRORS)

#define SMB_WRITE			0x0
#define SMB_READ			0x1
#define SMB_BUSY			0x2
#define SMB_WRITE_EXT		0x4
#define SMB_READ_TRUNC		0x10
#define SMB_COMPUTE_CRC		0x20
#define SMB_RECV_NACK		0x40

#define SMB_READING() 				((SMB_FLAGS & SMB_READ))
#define SMB_WRITING() 				(!(SMB_FLAGS & SMB_READ))
#define SMB_WRITING_EXT() 			((SMB_FLAGS & SMB_WRITE_EXT))
#define SMB_IS_BUSY() 				((SMB_FLAGS & SMB_BUSY))
#define SMB_BUSY_CLEAR() 			(SMB_FLAGS &= ~SMB_BUSY)
#define SMB_HAS_CRC() 				(SMB_FLAGS & SMB_COMPUTE_CRC)
#define SMB_CRC_CLEAR() 			(SMB_FLAGS &= ~SMB_COMPUTE_CRC)
#define SMB_WAS_NACKED() 			(SMB_FLAGS & SMB_RECV_NACK)

void smb_init();

// read from I2C device, returns number of bytes read.
// sets truncated flag if it had to stop because count wasn't
// large enough
uint8_t smb_read (uint8_t addr, uint8_t* dest, uint8_t count);

// write to I2C device
void smb_write (uint8_t addr, uint8_t* buf, uint8_t len);

void smb_set_ext_write( uint8_t* extbuf, uint8_t extlen);

void smb_init_crc();

// reverse bits for a 16 bit int
uint16_t reverse_bits(uint16_t crc);

// does a crc on byte @b and accumulates it to crc @crc
uint16_t feed_crc(uint16_t crc, uint8_t b);

#endif /* I2C_H_ */
