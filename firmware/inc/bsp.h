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

SI_SBIT(U2F_BUTTON, SFR_P1, 5);
SI_SBIT(U2F_BUTTON_VAL, SFR_P1, 6);

#define U2F_BUTTON_IS_PRESSED() (U2F_BUTTON == 0)

SI_SBIT(U2F_RED, SFR_P1, 1);
SI_SBIT(U2F_GREEN, SFR_P1, 0);
SI_SBIT(U2F_BLUE, SFR_P0, 7);


// Set brightness via PWM
#define LED_B(x)\
	PCA0CPL0 = ((((uint8_t)(x))) << PCA0CPL0_PCA0CPL0__SHIFT);\
	PCA0CPH0 = ((((uint8_t)(x))) << PCA0CPH0_PCA0CPH0__SHIFT)

#define LED_G(x)\
	PCA0CPL1 = ((((uint8_t)(x))) << PCA0CPL1_PCA0CPL1__SHIFT); \
	PCA0CPH1 = ((((uint8_t)(x))) << PCA0CPH1_PCA0CPH1__SHIFT)

#define LED_R(x)\
	PCA0CPL2 = ((((uint8_t)(x))) << PCA0CPL2_PCA0CPL2__SHIFT);\
	PCA0CPH2 = ((((uint8_t)(x))) << PCA0CPH2_PCA0CPH2__SHIFT)

#define watchdog()	(WDTCN = 0xA5)

#define reboot()	(RSTSRC = 1 << 4)

void u2f_delay(uint32_t ms);

void usb_write(uint8_t* buf, uint8_t len);



#ifdef U2F_PRINT

	void dump_hex(uint8_t* hex, uint8_t len);

	void u2f_putd(uint32_t i);
	void u2f_putx(uint32_t i);

#define u2f_putb(x)	u2f_putx((uint8_t) (x))
#define u2f_putl(x)	u2f_putd((uint32_t) (x))
#define u2f_putlx(x)	u2f_putx((uint32_t) (x))

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
