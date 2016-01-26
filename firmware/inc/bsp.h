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

extern uint16_t _MS_;
extern SI_SEGMENT_VARIABLE(myUsbDevice, USBD_Device_TypeDef, MEM_MODEL_SEG);

#define get_ms() _MS_

#define GetEp(epAddr)  (&myUsbDevice.ep0 + epAddr)

#define _EFMSDK16
#ifdef _EFMSDK16
SI_SBIT(LED_R, SFR_P1, 6);
SI_SBIT(LED_B, SFR_P1, 5);
SI_SBIT(LED_G, SFR_P1, 4);
#endif

void print_u2f(const char* fmt, ...);


void Delay(int16_t ms);

void write_s_tx(char* d);



#endif /* BSP_H_ */
