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

 *
 *
 * U2F HID spec: https://fidoalliance.org/specs/fido-u2f-v1.0-nfc-bt-amendment-20150514/fido-u2f-hid-protocol.html
 *
 */

#ifndef U2F_HID_H_
#define U2F_HID_H_

#include <stdint.h>
#include "descriptors.h"

#define TYPE_MASK               0x80	// Frame type mask
#define TYPE_INIT               0x80	// Initial frame identifier
#define TYPE_CONT               0x00	// Continuation frame identifier

#define U2FHID_PING         (TYPE_INIT | 0x01)	// Echo data through local processor only
#define U2FHID_MSG          (TYPE_INIT | 0x03)	// Send U2F message frame
#define U2FHID_LOCK         (TYPE_INIT | 0x04)	// Send lock channel command
#define U2FHID_INIT         (TYPE_INIT | 0x06)	// Channel initialization
#define U2FHID_WINK         (TYPE_INIT | 0x08)	// Send device identification wink
#define U2FHID_ERROR        (TYPE_INIT | 0x3f)	// Error response

#define U2FHID_VENDOR_FIRST (TYPE_INIT | 0x40)	// First vendor defined command
#define U2FHID_VENDOR_LAST  (TYPE_INIT | 0x7f)	// Last vendor defined command

#define ERR_NONE                0x00    // No error
#define ERR_INVALID_CMD         0x01    // Invalid command
#define ERR_INVALID_PAR         0x02    // Invalid parameter
#define ERR_INVALID_LEN         0x03    // Invalid message length
#define ERR_INVALID_SEQ         0x04    // Invalid message sequencing
#define ERR_MSG_TIMEOUT         0x05    // Message has timed out
#define ERR_CHANNEL_BUSY        0x06    // Channel busy
#define ERR_LOCK_REQUIRED       0x0a    // Command requires channel lock
#define ERR_SYNC_FAIL           0x0b    // SYNC command failed
#define ERR_OTHER               0x7f    // Other unspecified error

#define CAPABILITY_WINK  		0x01
#define CAPABILITY_LOCK  		0x02

#define U2FHID_BROADCAST 		0xffffffff

#define U2FHID_INIT_PAYLOAD_SIZE  (HID_PACKET_SIZE-7)
#define U2FHID_CONT_PAYLOAD_SIZE  (HID_PACKET_SIZE-5)
#define U2FHID_MAX_PAYLOAD_SIZE  (7609)

#define U2FHID_LEN(req) (*(uint16_t*)&req->pkt.init.bcnth)
#define U2FHID_SET_LEN(req,len) (*(uint16_t*)&req->pkt.init.bcnth = (uint16_t)len)

#define U2FHID_TIMEOUT_MS 5000
#define U2FHID_TIMEOUT(hid) (get_ms() - (hid)->last_buffered > U2FHID_TIMEOUT_MS)

struct u2f_hid_msg
{
	uint32_t cid;
	union{
		struct{
			uint8_t cmd;
			uint8_t bcnth;		// length high
			uint8_t bcntl;		// length low
			uint8_t payload[U2FHID_INIT_PAYLOAD_SIZE];
		} init;
		struct{
			uint8_t seq;
			uint8_t payload[U2FHID_CONT_PAYLOAD_SIZE];
		} cont;
	} pkt;
};

struct u2f_hid_nonce
{
	uint8_t nonce[8];
};

struct u2f_hid_init_response
{
	// struct u2f_hid_nonce nonce;
	uint32_t cid;
	uint8_t version_id;
	uint8_t version_major;
	uint8_t version_minor;
	uint8_t version_build;
	uint8_t cflags;
};

struct CID
{
	uint32_t cid;
	uint32_t last_used;
	uint8_t busy;
	uint8_t last_cmd;
};

typedef enum
{
	U2FHID_REPLY=0,
	U2FHID_WAIT,
	U2FHID_INCOMPLETE,
	U2FHID_FAIL,
} U2FHID_STATUS;


void u2f_hid_init();


// set the length of the total payload to be sent
//  @len length of payload in bytes
void u2f_hid_set_len(uint16_t len);

// u2f_hid_writeback handles the sequencing and per packet buffering
//  @payload the buffer to write
//  @len length of buffer
//  @prereq is that hid_layer.current_cid, hid_layer.res_len each set to correct values
void u2f_hid_writeback(uint8_t * payload, uint16_t len);

// u2f_hid_flush flush any remaining data that may be buffered.
void u2f_hid_flush();

// u2f_hid_request entry function for U2F HID protocol.
// It will pass up to U2F protocol if necessary.
//  @param req the U2F HID message
void u2f_hid_request(struct u2f_hid_msg* req);

struct CID* get_cid(uint32_t cid);

// app_wink blink a light on the platform
// must be implemented elsewhere for specific platform used
//  @color optional hex color
extern void app_wink(uint32_t color);


// set_app_error set global error value
// must be implemented elsewhere for specific platform used
//  @ec error values defined in app.h
extern void set_app_error(uint8_t ec);

// Call from main loop to ensure stale channels get timeout error.
void u2f_hid_check_timeouts();
void u2f_print_hid_check_timeouts();

#define U2FHID_IS_INIT(cmd)			((cmd) & 0x80)


#endif /* U2F_HID_H_ */
