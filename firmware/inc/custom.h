/*
 * custom.h
 *
 *  Created on: Mar 30, 2016
 *      Author: pp
 */

#ifndef CUSTOM_H_
#define CUSTOM_H_

#include "app.h"
#include "u2f_hid.h"

#define U2F_CUSTOM_GET_RNG		0x21
#define U2F_CUSTOM_SEED_RNG		0x22
#define U2F_CUSTOM_WIPE_KEYS	0x23
#define U2F_CUSTOM_PULSE		0x24
#define U2F_CUSTOM_IDLE_COLOR	0x25
#define U2F_CUSTOM_IDLE_COLORP	0x26


uint8_t custom_command(struct u2f_hid_msg * msg);


#endif
