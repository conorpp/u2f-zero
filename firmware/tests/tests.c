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
#define TEST_SHA

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



void run_tests()
{
	int rc;

	PRINT("--- STARTING SHA TEST ---\r\n");
	rc = test_sha();
	if (rc == 0)
		PRINT("--- SHA TEST SUCCESS ---\r\n");
	else
		PRINT("--- SHA TEST FAILED %d ---\r\n",rc);
}



#endif
