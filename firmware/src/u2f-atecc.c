/*
 * u2f-atecc.c
 *
 *  Created on: Feb 27, 2016
 *      Author: pp
 */

#include "app.h"
#ifndef U2F_DISABLE
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

#define U2F_NUM_KEYS 			U2F_ATTESTATION_KEY_SLOT
#define U2F_KEYS_ADDR			(0xF800)
#define U2F_KEY_ADDR(k)			(U2F_KEYS_ADDR + ((k)*U2F_KEY_HANDLE_SIZE))


#define IS_KEY_VALID(mask,key)	((~mask) & (1<<key))

static struct u2f_hid_msg res;
static uint8_t* resbuf = (uint8_t*)&res;
static uint8_t resseq = 0;
static uint8_t serious = 0;

// 0 if same, 1 otherwise
static uint8_t key_same(struct key_handle * k1, struct key_handle * k2)
{
	uint8_t i;
	if (k1->index != k2->index) return 1;
	for (i=0; i < U2F_KEY_HANDLE_SIZE-1; i++)
	{
		if (k1->entropy[i] != k2->entropy[i])
			return 1;
	}
	return 0;
}

static void flush_key_store()
{
	eeprom_erase(U2F_EEPROM_CONFIG);
	eeprom_write(U2F_EEPROM_CONFIG, (uint8_t* )&key_store, sizeof(struct key_storage_header));
}

int8_t u2f_wipe_keys()
{
	uint8_t presses = 5;
	serious = 1;
	while(presses--)
	{
		if (u2f_get_user_feedback() != 0)
		{
			goto nowipe;
		}
	}

	// wipe
	serious = 0;
	eeprom_erase(U2F_EEPROM_CONFIG);
	u2f_init();
	return 0;

	nowipe:
	serious = 0;
	return -1;
}

void u2f_init()
{
	uint8_t i,ec;
	struct atecc_response res;

	eeprom_read(U2F_EEPROM_CONFIG, (uint8_t* )&key_store, sizeof(struct key_storage_header));


	// initialize key handles
	if (key_store.num_keys != U2F_NUM_KEYS)
	{
		watchdog();
		key_store.num_keys = U2F_NUM_KEYS;
		key_store.valid_keys = 0;
		key_store.num_issued = 0;
		flush_key_store();

		for (i=0; i < U2F_NUM_KEYS; i++)
		{
			watchdog();
			ec = atecc_send_recv(ATECC_CMD_RNG,ATECC_RNG_P1,ATECC_RNG_P2,
							NULL, 0,
							appdata.tmp,
							sizeof(appdata.tmp), &res);
			if (ec != 0)
			{
				u2f_printb("REDO! REDO! ",1,i);
				eeprom_erase(U2F_EEPROM_CONFIG);
				// reset
				reboot();
			}
			res.buf[0] = i+1;
			eeprom_write(U2F_KEYS_ADDR + i * U2F_KEY_HANDLE_SIZE,
							res.buf, U2F_KEY_HANDLE_SIZE);
		}

	}
}

void u2f_response_writeback(uint8_t * buf, uint16_t len)
{
	u2f_hid_writeback(buf, len);
}

void u2f_response_flush()
{
	watchdog();
	u2f_hid_flush();
}

void u2f_response_start()
{
	watchdog();
}

int8_t u2f_get_user_feedback()
{
	uint32_t t;
	u2f_delay(1);
	t = get_ms();
	while(U2F_BUTTON_IS_PRESSED()){}
	while(!U2F_BUTTON_IS_PRESSED())
	{
		// turn red
		if (serious)
		{
			rgb(200,0,0);
		}
		else
		{	// yellow
			rgb(128,128,0);
		}
		if (get_ms() - t > 10000)
			break;
		watchdog();
	}

	if (U2F_BUTTON_IS_PRESSED())
	{
		rgb(0,150,128);
	}
	else
	{
		rgb(200,0,0);
		return 1;
	}

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
	watchdog();
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


int8_t u2f_ecdsa_sign(uint8_t * dest, uint8_t * handle)
{
	struct atecc_response res;
	struct key_handle k;
	uint16_t keyslot = (uint16_t)((struct key_handle *)handle)->index;
	watchdog();
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

		if (key_same((struct key_handle *)handle, &k) != 0)
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
		app_wink(150,15,15);
		return -1;
	}
	watchdog();
	atecc_send_recv(ATECC_CMD_GENKEY,
			ATECC_GENKEY_PRIVATE, keyslot, NULL, 0,
			appdata.tmp, sizeof(appdata.tmp), &res);

	memmove(pubkey, res.buf, 64);

	eeprom_read(U2F_KEY_ADDR(keyslot), (uint8_t* )&k, U2F_KEY_HANDLE_SIZE);
	if (k.index-1 != keyslot)
	{

		k.index = keyslot;
		set_app_error(ERROR_BAD_KEY_STORE);
	}
	memmove(handle, &k, U2F_KEY_HANDLE_SIZE);
	key_store.num_issued++;
	flush_key_store();

	return 0;
}

int8_t u2f_load_key(uint8_t * handle, uint8_t len)
{
	struct key_handle k;
	uint8_t keyslot = handle[0]-1;
	if (keyslot >= U2F_NUM_KEYS)
	{
		return -1;
	}
	eeprom_read(U2F_KEY_ADDR(keyslot), (uint8_t* )&k, U2F_KEY_HANDLE_SIZE);

	if (key_same((struct key_handle *)handle, &k) != 0)
	{
		return -1;
	}
	return 0;
}

uint32_t u2f_count()
{
	struct atecc_response res;
	atecc_send_recv(ATECC_CMD_COUNTER,
			ATECC_COUNTER_INC, ATECC_COUNTER0,NULL,0,
			appdata.tmp, sizeof(appdata.tmp), &res);
	return le32toh(*(uint32_t*)res.buf);
}

code char __attest[] =
// {{{
"\x30\x82\x01\x5b\x30\x82\x01\x00\x02\x01\x01\x30\x0a\x06\x08\x2a\x86\x48\xce\x3d"
"\x04\x03\x02\x30\x39\x31\x0b\x30\x09\x06\x03\x55\x04\x06\x13\x02\x56\x41\x31\x14"
"\x30\x12\x06\x03\x55\x04\x0a\x13\x0b\x43\x6f\x6e\x6f\x72\x43\x6f\x20\x4c\x4c\x43"
"\x31\x14\x30\x12\x06\x03\x55\x04\x03\x13\x0b\x75\x32\x66\x7a\x65\x72\x6f\x2e\x63"
"\x6f\x6d\x30\x1e\x17\x0d\x31\x36\x30\x34\x30\x32\x30\x35\x31\x32\x32\x37\x5a\x17"
"\x0d\x32\x32\x30\x34\x30\x31\x30\x35\x31\x32\x32\x37\x5a\x30\x39\x31\x0b\x30\x09"
"\x06\x03\x55\x04\x06\x13\x02\x56\x41\x31\x14\x30\x12\x06\x03\x55\x04\x0a\x13\x0b"
"\x43\x6f\x6e\x6f\x72\x43\x6f\x20\x4c\x4c\x43\x31\x14\x30\x12\x06\x03\x55\x04\x03"
"\x13\x0b\x75\x32\x66\x7a\x65\x72\x6f\x2e\x63\x6f\x6d\x30\x59\x30\x13\x06\x07\x2a"
"\x86\x48\xce\x3d\x02\x01\x06\x08\x2a\x86\x48\xce\x3d\x03\x01\x07\x03\x42\x00\x04"
"\x81\x3d\x72\x3d\x27\xef\x60\x74\xb0\x8a\x2d\xa5\xd1\x8a\x13\x0c\x89\x88\x93\x9d"
"\x9d\x7b\x7f\xf3\xe9\x13\x81\x91\xf3\xd7\x58\x5c\x28\xdb\xd3\x58\x57\x0a\xa0\xab"
"\x60\x23\x7f\xa5\x23\x24\x99\x8c\x5a\x87\x7f\x6c\xef\xe7\xf8\x83\x3e\x8a\x3e\x31"
"\x88\xaa\xc4\x05\x30\x0a\x06\x08\x2a\x86\x48\xce\x3d\x04\x03\x02\x03\x49\x00\x30"
"\x46\x02\x21\x00\xa1\x54\x45\x9b\xdd\xdb\x59\xcb\xfb\x0b\xb0\xee\x1f\x81\x41\xf3"
"\x32\xc4\x72\x7b\x3d\x9c\x53\xd3\xc7\x44\x5d\x3b\x61\xac\x73\x1e\x02\x21\x00\xea"
"\x30\xab\x0f\x30\x8e\x29\xbc\x23\x6a\x94\x3b\xbd\xd8\xf8\x8d\x18\xaa\xf1\x62\xbb"
"\xf8\xcc\x5d\x3a\xbd\xe1\x7d\xc9\x5c\xdd\x07"

// }}}
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
