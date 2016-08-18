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

 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#ifndef APP_H_
#define APP_H_

#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>
#include "u2f_hid.h"



// application settings
#define U2F_ATTESTATION_KEY_SLOT	15
//#define ATECC_SETUP_DEVICE
//#define U2F_PRINT
//#define U2F_BLINK_ERRORS

// efm8ub1 application eeprom memory mappings
#define U2F_KEY_HEADER_ADDR		0xF800
#define U2F_EEPROM_CONFIG		(U2F_KEY_HEADER_ADDR + 128)
#define U2F_EEPROM_APP_IDS		(U2F_EEPROM_CONFIG + 64)

#if ((U2F_ATTESTATION_KEY_SLOT * 32) > 768)
#error "not enough eeprom"
#endif

//								{blue(0), green(0x5a), red(0)}
#define U2F_DEFAULT_BRIGHTNESS					90
#define U2F_COLOR 								0x001300
#define U2F_DEFAULT_COLOR_PRIME 				0x130000
#define U2F_DEFAULT_COLOR_ERROR 				0x000038
#define U2F_DEFAULT_COLOR_INPUT 				0x000603
#define U2F_DEFAULT_COLOR_INPUT_SUCCESS			0x251200
#define U2F_COLOR_WINK 							0x120000
#define U2F_DEFAULT_COLOR_WINK_OUT_OF_SPACE 	0x030312

#define U2F_DEFAULT_COLOR_PERIOD				20

typedef enum
{
	APP_NOTHING = 0,
	APP_HID_MSG,
	APP_WINK,
	_APP_WINK,
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
	ERROR_I2C_CRC = 0x15,
	ERROR_SEQ_EXCEEDED = 0x12,
	ERROR_BAD_KEY_STORE = 0x13,
	ERROR_USB_WRITE = 0x14,
	ERROR_I2C_BAD_LEN = 0x16,
	ERROR_HID_BUFFER_FULL = 0x17,
	ERROR_HID_INVALID_CMD = 0x18,
	ERROR_DAMN_WATCHDOG = 0x19,
	ERROR_OUT_OF_CIDS = 0x20,
	ERROR_I2C_RESTART = 0x21,
}
APP_ERROR_CODE;

struct APP_DATA
{
	// must be at least 70 bytes
	uint8_t tmp[70];
};

#define U2F_CONFIG_GET_SERIAL_NUM		0x80
#define	U2F_CONFIG_IS_BUILD				0x81
#define U2F_CONFIG_IS_CONFIGURED		0x82
#define U2F_CONFIG_LOCK					0x83
#define U2F_CONFIG_GENKEY				0x84

struct config_msg
{
	uint8_t cmd;
	uint8_t buf[HID_PACKET_SIZE-1];
};



extern uint8_t hidmsgbuf[64];

extern data struct APP_DATA appdata;

void set_app_u2f_hid_msg(struct u2f_hid_msg * msg );

void set_app_error(APP_ERROR_CODE ec);

uint8_t get_app_error();

uint8_t get_app_state();

void set_app_state(APP_STATE s);

void rgb(uint8_t r, uint8_t g, uint8_t b);
#define rgb_hex(c) (rgb((c)&0xff,((c)>>8)&0xff,((c)>>16)&0xff))

void app_wink(uint32_t color);

// should be called after initializing eeprom
void u2f_init();


#ifdef ATECC_SETUP_DEVICE

#include "atecc508a.h"

void atecc_setup_device(struct config_msg * msg);
void atecc_setup_init(uint8_t * buf);

void u2f_config_request();

#define U2F_HID_DISABLE
#define U2F_DISABLE
#define u2f_init(x)
#define u2f_hid_init(x)
#define u2f_hid_request(x)	atecc_setup_device((struct config_msg*)x)
#define u2f_hid_set_len(x)
#define u2f_hid_flush(x)
#define u2f_hid_writeback(x)
#define u2f_hid_check_timeouts(x)
#define u2f_wipe_keys(x)	1

#else

int8_t u2f_wipe_keys();
#define atecc_setup_device(x)
#define atecc_setup_init(x)
#endif


#endif /* APP_H_ */
