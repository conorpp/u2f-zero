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

typedef enum
{
	EP_BUSY = 0,
	EP_FREE,
}
ENDPOINT_STATE;


struct APP_DATA
{
	ENDPOINT_STATE EP1_state;
	uint8_t hidmsgbuf[64];

	// ms
	uint16_t usb_read_timeout;
};


SI_SBIT(LED_R, SFR_P0, 6);
SI_SBIT(LED_G, SFR_P0, 5);
SI_SBIT(LED_B, SFR_P0, 4);

void Delay(int16_t ms);

extern uint16_t _MS_;

#define get_ms() _MS_

extern SI_SEGMENT_VARIABLE(myUsbDevice, USBD_Device_TypeDef, MEM_MODEL_SEG);

#define GetEp(epAddr)  (&myUsbDevice.ep0 + epAddr)

#endif /* BSP_H_ */
