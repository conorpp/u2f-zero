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

#define ATECC_CMD_RNG 0x1B
	#define ATECC_RNG_P1 0
	#define ATECC_RNG_P2 0

#define ATECC_CMD_SHA 0x47
	#define ATECC_SHA_START 0x0
	#define ATECC_SHA_UPDATE 0x1
	#define ATECC_SHA_END 0x2

void atecc_send(uint8_t cmd, uint8_t p1, uint16_t p2,
					uint8_t * buf, uint8_t len);

uint8_t atecc_recv(uint8_t * buf, uint8_t buflen);

#endif /* ATECC508A_H_ */
