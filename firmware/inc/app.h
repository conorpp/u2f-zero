/*
 * app.h
 *
 */

#ifndef APP_H_
#define APP_H_

#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>

SI_SBIT (SDA, SFR_P1, 2);              // SMBus on P1.2
SI_SBIT(SCL, SFR_P1, 3);               // and P1.3

SI_SBIT (LED1, SFR_P1, 4);             // LED green

// debug options
#define U2F_PRINT

#define KEYHANDLES_START 			(EEPROM_DATA_START + 30)
#define KEYHANDLES_COUNT			14

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
}
APP_ERROR_CODE;

struct APP_DATA
{
	uint8_t hidmsgbuf[64];
	uint8_t tmp[10];
	uint8_t state;
	uint8_t error;
};

extern data struct APP_DATA appdata;


void set_app_error(APP_ERROR_CODE ec);




#endif /* APP_H_ */
