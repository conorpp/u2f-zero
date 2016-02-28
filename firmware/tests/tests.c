/*
 * tests.c
 *
 *  Created on: Feb 14, 2016
 *      Author: Conor
 */

#include <stdarg.h>
#include <stdio.h>
#include "app.h"
#include "bsp.h"
#include "i2c.h"
#include "atecc508a.h"
#include "tests.h"


#ifdef ENABLE_TESTS


static void PRINT(const char * fmt, ...)
{
	va_list args;

	va_start(args,fmt);
	vsprintf(appdata.hidmsgbuf, fmt, args);
	va_end(args);

	u2f_write_s(appdata.hidmsgbuf);
}


#ifdef TEST_SHA
static int test_sha()
{
	uint8_t buf[40];
	uint8_t len;
	char pw[] = "2pckJ4IkT3PwdGMwuCygPpxD6+lObNGORiLGPQxM4ef4YoNvx9/k0xskZl84rCd3TllCvitepe+B";
	do{
		atecc_send(ATECC_CMD_SHA,
				ATECC_SHA_START,
				0,NULL,0);
	}while((len = atecc_recv(buf,sizeof(buf))) < 0);
	dump_hex(buf,len);

	do{
		atecc_send(ATECC_CMD_SHA,
				ATECC_SHA_UPDATE,
				64,pw,64);
	} while ((len = atecc_recv(buf,sizeof(buf))) < 0);
	dump_hex(buf,len);

	do{
		atecc_send(ATECC_CMD_SHA,
				ATECC_SHA_END,
				sizeof(pw)-65,pw+64,sizeof(pw)-65);
	}while((len = atecc_recv(buf,sizeof(buf))) < 0);
	dump_hex(buf,len);

	// sha256 sum should be bcddd71b48f8a31d1374ad51c2e4138a871cb7f1eb3f2bdab49bc9bc60afc3a5
	return (SMB.crc == 0x9768) ? 0 : -1;
}
#else
#define test_sha(x)
#endif

#ifdef TEST_EEPROM

static void slot_dump(void* slot)
{
	struct atecc_slot_config* a = (struct atecc_slot_config*) slot;
	flush_messages();
	u2f_print("    readkey %bx\r\n", a->readkey);
	if (a->nomac) u2f_print("    nomac\r\n");
	flush_messages();
	if (a->limiteduse) u2f_print("    limiteduse\r\n");
	if (a->encread) u2f_print("    encread\r\n");
	flush_messages();
	if (a->secret) u2f_print("    secret\r\n");
	u2f_print("    writekey %bx\r\n", a->writekey);
	flush_messages();
	u2f_print("    writeconfig %bx\r\n", a->writeconfig);
	flush_messages();
}

static void key_dump(void* slot)
{
	struct atecc_key_config* a = (struct atecc_slot_config*) slot;
	flush_messages();

	if (a->private) u2f_print("    private\r\n");
	if (a->pubinfo) u2f_print("    pubinfo\r\n");
	flush_messages();
	u2f_print("    keytype %bx\r\n", a->keytype);
	if (a->lockable) u2f_print("    lockable\r\n");
	flush_messages();
	if (a->reqrandom) u2f_print("    reqrandom\r\n");
	if (a->reqauth) u2f_print("    reqauth\r\n");
	flush_messages();
	u2f_print("    authkey %bx\r\n", a->authkey);
	if (a->intrusiondisable) u2f_print("    intrusiondisable\r\n");
	flush_messages();
	if (a->rfu) u2f_print("    rfu\r\n");
	u2f_print("    x509id %bx\r\n", a->x509id);
	flush_messages();
}

static int test_eeprom()
{
	uint8_t buf[7];
	uint16_t c1,c2,c3,c4;

	struct atecc_response res;
	struct atecc_slot_config slotconfig;
	struct atecc_key_config keyconfig;

	memset(&slotconfig, 0, sizeof(struct atecc_slot_config));
	memset(&keyconfig, 0, sizeof(struct atecc_key_config));

	slotconfig.secret = 1;
	slotconfig.writeconfig = 0xA;
	slotconfig.readkey = 0x3;


	if (atecc_write_eeprom(ATECC_EEPROM_SLOT(0), ATECC_EEPROM_SLOT_OFFSET(0), &slotconfig, ATECC_EEPROM_SLOT_SIZE) != 0)
	{
		return -1;
	}

	slotconfig.writeconfig = 0x3;

	if (atecc_write_eeprom(ATECC_EEPROM_SLOT(1), ATECC_EEPROM_SLOT_OFFSET(1), &slotconfig, ATECC_EEPROM_SLOT_SIZE) != 0)
	{
		return -1;
	}

	keyconfig.private = 1;
	keyconfig.pubinfo = 1;
	keyconfig.keytype = 0x4;
	keyconfig.lockable = 1;

	if (atecc_write_eeprom(ATECC_EEPROM_KEY(0), ATECC_EEPROM_KEY_OFFSET(0), &keyconfig, ATECC_EEPROM_KEY_SIZE) != 0)
	{
		return -1;
	}

	keyconfig.keytype = 0x3;

	if (atecc_write_eeprom(ATECC_EEPROM_KEY(1), ATECC_EEPROM_KEY_OFFSET(1), &keyconfig, ATECC_EEPROM_KEY_SIZE) != 0)
	{
		return -1;
	}


	atecc_send_recv(ATECC_CMD_READ,
			ATECC_RW_CONFIG, 5,NULL,0,
			buf,sizeof(buf), &res);

	u2f_print("-- slot 0 --\r\n");
	dump_hex(res.buf,2);
	slot_dump(res.buf);

	u2f_print("-- slot 1 --\r\n");
	dump_hex(res.buf+2,2);
	slot_dump(res.buf+2);

	if (*(uint16_t*)(res.buf ) != 0x83a0 || *(uint16_t*)(res.buf + 2) != 0x8330)
	{
		return -1;
	}

	atecc_send_recv(ATECC_CMD_READ,
			ATECC_RW_CONFIG, 24,NULL,0,
			buf,sizeof(buf), &res);

	u2f_print("-- key 0 --\r\n");
	dump_hex(res.buf,2);
	key_dump(res.buf);

	u2f_print("-- key 1 --\r\n");
	dump_hex(res.buf+2,2);
	key_dump(res.buf+2);

	if (*(uint16_t*)(res.buf ) != 0x3300 || *(uint16_t*)(res.buf+2) != 0x2f00)
	{
		return -1;
	}

	return 0;
}
#else
#define test_eeprom(x)
#endif

#ifdef TEST_KEY_SIGNING
int test_key_signing()
{
	struct atecc_response res;
	uint8_t buf[72];
	char msg[] = "wow a signature!\n";

	atecc_send_recv(ATECC_CMD_SHA,
			ATECC_SHA_START, 0, NULL, 0,
			buf, sizeof(buf), &res);

	atecc_send_recv(ATECC_CMD_SHA,
			ATECC_SHA_END, sizeof(msg)-1, msg, sizeof(msg)-1,
			buf, sizeof(buf), &res);

	dump_hex(res.buf, res.len);

	u2f_print("sig:\r\n");

	atecc_send_recv(ATECC_CMD_SIGN,
			ATECC_SIGN_EXTERNAL, 0, NULL, 0,
			buf, sizeof(buf), &res);
	dump_hex(res.buf, res.len);

	// lazy/bad check but eh
	return res.len > 8 ? 0 : -1;
}
#else
#define test_key_signing(x)
#endif

void run_tests()
{
	int rc;

#ifdef SHA_TEST
	PRINT("--- STARTING SHA TEST ---\r\n");
	rc = test_sha();
	if (rc == 0)
		PRINT("--- SHA TEST SUCCESS ---\r\n");
	else
		PRINT("--- SHA TEST FAILED %d ---\r\n",rc);
#endif

#ifdef TEST_EEPROM
	PRINT("--- STARTING EEPROM TEST ---\r\n");
	rc = test_eeprom();
	if (rc == 0)
		PRINT("--- EEPROM TEST SUCCESS ---\r\n");
	else
		PRINT("--- EEPROM TEST FAILED %d ---\r\n",rc);
#endif

#ifdef TEST_KEY_SIGNING
	PRINT("--- STARTING KEY SIGNING TEST ---\r\n");
	rc = test_key_signing();
	if (rc == 0)
		PRINT("--- KEY SIGNING SUCCESS ---\r\n");
	else
		PRINT("--- KEY SIGNING FAILED %d ---\r\n",rc);
#endif

}



#endif
