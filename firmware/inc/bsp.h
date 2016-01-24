/*
 * bsp.h
 *
 *  Created on: Jan 22, 2016
 *      Author: pp
 */

#ifndef BSP_H_
#define BSP_H_

SI_SBIT(LED_R, SFR_P0, 6);
SI_SBIT(LED_G, SFR_P0, 5);
SI_SBIT(LED_B, SFR_P0, 4);

void Delay(int16_t ms);

#endif /* BSP_H_ */
