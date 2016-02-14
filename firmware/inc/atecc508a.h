/*
 * atecc508a.h
 *
 *  Created on: Feb 13, 2016
 *      Author: Conor
 */

#ifndef ATECC508A_H_
#define ATECC508A_H_

#define ATECC508A_ADDR 0xc0

#define ATECC_CMD_COUNTER 0x24
#define ATECC_COUNTER0 0
#define ATECC_COUNTER1 1
#define ATECC_COUNTER_READ 0
#define ATECC_COUNTER_INC 1

uint16_t atecc_crc16(uint8_t* buf, uint16_t len);

void atecc_send(uint8_t cmd, uint8_t p1, uint16_t p2,
					uint8_t * buf, uint8_t len);

uint8_t atecc_recv(uint8_t * buf, uint8_t buflen);

#endif /* ATECC508A_H_ */
