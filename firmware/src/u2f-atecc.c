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

	eeprom_read(U2F_APP_CONFIG, (uint8_t* )&appconf, sizeof(struct APP_CONF));
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

			appconf.pulse_period = 20;
			appconf.idle_color = U2F_DEFAULT_COLOR;
			appconf.idle_color_prime = U2F_DEFAULT_COLOR_PRIME;

			flush_app_conf();

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
			rgb_hex(U2F_DEFAULT_COLOR_ERROR);
		}
		else
		{	// yellow
			rgb_hex(U2F_DEFAULT_COLOR_INPUT);
		}
		if (get_ms() - t > 10000)
			break;
		watchdog();
	}

	if (U2F_BUTTON_IS_PRESSED())
	{
		rgb_hex(U2F_DEFAULT_COLOR_INPUT_SUCCESS);
	}
	else
	{
		rgb_hex(U2F_DEFAULT_COLOR_ERROR);
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
		app_wink(U2F_DEFAULT_COLOR_WINK_OUT_OF_SPACE);
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
"\x30\x82\x01\x5a\x30\x82\x01\x00\x02\x01\x01\x30\x0a\x06\x08\x2a\x86\x48\xce\x3d"
"\x04\x03\x02\x30\x39\x31\x0b\x30\x09\x06\x03\x55\x04\x06\x13\x02\x56\x41\x31\x14"
"\x30\x12\x06\x03\x55\x04\x0a\x13\x0b\x43\x6f\x6e\x6f\x72\x43\x6f\x20\x4c\x4c\x43"
"\x31\x14\x30\x12\x06\x03\x55\x04\x03\x13\x0b\x75\x32\x66\x7a\x65\x72\x6f\x2e\x63"
"\x6f\x6d\x30\x1e\x17\x0d\x31\x36\x30\x34\x31\x36\x31\x34\x33\x33\x33\x34\x5a\x17"
"\x0d\x32\x32\x30\x34\x31\x35\x31\x34\x33\x33\x33\x34\x5a\x30\x39\x31\x0b\x30\x09"
"\x06\x03\x55\x04\x06\x13\x02\x56\x41\x31\x14\x30\x12\x06\x03\x55\x04\x0a\x13\x0b"
"\x43\x6f\x6e\x6f\x72\x43\x6f\x20\x4c\x4c\x43\x31\x14\x30\x12\x06\x03\x55\x04\x03"
"\x13\x0b\x75\x32\x66\x7a\x65\x72\x6f\x2e\x63\x6f\x6d\x30\x59\x30\x13\x06\x07\x2a"
"\x86\x48\xce\x3d\x02\x01\x06\x08\x2a\x86\x48\xce\x3d\x03\x01\x07\x03\x42\x00\x04"
"\x2e\x36\xf8\x28\xa5\xcd\x8b\x3e\xc7\x2e\x88\xbf\xb7\x62\x5a\x31\xef\xfb\x2d\x69"
"\x27\x22\x0f\x87\x2d\x0b\xda\x8e\xf1\xe0\x52\x5a\x9b\xec\xf2\x82\xc0\x11\x60\xd9"
"\x85\x4f\x5e\xc6\xcc\x90\xdf\xf7\xdc\x02\xa9\x2d\x6b\x44\xae\xf9\xa5\x1f\x3d\xdf"
"\x3b\x2d\xa3\x90\x30\x0a\x06\x08\x2a\x86\x48\xce\x3d\x04\x03\x02\x03\x48\x00\x30"
"\x45\x02\x20\x60\x7e\x5f\x9d\xec\xdc\x66\xac\xa1\x53\xc8\x83\xb2\x83\x64\xd8\x54"
"\xb4\x9f\xf5\x71\xc2\x92\x7b\xd8\xd8\x71\x50\xc6\x72\x94\xa5\x02\x21\x00\xd2\x68"
"\xaf\x1d\x55\x2b\xd5\xf6\x6f\xf7\xf4\xa8\x54\xb3\xc0\xd5\xd4\x1c\x21\x5e\x82\x03"
"\x1d\x07\x48\x18\x18\xfd\x04\x45\xd5\xbb"

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
