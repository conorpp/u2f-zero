/*
 * bsp.c
 *
 */

#include "bsp.h"


void Delay(uint16_t ms) {
	int16_t x;
	int16_t y;
	for (y = 0; y < ms; y++) {
		for (x = 0; x < 1000; x) {
			x++;
		}
	}
}
