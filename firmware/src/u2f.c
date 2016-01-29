/*
 * u2f.c
 *
 *  Created on: Jan 26, 2016
 *      Author: pp
 */

#include "u2f.h"
#include "bsp.h"

int u2f_request(struct u2f_message* req, struct u2f_message* res)
{
	u2f_print("U2F REQUEST\r\n");
	return 0;
}
