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
 * u2f.c
 * 		Cross platform implementation of U2F.  See API docs in u2f.h.
 *
 * 		U2F spec: https://fidoalliance.org/specs/fido-u2f-v1.0-nfc-bt-amendment-20150514/fido-u2f-raw-message-formats.html
 *
 */

#include "app.h"


#ifndef U2F_DISABLE

#include "bsp.h"
#include "u2f.h"


// void u2f_response_writeback(uint8_t * buf, uint8_t len);
static int16_t u2f_register(struct u2f_register_request * req);
static int16_t u2f_version();
static int16_t u2f_authenticate(struct u2f_authenticate_request * req, uint8_t control);

void u2f_request(struct u2f_request_apdu * req)
{
    uint16_t * rcode = (uint16_t *)req;
    uint32_t len = ((req->LC3) | ((uint32_t)req->LC2 << 8) | ((uint32_t)req->LC1 << 16));

    u2f_response_start();

    if (req->cla != 0)
    {
    	u2f_hid_set_len(2);
    	*rcode = U2F_SW_CLASS_NOT_SUPPORTED;
    	goto end;
    }

    switch(req->ins)
    {
        case U2F_REGISTER:
        	if (len != 64)
        	{
            	u2f_hid_set_len(2);
            	*rcode = U2F_SW_WRONG_LENGTH;
        	}
        	else
        	{
        		*rcode = u2f_register((struct u2f_register_request*)req->payload);
        	}
            break;
        case U2F_AUTHENTICATE:
        	 *rcode = u2f_authenticate((struct u2f_authenticate_request*)req->payload, req->p1);
        	break;
        case U2F_VERSION:
        	if (len)
        	{
            	u2f_hid_set_len(2);
            	*rcode = U2F_SW_WRONG_LENGTH;
        	}
        	else
        	{
        		*rcode = u2f_version();
        	}
        	break;
        case U2F_VENDOR_FIRST:
        case U2F_VENDOR_LAST:
        	*rcode = U2F_SW_NO_ERROR;
        	break;
        default:
        	u2f_hid_set_len(2);
        	*rcode = U2F_SW_INS_NOT_SUPPORTED;
        	break;
    }

    end:
    u2f_response_writeback((uint8_t*)rcode,2);
    u2f_response_flush();
}

static uint8_t get_signature_length(uint8_t * sig)
{
	return 0x46 + ((sig[32] & 0x80) == 0x80) + ((sig[0] & 0x80) == 0x80);
}

static void dump_signature_der(uint8_t * sig)
{
    uint8_t pad_s = (sig[32] & 0x80) == 0x80;
    uint8_t pad_r = (sig[0] & 0x80) == 0x80;
    uint8_t i[] = {0x30, 0x44};
    i[1] += (pad_s + pad_r);


    // DER encoded signature
    // write der sequence
    // has to be minimum distance and padded with 0x00 if MSB is a 1.
    u2f_response_writeback(i,2);
    i[1] = 0;

    // length of R value plus 0x00 pad if necessary
    u2f_response_writeback("\x02",1);
    i[0] = 0x20 + pad_r;
    u2f_response_writeback(i,1 + pad_r);

    // R value
    u2f_response_writeback(sig, 32);

    // length of S value plus 0x00 pad if necessary
    u2f_response_writeback("\x02",1);
    i[0] = 0x20 + pad_s;
    u2f_response_writeback(i,1 + pad_s);

    // S value
    u2f_response_writeback(sig+32, 32);
}


static int16_t u2f_authenticate(struct u2f_authenticate_request * req, uint8_t control)
{

	uint8_t up = 1;
	uint32_t count;

	if (control == U2F_AUTHENTICATE_CHECK)
	{
		u2f_hid_set_len(2);
		if (u2f_appid_eq(req->kh, req->app) == 0)
		{
			return U2F_SW_CONDITIONS_NOT_SATISFIED;
		}
		else
		{
			return U2F_SW_WRONG_DATA;
		}
	}
	if 	(
			control != U2F_AUTHENTICATE_SIGN ||
			req->khl != U2F_KEY_HANDLE_SIZE  ||
			u2f_appid_eq(req->kh, req->app) != 0 ||		// Order of checks is important
			u2f_load_key(req->kh, req->app) != 0

		)
	{
		u2f_hid_set_len(2);
		return U2F_SW_WRONG_PAYLOAD;
	}



	if (u2f_get_user_feedback())
	{
		u2f_hid_set_len(2);
		return U2F_SW_CONDITIONS_NOT_SATISFIED;
	}

	count = u2f_count();

    u2f_sha256_start();
    u2f_sha256_update(req->app,32);
    u2f_sha256_update(&up,1);
    u2f_sha256_update((uint8_t *)&count,4);
    u2f_sha256_update(req->chal,32);

    u2f_sha256_finish();

    if (u2f_ecdsa_sign((uint8_t*)req, req->kh, req->app) == -1)
	{
    	return U2F_SW_WRONG_DATA+0x20;
	}

    u2f_hid_set_len(7 + get_signature_length((uint8_t*)req));

    u2f_response_writeback(&up,1);
    u2f_response_writeback((uint8_t *)&count,4);
    dump_signature_der((uint8_t*)req);

	return U2F_SW_NO_ERROR;
}

static int16_t u2f_register(struct u2f_register_request * req)
{
    uint8_t i[] = {0x0,U2F_EC_FMT_UNCOMPRESSED};

    uint8_t key_handle[U2F_KEY_HANDLE_SIZE];
    uint8_t pubkey[64];


    const uint16_t attest_size = u2f_attestation_cert_size();

    if (u2f_get_user_feedback())
    {
    	u2f_hid_set_len(2);
        return U2F_SW_CONDITIONS_NOT_SATISFIED;
    }

    if ( u2f_new_keypair(key_handle, req->app, pubkey) == -1)
    {
    	u2f_hid_set_len(2);
    	return U2F_SW_INSUFFICIENT_MEMORY;
    }

    u2f_sha256_start();
    u2f_sha256_update(i,1);
    u2f_sha256_update(req->app,32);

    u2f_sha256_update(req->chal,32);

    u2f_sha256_update(key_handle,U2F_KEY_HANDLE_SIZE);
    u2f_sha256_update(i+1,1);
    u2f_sha256_update(pubkey,64);
    u2f_sha256_finish();
    
    if (u2f_ecdsa_sign((uint8_t*)req, U2F_ATTESTATION_HANDLE, req->app) == -1)
	{
    	return U2F_SW_WRONG_DATA;
	}

    u2f_hid_set_len(69 + get_signature_length((uint8_t*)req) + U2F_KEY_HANDLE_SIZE + u2f_attestation_cert_size());
    i[0] = 0x5;
    u2f_response_writeback(i,2);
    u2f_response_writeback(pubkey,64);
    i[0] = U2F_KEY_HANDLE_SIZE;
    u2f_response_writeback(i,1);
    u2f_response_writeback(key_handle,U2F_KEY_HANDLE_SIZE);

    u2f_response_writeback(u2f_get_attestation_cert(),u2f_attestation_cert_size());

    dump_signature_der((uint8_t*)req);


    return U2F_SW_NO_ERROR;
}

static int16_t u2f_version()
{
	code const char version[] = "U2F_V2";
	u2f_hid_set_len(2 + sizeof(version)-1);
	u2f_response_writeback(version, sizeof(version)-1);
	return U2F_SW_NO_ERROR;
}

#endif
