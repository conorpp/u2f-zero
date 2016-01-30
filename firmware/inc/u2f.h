/*
 * u2f.h
 *
 *  Created on: Jan 26, 2016
 *      Author: pp
 */

#ifndef _U2F_H_
#define _U2F_H_

#include <stdint.h>

struct u2f_message
{
	uint8_t hihuht[8];
	int wat;
};


int u2f_request(struct u2f_message* req, struct u2f_message* res);

#endif /* U2F_H_ */
