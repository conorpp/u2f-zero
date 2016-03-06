/*
 * app.h
 *
 */

#ifndef APP_H_
#define APP_H_

#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>

// debug options
#define U2F_PRINT


// application settings
#define U2F_ATTESTATION_KEY_SLOT	15
//#define ATECC_SETUP_DEVICE

// efm8ub1 application eeprom memory mappings
#define U2F_KEY_HEADER_ADDR		0xF800
#define U2F_EEPROM_CONFIG		(0xF800 + 128)


typedef enum
{
	APP_NOTHING = 0,
	APP_WINK,
	_APP_WINK,
	APP_ERROR,
}
APP_STATE;

typedef enum
{
	ERROR_NOTHING = 0,
	ERROR_I2C_ERRORS_EXCEEDED = 2,
	ERROR_READ_TRUNCATED = 6,
	ERROR_ATECC_VERIFY = 0x01,
	ERROR_ATECC_PARSE = 0x03,
	ERROR_ATECC_FAULT = 0x05,
	ERROR_ATECC_EXECUTION = 0x0f,
	ERROR_ATECC_WAKE = 0x11,
	ERROR_ATECC_WATCHDOG = 0xee,
	ERROR_ATECC_CRC = 0xff,
	ERROR_SEQ_EXCEEDED = 0x12,
	ERROR_BAD_KEY_STORE = 0x13,
}
APP_ERROR_CODE;

struct APP_DATA
{
	// must be at least 37 bytes
	uint8_t tmp[70];
	uint8_t state;
	uint8_t error;
};

extern uint8_t hidmsgbuf[64];

extern data struct APP_DATA appdata;


void set_app_error(APP_ERROR_CODE ec);

// should be called after initializing eeprom
void u2f_init();

#ifdef ATECC_SETUP_DEVICE

void atecc_setup_device(uint8_t * buf);
#define U2F_HID_DISABLE
#define U2F_DISABLE
#define u2f_init(x)
#define u2f_hid_init(x)

#else

#define atecc_setup_device(x)

#endif


#endif /* APP_H_ */
