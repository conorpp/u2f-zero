/*
 * u2f.c
 *
 *  Created on: Jan 26, 2016
 *      Author: pp
 */

#include "u2f.h"
#include "bsp.h"

static uint8_t count = 1;

int u2f_request(struct u2f_message* req, struct u2f_message* res)
{
	u2f_print("U2F REQUEST %bd\r\n", count++);
	return 0;
}
