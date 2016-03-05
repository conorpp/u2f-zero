/*
 * u2f-atecc.c
 *
 *  Created on: Feb 27, 2016
 *      Author: pp
 */
#include "app.h"
#include "bsp.h"
#include "u2f.h"
#include "u2f_hid.h"
#include "eeprom.h"
#include "atecc508a.h"



struct key_handle
{
	uint8_t index;
	uint8_t entropy[3];
};

struct key_storage_header
{
	uint8_t num_keys;
	uint16_t valid_keys;
	uint8_t num_issued;
} key_store;

#define U2F_NUM_KEYS 			14
#define U2F_KEY_HEADER_ADDR		0xF800
#define U2F_KEYS_ADDR			(0xF800 + sizeof(struct key_storage_header))

#define IS_KEY_VALID(mask,key)	((~mask) & (1<<key))

static struct u2f_hid_msg res;
static uint8_t* resbuf = (uint8_t*)&res;
static uint8_t resoffset = 0;
static uint8_t resseq = 0;

static void flush_key_store()
{
	eeprom_write(U2F_KEY_HEADER_ADDR, (uint8_t* )&key_store, sizeof(struct key_storage_header));
}

void u2f_init()
{
	uint8_t i,j;
	struct atecc_response res;

	eeprom_read(U2F_KEY_HEADER_ADDR, (uint8_t* )&key_store, sizeof(struct key_storage_header));

	// initialize key handles
	if (key_store.num_keys != U2F_NUM_KEYS)
	{
		key_store.num_keys = U2F_NUM_KEYS;
		key_store.valid_keys = 0;
		key_store.num_issued = 0;
		flush_key_store();

		for (i=0; i < 2; i++)
		{
			atecc_send_recv(ATECC_CMD_RNG,ATECC_RNG_P1,ATECC_RNG_P2,
							NULL, 0,
							appdata.tmp,
							sizeof(appdata.tmp), &res);
			for (j=0; j < U2F_NUM_KEYS/2; j++) res.buf[j * U2F_KEY_HANDLE_SIZE] = j+1 + i*U2F_NUM_KEYS/2;
			eeprom_write(U2F_KEYS_ADDR + i * (U2F_KEY_HANDLE_SIZE * U2F_NUM_KEYS/2),
							res.buf, U2F_KEY_HANDLE_SIZE * U2F_NUM_KEYS/2);
		}

	}
}

void u2f_response_writeback(uint8_t * buf, uint8_t len)
{
	u2f_hid_writeback(buf, len);
}

void u2f_response_flush()
{
	USBD_Write(EP1IN, resbuf, 64, false);
	resseq = 0;
}

void u2f_response_start()
{
	resoffset = 0;
}

int8_t u2f_get_user_feedback()
{
	return 0;
}

static uint8_t shabuf[64];
static uint8_t shaoffset = 0;
static struct atecc_response res_digest;

void u2f_sha256_start()
{
	shaoffset = 0;
	atecc_send_recv(ATECC_CMD_SHA,
			ATECC_SHA_START, 0,NULL,0,
			appdata.tmp, sizeof(appdata.tmp), NULL);
}


void u2f_sha256_update(uint8_t * buf, uint8_t len)
{
	uint8_t i = 0;
	while(len--)
	{
		shabuf[shaoffset++] = *buf++;
		if (shaoffset == 64)
		{
			atecc_send_recv(ATECC_CMD_SHA,
					ATECC_SHA_UPDATE, 64,shabuf,64,
					appdata.tmp, sizeof(appdata.tmp), NULL);
			shaoffset = 0;
		}
	}
}


void u2f_sha256_finish()
{
	atecc_send_recv(ATECC_CMD_SHA,
			ATECC_SHA_END, shaoffset,shabuf,shaoffset,
			shabuf, sizeof(shabuf), &res_digest);
}


void u2f_ecdsa_sign(uint8_t * dest, uint8_t * handle)
{

}


void u2f_new_keypair(uint8_t * handle, uint8_t * pubkey)
{

}

code char __attest[] =
"\x30\x82\x01\x72\x30\x82\x01\x18\x02\x01\x01\x30\x0a\x06\x08\x2a\x86\x48\xce\x3d"
"\x04\x03\x02\x30\x45\x31\x0b\x30\x09\x06\x03\x55\x04\x06\x13\x02\x41\x55\x31\x13"
"\x30\x11\x06\x03\x55\x04\x08\x0c\x0a\x53\x6f\x6d\x65\x2d\x53\x74\x61\x74\x65\x31"
"\x21\x30\x1f\x06\x03\x55\x04\x0a\x0c\x18\x49\x6e\x74\x65\x72\x6e\x65\x74\x20\x57"
"\x69\x64\x67\x69\x74\x73\x20\x50\x74\x79\x20\x4c\x74\x64\x30\x1e\x17\x0d\x31\x36"
"\x30\x32\x32\x37\x31\x36\x31\x33\x30\x35\x5a\x17\x0d\x32\x32\x30\x32\x32\x35\x31"
"\x36\x31\x33\x30\x35\x5a\x30\x45\x31\x0b\x30\x09\x06\x03\x55\x04\x06\x13\x02\x41"
"\x55\x31\x13\x30\x11\x06\x03\x55\x04\x08\x0c\x0a\x53\x6f\x6d\x65\x2d\x53\x74\x61"
"\x74\x65\x31\x21\x30\x1f\x06\x03\x55\x04\x0a\x0c\x18\x49\x6e\x74\x65\x72\x6e\x65"
"\x74\x20\x57\x69\x64\x67\x69\x74\x73\x20\x50\x74\x79\x20\x4c\x74\x64\x30\x59\x30"
"\x13\x06\x07\x2a\x86\x48\xce\x3d\x02\x01\x06\x08\x2a\x86\x48\xce\x3d\x03\x01\x07"
"\x03\x42\x00\x04\x36\xd9\x9a\xc6\xa8\xda\x88\x55\x18\x85\x8c\xed\xad\xe5\x51\xe0"
"\x78\x76\xc0\x1c\x19\xe8\xe8\xdf\xde\xee\xd1\x2d\x8c\x8f\xad\x5c\x52\xa7\x5e\x74"
"\x52\x9a\x65\x45\xf8\x69\xfc\xe8\x8c\x0c\x42\xd7\x1e\x2a\x78\x54\x08\x61\x19\x01"
"\x00\x1b\x77\x60\x8a\x92\x8a\x29\x30\x0a\x06\x08\x2a\x86\x48\xce\x3d\x04\x03\x02"
"\x03\x48\x00\x30\x45\x02\x21\x00\xc5\xa9\x27\xa6\xf3\x06\xee\xbd\xd5\xbe\x47\xe8"
"\x52\xc4\x37\x63\xd4\xc0\x01\x86\xbc\xd0\x87\xe9\x54\x76\x4e\x81\x6c\xd7\x56\x72"
"\x02\x20\x76\x77\x24\x93\x98\xc0\x0e\x0d\x3e\xfd\x6e\x01\x56\x42\x56\xec\xcf\xba"
"\x8a\xa5\x6c\x5e\x28\x51\x37\x8d\x55\xf4\x0d\x12\x58\x9d"
;

uint8_t * u2f_get_attestation_cert()
{
	return __attest;
}


uint16_t u2f_attestation_cert_size()
{
	return sizeof(__attest);
}

