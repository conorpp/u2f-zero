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

#ifndef U2F_DISABLE

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

#define U2F_NUM_KEYS 			U2F_ATTESTATION_KEY_SLOT
#define U2F_KEYS_ADDR			(0xF800)
#define U2F_KEY_ADDR(k)			(U2F_KEYS_ADDR + ((k)*U2F_KEY_HANDLE_SIZE))


#define IS_KEY_VALID(mask,key)	((~mask) & (1<<key))

static struct u2f_hid_msg res;
static uint8_t* resbuf = (uint8_t*)&res;
static uint8_t resseq = 0;

static void flush_key_store()
{
	eeprom_erase(U2F_EEPROM_CONFIG);
	eeprom_write(U2F_EEPROM_CONFIG, (uint8_t* )&key_store, sizeof(struct key_storage_header));
}

void u2f_init()
{
	uint8_t i,j;
	struct atecc_response res;

	eeprom_read(U2F_EEPROM_CONFIG, (uint8_t* )&key_store, sizeof(struct key_storage_header));

	// initialize key handles
	if (key_store.num_keys != U2F_NUM_KEYS)
	{
		u2f_prints("init key handles\r\n");
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
	u2f_hid_flush();
}

void u2f_response_start()
{
}

int8_t u2f_get_user_feedback()
{
	return 0;
}

static uint8_t shabuf[70];
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

	dump_hex(res_digest.buf, res_digest.len);
}


int8_t u2f_ecdsa_sign(uint8_t * dest, uint8_t * handle)
{
	struct atecc_response res;
	struct key_handle k;
	uint16_t keyslot = ((struct key_handle *)handle)->index;
	if (keyslot > U2F_NUM_KEYS)
	{
		return -1;
	}
	if (keyslot == 0)
	{
		keyslot = U2F_ATTESTATION_KEY_SLOT;
	}
	else
	{
		keyslot--;
	}
	atecc_send_recv(ATECC_CMD_SIGN,
			ATECC_SIGN_EXTERNAL, keyslot, NULL, 0,
			appdata.tmp, sizeof(appdata.tmp), &res);

	if (keyslot != U2F_ATTESTATION_KEY_SLOT)
	{
		eeprom_read(U2F_KEY_ADDR(keyslot), (uint8_t* )&k, U2F_KEY_HANDLE_SIZE);

		if (
				((struct key_handle *)handle)->index-1 != k.index ||
				((struct key_handle *)handle)->entropy[0] != k.entropy[0] ||
				((struct key_handle *)handle)->entropy[1] != k.entropy[1] ||
				((struct key_handle *)handle)->entropy[2] != k.entropy[2]
		)
		{
			return -1;
		}
	}

	memmove(dest, res.buf, 64);

	return 0;
}

// bad if this gets interrupted
int8_t u2f_new_keypair(uint8_t * handle, uint8_t * pubkey)
{
	struct atecc_response res;
	struct key_handle k;
	uint8_t keyslot = key_store.num_issued;
	if (keyslot > U2F_NUM_KEYS-1)
	{
		return -1;
	}

	atecc_send_recv(ATECC_CMD_GENKEY,
			ATECC_GENKEY_PRIVATE, keyslot, NULL, 0,
			appdata.tmp, sizeof(appdata.tmp), &res);

	memmove(pubkey, res.buf, 64);

	eeprom_read(U2F_KEY_ADDR(keyslot), (uint8_t* )&k, U2F_KEY_HANDLE_SIZE);
	if (k.index != keyslot)
	{
		k.index = keyslot;
		set_app_error(ERROR_BAD_KEY_STORE);
	}
	k.index++;
	memmove(handle, &k, U2F_KEY_HANDLE_SIZE);
	key_store.num_issued++;
	flush_key_store();

	return 0;
}

code char __attest[] =
		"\x30\x82\x01\x5a\x30\x82\x01\x00\x02\x01\x01\x30\x0a\x06\x08\x2a\x86\x48\xce\x3d"
		"\x04\x03\x02\x30\x39\x31\x0b\x30\x09\x06\x03\x55\x04\x06\x13\x02\x56\x41\x31\x14"
		"\x30\x12\x06\x03\x55\x04\x0a\x13\x0b\x43\x6f\x6e\x6f\x72\x43\x6f\x20\x4c\x4c\x43"
		"\x31\x14\x30\x12\x06\x03\x55\x04\x03\x13\x0b\x75\x32\x66\x7a\x65\x72\x6f\x2e\x63"
		"\x6f\x6d\x30\x1e\x17\x0d\x31\x36\x30\x33\x30\x36\x30\x32\x32\x38\x33\x38\x5a\x17"
		"\x0d\x32\x32\x30\x33\x30\x35\x30\x32\x32\x38\x33\x38\x5a\x30\x39\x31\x0b\x30\x09"
		"\x06\x03\x55\x04\x06\x13\x02\x56\x41\x31\x14\x30\x12\x06\x03\x55\x04\x0a\x13\x0b"
		"\x43\x6f\x6e\x6f\x72\x43\x6f\x20\x4c\x4c\x43\x31\x14\x30\x12\x06\x03\x55\x04\x03"
		"\x13\x0b\x75\x32\x66\x7a\x65\x72\x6f\x2e\x63\x6f\x6d\x30\x59\x30\x13\x06\x07\x2a"
		"\x86\x48\xce\x3d\x02\x01\x06\x08\x2a\x86\x48\xce\x3d\x03\x01\x07\x03\x42\x00\x04"
		"\xe0\xc4\x32\x2c\x33\xdc\x1e\xa3\x60\x10\x0a\x0f\x2a\xb7\xb7\x2c\xbe\x53\x3d\xac"
		"\x4e\x9d\x4f\xee\x4a\xf5\x6c\xcd\xe9\xe2\x23\x7f\x9c\x65\x15\x5d\x9f\x25\x19\x72"
		"\x87\xd5\x2f\x23\x9a\xb3\x41\xeb\x0c\xb5\xbb\x4b\xae\xc3\x9a\xf3\xe4\xfb\xa7\xd2"
		"\x33\x9b\xa2\xa8\x30\x0a\x06\x08\x2a\x86\x48\xce\x3d\x04\x03\x02\x03\x48\x00\x30"
		"\x45\x02\x20\x12\xa2\x68\x62\xf4\x4d\x27\x61\xdf\x01\xc8\x74\x15\xba\xa6\x0d\x51"
		"\x93\x70\x3c\xcc\x39\xe2\x2d\xfd\x71\x99\x7c\xd3\x26\x18\x2b\x02\x21\x00\xfa\x69"
		"\x16\x10\xaa\xc0\xe7\x9b\x73\xea\x2a\xa3\x53\x6a\x13\x04\xcc\xf0\x04\x82\x4c\x3c"
		"\xe4\xd4\xc9\x3b\x2e\xaa\x39\x9d\x60\x93"
;

uint8_t * u2f_get_attestation_cert()
{
	return __attest;
}


uint16_t u2f_attestation_cert_size()
{
	return sizeof(__attest)-1;
}

void set_response_length(uint16_t len)
{
	u2f_hid_set_len(len);
}

#endif
