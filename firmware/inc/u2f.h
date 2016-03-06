/*
 * u2f.h
 *
 *  Created on: Jan 26, 2016
 *      Author: pp
 */

#ifndef _U2F_H_
#define _U2F_H_

#include <stdint.h>

#define SW_NO_ERROR 						0x00
#define SW_CONDITIONS_NOT_SATISFIED 		0x01
#define SW_WRONG_DATA 						0x02

#define U2F_EC_FMT_UNCOMPRESSED				0x04

#define U2F_EC_POINT_SIZE					32
#define U2F_EC_PUBKEY_SIZE					65
#define U2F_APDU_SIZE                       7
#define U2F_CHALLENGE_SIZE                  32
#define U2F_APPLICATION_SIZE                32
#define U2F_KEY_HANDLE_SIZE                 4
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


void u2f_request(struct u2f_request_apdu* req);

// Command status responses
#define U2F_SW_NO_ERROR                     0x9000
#define U2F_SW_WRONG_DATA                   0x6984
#define U2F_SW_CONDITIONS_NOT_SATISFIED     0x6985
#define U2F_SW_INS_NOT_SUPPORTED            0x6d00

/* IMPLEMENTATION specific functions that must be implemented by user */

// Key id/handle for the private key for attestation
#define U2F_ATTESTATION_HANDLE              ((uint8_t *)"\x00\x00\x00\x00")

// return size of the DER formatted attestation certificate
uint16_t u2f_attestation_cert_size();

// callback for u2f to send back response data
//  @buf data to write back
//  @len length of buf in bytes
void u2f_response_writeback(uint8_t * buf, uint16_t len);

// callback when u2f finishes and will
// indicate when all buffer data, if any, should be written
void u2f_response_flush();

// callback when u2f starts a new transaction
void u2f_response_start();

// Return 0 if user provides feedback, -1 if not
// This should block as long as it needs to get feedback
// before failing.
int8_t u2f_get_user_feedback();

// callback for u2f to start a sha256 hash
void u2f_sha256_start();

// callback for u2f to add data to started sha256 state
//  @buf data to update hash with
//  @len length of buf in bytes
void u2f_sha256_update(uint8_t * buf, uint8_t len);

// callback for u2f to havest hash from
//  @buf final data to update hash with
//  @len length of buf in bytes
void u2f_sha256_finish();


// callback for u2f to compute signature on the previously computed sha256 digest
//  @dest atleast 64 bytes to write back signature R and S values
//  @handle for the private key to use
//  @return -1 for failure, 0 for success
int8_t u2f_ecdsa_sign(uint8_t * dest, uint8_t * handle);

// callback to get a new key handle
//  @handle location to write the key handle (should be U2F_KEY_HANDLE_SIZE bytes long)
//  @pubkey location to write the public key R & S (64 bytes)
//  @return -1 for failure, 0 for success
int8_t u2f_new_keypair(uint8_t * handle, uint8_t * pubkey);

// method to return pointer to attestation cert
uint8_t * u2f_get_attestation_cert();

// method to set the total length of the response for use by underlying layer
// @len the length of U2F response in bytes
void set_response_length(uint16_t len);

#endif /* U2F_H_ */
