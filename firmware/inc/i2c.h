/*
 * i2c.h
 *
 *  Created on: Feb 14, 2016
 *      Author: Conor
 */

#ifndef I2C_H_
#define I2C_H_

void smb_init();

void smb_read (uint8_t addr, uint8_t* dest, uint8_t count);

void smb_write (uint8_t addr, uint8_t* buf, uint8_t len);

#endif /* I2C_H_ */
