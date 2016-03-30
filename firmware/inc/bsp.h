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

SI_SBIT(U2F_BUTTON, SFR_P0, 2);
SI_SBIT(U2F_BUTTON_VAL, SFR_P0, 3);

#define U2F_BUTTON_IS_PRESSED() (U2F_BUTTON == 0)

// Set brightness via PWM
#define LED_B(x)\
	PCA0CPL0 = ((((uint8_t)(x))) << PCA0CPL0_PCA0CPL0__SHIFT);\
	PCA0CPH0 = ((((uint8_t)(x))) << PCA0CPH0_PCA0CPH0__SHIFT)

#define LED_G(x) \
	PCA0CPL1 = ((((uint8_t)(x))) << PCA0CPL1_PCA0CPL1__SHIFT); \
	PCA0CPH1 = ((((uint8_t)(x))) << PCA0CPH1_PCA0CPH1__SHIFT)

#define LED_R(x)\
	PCA0CPL2 = ((((uint8_t)(x))) << PCA0CPL2_PCA0CPL2__SHIFT);\
	PCA0CPH2 = ((((uint8_t)(x))) << PCA0CPH2_PCA0CPH2__SHIFT)

#define watchdog()	(WDTCN = 0xA5)


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
