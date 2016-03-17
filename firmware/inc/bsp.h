/*
 * bsp.h
 *
 *  Created on: Jan 22, 2016
 *      Author: pp
 */

#ifndef BSP_H_
#define BSP_H_

#include <SI_EFM8UB1_Register_Enums.h>
#include <efm8_usb.h>
#include <stdint.h>
#include "descriptors.h"
#include "app.h"


extern data uint32_t _MS_;
extern SI_SEGMENT_VARIABLE(myUsbDevice, USBD_Device_TypeDef, MEM_MODEL_SEG);

#define get_ms() _MS_

#define GetEp(epAddr)  (&myUsbDevice.ep0 + epAddr)

SI_SBIT(LED_G, SFR_P0, 0);
SI_SBIT(LED_R, SFR_P0, 1);
SI_SBIT(LED_B, SFR_P0, 2);


void u2f_delay(int16_t ms);

void usb_write(uint8_t* buf, uint8_t len);



#ifdef U2F_PRINT

	void dump_hex(uint8_t* hex, uint8_t len);
	void u2f_putb(uint8_t i);
	void u2f_putd(int16_t i);
	void u2f_putx(uint16_t i);
	void u2f_putl(int32_t i);
	void u2f_putlx(int32_t i);

	void u2f_prints(const char * str);
	void u2f_printb(const char * tag, uint8_t c, ...);
	void u2f_printd(const char * tag, uint8_t c, ...);
	void u2f_printx(const char * tag, uint8_t c, ...);
	void u2f_printl(const char * tag, uint8_t c, ...);
	void u2f_printlx(const char * tag, uint8_t c, ...);

#else

	#define u2f_printx(x)
	#define u2f_printb(x)
	#define u2f_printlx(x)
	#define u2f_printl(x)
	#define u2f_printd(x)
	#define u2f_prints(x)

	#define u2f_putx(x)
	#define u2f_putb(x)
	#define u2f_putl(x)
	#define u2f_putlx(x)

	#define putf(x)
	#define dump_hex(x)

#endif




#endif /* BSP_H_ */
