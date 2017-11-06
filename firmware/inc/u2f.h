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
 * U2F spec: https://fidoalliance.org/specs/fido-u2f-v1.0-nfc-bt-amendment-20150514/fido-u2f-raw-message-formats.html
 */

#ifndef _U2F_H_
#define _U2F_H_

#include <stdint.h>

#define U2F_EC_FMT_UNCOMPRESSED				0x04

#define U2F_EC_POINT_SIZE					32
#define U2F_EC_PUBKEY_SIZE					65
#define U2F_APDU_SIZE                       7
#define U2F_CHALLENGE_SIZE                  32
#define U2F_APPLICATION_SIZE                32
#define U2F_KEY_HANDLE_ID_SIZE              8
#define U2F_KEY_HANDLE_KEY_SIZE             36
#define U2F_KEY_HANDLE_SIZE                 (U2F_KEY_HANDLE_KEY_SIZE+U2F_KEY_HANDLE_ID_SIZE)
#define U2F_REGISTER_REQUEST_SIZE           (U2F_CHALLENGE_SIZE+U2F_APPLICATION_SIZE)
#define U2F_MAX_REQUEST_PAYLOAD             (1 + U2F_CHALLENGE_SIZE+U2F_APPLICATION_SIZE + 1 + U2F_KEY_HANDLE_SIZE)


// U2F native commands
#define U2F_REGISTER 						0x01
#define U2F_AUTHENTICATE 					0x02
#define U2F_VERSION 						0x03
#define U2F_VENDOR_FIRST 					0xc0
#define U2F_VENDOR_LAST 					0xff

// U2F_CMD_REGISTER command defines
#define U2F_REGISTER_ID 					0x05
#define U2F_REGISTER_HASH_ID 				0x00

// U2F Authenticate
#define U2F_AUTHENTICATE_CHECK				0x7
#define U2F_AUTHENTICATE_SIGN				0x3


// Command status responses
#define U2F_SW_NO_ERROR                     0x9000
#define U2F_SW_WRONG_DATA                   0x6984
#define U2F_SW_CONDITIONS_NOT_SATISFIED     0x6985
#define U2F_SW_INS_NOT_SUPPORTED            0x6d00
#define U2F_SW_WRONG_LENGTH            		0x6700
#define U2F_SW_CLASS_NOT_SUPPORTED          0x6E00
#define U2F_SW_WRONG_PAYLOAD	            0x6a80
#define U2F_SW_INSUFFICIENT_MEMORY          0x9210

// Delay in milliseconds to wait for user input
#define U2F_MS_USER_INPUT_WAIT				3000

struct u2f_request_apdu
{
    uint8_t cla;
    uint8_t ins;
    uint8_t p1;
    uint8_t p2;
    uint8_t LC1;
    uint8_t LC2;
    uint8_t LC3;
    uint8_t payload[U2F_MAX_REQUEST_PAYLOAD];
};

struct u2f_ec_point
{
    uint8_t fmt;
    uint8_t x[U2F_EC_POINT_SIZE];
    uint8_t y[U2F_EC_POINT_SIZE];
};

struct u2f_register_request
{
    uint8_t chal[U2F_CHALLENGE_SIZE];
    uint8_t app[U2F_APPLICATION_SIZE];
};

struct u2f_authenticate_request
{
    uint8_t chal[U2F_CHALLENGE_SIZE];
    uint8_t app[U2F_APPLICATION_SIZE];
    uint8_t khl;
    uint8_t kh[U2F_KEY_HANDLE_SIZE];
} ;

// u2f_request send a U2F message to U2F protocol
//  @req U2F message
void u2f_request(struct u2f_request_apdu* req);



//////////////////////////////////////////////////////////////////
/* Platform specific functions that must be implemented by user */
//////////////////////////////////////////////////////////////////

// Key id/handle for the private key for attestation
#define U2F_ATTESTATION_HANDLE              NULL

// u2f_attestation_cert_size return size of the DER formatted attestation certificate
extern uint16_t u2f_attestation_cert_size();

// u2f_response_writeback callback for u2f to send back response data
//  @buf data to write back
//  @len length of buf in bytes
extern void u2f_response_writeback(uint8_t * buf, uint16_t len);

// u2f_response_flush callback when u2f finishes and will
// indicate when all buffer data, if any, should be written
extern void u2f_response_flush();

// u2f_response_start callback when u2f starts a new transaction
extern void u2f_response_start();

// u2f_get_user_feedback return 0 if user provides feedback, -1 if not
// This should block as long as it needs to get feedback
// before failing.
extern int8_t u2f_get_user_feedback();

// u2f_sha256_start callback for u2f to start a sha256 hash
extern void u2f_sha256_start();

// u2f_sha256_update callback for u2f to add data to started sha256 state
//  @buf data to update hash with
//  @len length of buf in bytes
extern void u2f_sha256_update(uint8_t * buf, uint8_t len);

// u2f_sha256_finish callback for u2f to havest hash from
//  @buf final data to update hash with
//  @len length of buf in bytes
extern void u2f_sha256_finish();


// u2f_ecdsa_sign callback for u2f to compute signature on the previously computed sha256 digest
//  @dest atleast 64 bytes to write back signature R and S values
//  @handle for the private key to use
//  @return -1 for failure, 0 for success
extern int8_t u2f_ecdsa_sign(uint8_t * dest, uint8_t * handle, uint8_t * appid);


// u2f_new_keypair callback to get a new key handle
//  @handle location to write the key handle (should be U2F_KEY_HANDLE_SIZE bytes long)
//  @appid the new application ID to associate with key pair. 32 bytes.
//  @pubkey location to write the public key R & S (64 bytes)
//  @return -1 for failure, 0 for success
int8_t u2f_new_keypair(uint8_t * handle, uint8_t * appid, uint8_t * pubkey);

// u2f_appid_eq must check if app id is equal to app id associated with key pair handle
/// @handle the handle for the key pair
/// @appid the 32 byte app id to check against
/// @return non-zero not equal, 0 equal
int8_t u2f_appid_eq(uint8_t * handle, uint8_t * appid);

// u2f_load_key Load a key into memory or check to see that the handle exists
//  @handle the key handle to check
//	@return -1 if it doesn't exist, 0 for success
extern int8_t u2f_load_key(uint8_t * handle, uint8_t * appid);

// u2f_get_attestation_cert method to return pointer to attestation cert
extern uint8_t * u2f_get_attestation_cert();

// u2f_count Should increment a 4 byte persistent/atomic counter and return it.
// 	@return the counter
extern uint32_t u2f_count();

// set_response_length method to set the total length of the response for use by underlying layer
//  @len the length of U2F response in bytes
extern void set_response_length(uint16_t len);

#endif /* U2F_H_ */
