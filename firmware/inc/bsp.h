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

#define _EFMSDK16
#ifdef _EFMSDK16
SI_SBIT(LED_R, SFR_P1, 6);
SI_SBIT(LED_B, SFR_P1, 5);
SI_SBIT(LED_G, SFR_P1, 4);
#endif


void u2f_delay(int16_t ms);

void u2f_write_s(char* d);
void u2f_write_n(char* buf, uint32_t val, int base);

void qu2f_write_s(char* d);
void qu2f_write_n(char* buf, uint32_t val, int base);


void usb_write(uint8_t* buf, uint8_t len);



#ifdef U2F_PRINT

	void u2f_print(char* fmt, ...);

	// not reentrant
	void dump_hex(uint8_t* hex, uint8_t len);
	void flush_messages();

#else
	#define u2f_print(x)
	#define dump_hex(x)
	#define flush_messages(x)
#endif




#endif /* BSP_H_ */
