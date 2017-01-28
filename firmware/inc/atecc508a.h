/*
 * Copyright (c) 2016, Conor Patrick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#ifndef ATECC508A_H_
#define ATECC508A_H_

#define ATECC508A_ADDR 					0xc0

#define ATECC_CMD_COUNTER 				0x24
	// P1
	#define ATECC_COUNTER_READ 			0
	#define ATECC_COUNTER_INC 			1
	// P2
	#define ATECC_COUNTER0 				0
	#define ATECC_COUNTER1 				1


#define ATECC_CMD_RNG 					0x1B
	#define ATECC_RNG_P1 				0
	#define ATECC_RNG_P2 				0

#define ATECC_CMD_SHA 					0x47
	// P1
	#define ATECC_SHA_START 			0x0
	#define ATECC_SHA_UPDATE 			0x1
	#define ATECC_SHA_END 				0x2
	#define ATECC_SHA_HMACSTART			0x4
	#define ATECC_SHA_HMACEND			0x5
	// P2 is keyslot ID for hmac and message length otherwise

#define ATECC_CMD_READ 					0x02
	// P1
	#define ATECC_RW_CONFIG 			0x00
	#define ATECC_RW_OTP 				0x01
	#define ATECC_RW_DATA 				0x02
	#define ATECC_RW_EXT 				0x80
	// P2 read addr

#define ATECC_CMD_WRITE 				0x12
	// P1 same as for read
	// P2 write addr

	#define ATECC_EEPROM_SLOT(x) 		(0x5 + ((x)>>1))
	#define ATECC_EEPROM_SLOT_OFFSET(x) ( (x) & 1 ? 2 : 0 )
	#define ATECC_EEPROM_SLOT_SIZE 		0x2

	#define ATECC_EEPROM_KEY(x) 		(24 + ((x)>>1))
	#define ATECC_EEPROM_KEY_OFFSET(x) 	( (x) & 1 ? 2 : 0 )
	#define ATECC_EEPROM_KEY_SIZE 0x2

	#define ATECC_EEPROM_DATA_SLOT(x) 		(x<<3)

	#define ATECC_EEPROM_B2A(b) 		((b)>>2)
	#define ATECC_EEPROM_B2O(b) 		((b)&0x3)

#define ATECC_CMD_LOCK 					0x17
	// P1 flags
	#define ATECC_LOCK_CONFIG 			0x00
	#define ATECC_LOCK_DATA_OTP 		0x01
	#define ATECC_LOCK_SLOT 			0x02
	#define ATECC_LOCK_SLOTNUM(x) 		(((x)&0xf)<<2)
	#define ATECC_LOCK_IGNORE_SUMMARY 	0x08
	// P2 is CRC or 0

#define ATECC_CMD_GENKEY 				0x40
	// P1
	#define ATECC_GENKEY_PRIVATE 		0x04
	#define ATECC_GENKEY_PUBLIC 		0x00
	#define ATECC_GENKEY_PUBDIGEST 		0x08
	#define ATECC_GENKEY_PUBDIGEST2 	0x10
	// P2 is keyid

#define ATECC_CMD_NONCE 				0x16
	// P1
	#define ATECC_NONCE_RNG_UPDATE 		0x0
	#define ATECC_NONCE_TEMP_UPDATE 	0x3
	// P2 is 0

#define ATECC_CMD_SIGN 					0x41
	// P1
	#define ATECC_SIGN_INTERNAL 		0x00
	#define ATECC_SIGN_EXTERNAL 		0x80
	// P2 is keyid

#define ATECC_CMD_GENDIG				0x15
	// P1 same as for read
	// P2 is keyid

#define ATECC_CMD_INFO					0x30
	// P1 same as for read
	#define ATECC_INFO_REVISION 		0x00
	#define ATECC_INFO_KEYVALID 		0x01
	#define ATECC_INFO_STATE 			0x02
	#define ATECC_INFO_GPIO 			0x03
	// P2 is keyid

#define ATECC_CMD_PRIVWRITE				0x46
	// P1
	#define ATECC_PRIVWRITE_ENC			0x40
	// P2 is keyid


struct atecc_response
{
	// length of payload
	uint8_t len;
	// payload
	uint8_t* buf;
};

struct atecc_slot_config
{
	uint8_t readkey : 4;
	uint8_t nomac : 1;
	uint8_t limiteduse : 1;
	uint8_t encread : 1;
	uint8_t secret : 1;
	uint8_t writekey : 4;
	uint8_t writeconfig : 4;
};

struct atecc_key_config
{
	uint8_t private : 1;
	uint8_t pubinfo : 1;
	uint8_t keytype : 3;
	uint8_t lockable : 1;
	uint8_t reqrandom : 1;
	uint8_t reqauth : 1;
	uint8_t authkey : 4;
	uint8_t intrusiondisable : 1;
	uint8_t rfu : 1;
	uint8_t x509id : 2;
};

void atecc_idle();
void atecc_wake();
void atecc_sleep();

int8_t atecc_send(uint8_t cmd, uint8_t p1, uint16_t p2,
					uint8_t * buf, uint8_t len);

int8_t atecc_recv(uint8_t * buf, uint8_t buflen, struct atecc_response* res);

int8_t atecc_send_recv(uint8_t cmd, uint8_t p1, uint16_t p2,
							uint8_t* tx, uint8_t txlen, uint8_t * rx,
							uint8_t rxlen, struct atecc_response* res);

int8_t atecc_write_eeprom(uint8_t base, uint8_t offset, uint8_t* srcbuf, uint8_t len);


#endif /* ATECC508A_H_ */
