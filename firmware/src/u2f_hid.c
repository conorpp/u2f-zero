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

 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 *
 * u2f_hid.c
 *
 * 		U2F HID layer.  Implemented to be platform independent.  See API docs in u2f_hid.h.
 * 		Makes calls to U2F layer, implemented in u2f.c.
 *
 * 		U2F HID spec: https://fidoalliance.org/specs/fido-u2f-v1.0-nfc-bt-amendment-20150514/fido-u2f-hid-protocol.html
 *
 */
#include "app.h"

#include <stdint.h>
#include <string.h>

#include "bsp.h"
#include "u2f_hid.h"
#include "u2f.h"

#ifndef U2F_HID_DISABLE

#define CID_MAX (sizeof(CIDS)/sizeof(struct CID))
#define BROADCAST_CID (CIDS[CID_MAX-1])


typedef enum
{
	HID_BUSY=0,
	HID_READY,
} HID_STATE;

struct CID
{
	uint32_t cid;
	uint32_t last_used;
	uint8_t busy;
	uint8_t last_cmd;
};

static struct hid_layer_param
{
	HID_STATE state;
	uint32_t current_cid;
	uint8_t current_cmd;

	uint32_t last_buffered;
	uint16_t bytes_buffered;
	uint16_t req_len;

	// number of payload bytes written in response
	uint16_t bytes_written;


	// total length of response in bytes
	uint16_t res_len;

	#define BUFFER_SIZE 270
	uint8_t buffer[BUFFER_SIZE];

} hid_layer;

uint32_t _hid_lockt = 0;
uint32_t _hid_lock_cid = 0;

static struct CID CIDS[5];

static uint8_t CID_NUM = 0;

static uint8_t _hid_pkt[HID_PACKET_SIZE];
static uint8_t _hid_offset = 0;
static uint16_t _hid_seq = 0;
static uint8_t _hid_in_session = 0;

#define u2f_hid_busy() (_hid_in_session)

#define MIN(a,b) ((a) < (b) ? (a):(b))

#define hid_is_locked()		(_hid_lockt > get_ms())
#define hid_is_lock_cid(c)	((c) == _hid_lock_cid)

void u2f_hid_init()
{
	memset(CIDS, 0, sizeof(CIDS));
	memset(&hid_layer, 0, sizeof(hid_layer));
	CID_NUM = 0;
	_hid_offset = 0;
	_hid_seq = 0;
	_hid_in_session = 0;
}

void u2f_hid_set_len(uint16_t len)
{
	hid_layer.res_len = len;
}


static void u2f_hid_reset_packet()
{
	_hid_seq = 0;
	_hid_offset = 0;
	_hid_in_session = 0;
	memset(&hid_layer, 0, sizeof(hid_layer));
	memset(_hid_pkt, 0, HID_PACKET_SIZE);
}

// writes what has been buffered and clears memory
void u2f_hid_flush()
{
	if (_hid_offset)
	{
		usb_write(_hid_pkt, HID_PACKET_SIZE);
	}
	u2f_hid_reset_packet();
}

// Buffers data to a 64 byte buffer before writing it while
// handling U2F HID sequencing
void u2f_hid_writeback(uint8_t * payload, uint16_t len)
{
	struct u2f_hid_msg * r = (struct u2f_hid_response *) _hid_pkt;

	_hid_in_session = 1;


	do
	{

		if (_hid_offset == 0)
		{
			r->cid = hid_layer.current_cid;
			if (!_hid_seq)
			{
				r->pkt.init.cmd = hid_layer.current_cmd;
				U2FHID_SET_LEN(r, hid_layer.res_len);
				_hid_offset = 7;
			}
			else
			{
				r->pkt.cont.seq = (uint8_t)_hid_seq - 1;
				_hid_offset = 5;
				if (_hid_seq-1 > 127)
				{
					set_app_error(ERROR_SEQ_EXCEEDED);
					return;
				}
			}
		}

		_hid_pkt[_hid_offset++] = *payload++;
		hid_layer.bytes_written++;
		if (_hid_offset == HID_PACKET_SIZE)
		{
			_hid_offset = 0;
			_hid_seq++;

			usb_write(_hid_pkt, HID_PACKET_SIZE);
			memset(_hid_pkt, 0, HID_PACKET_SIZE);
		}
		if (!len) break;
	}
	while(--len);

}



static void refresh_cid(struct CID* c)
{
	c->last_used = get_ms();
}


static uint32_t get_new_cid()
{
	static uint32_t base = 0xcafebabe;
	int i;
	for(i = 0; i < CID_MAX-1; i++)
	{
		if (!CIDS[i].busy)
		{
			goto newcid;
		}
	}
	return 0;
	newcid:

	do
	{
		CIDS[i].cid = base + CID_NUM++;
	}while(CIDS[i].cid == 0 || CIDS[i].cid == U2FHID_BROADCAST);

	CIDS[i].busy = 0;

	return CIDS[i].cid;
}

static int8_t add_new_cid(uint32_t cid)
{
	int i;
	for(i = 0; i < CID_MAX-1; i++)
	{
		if (!CIDS[i].busy)
		{
			CIDS[i].cid = cid;
			return 0;
		}
	}
	return -1;
}

static struct CID* get_cid(uint32_t cid)
{
	uint8_t i;
	for(i = 0; i < CID_MAX; i++)
	{
		if (CIDS[i].cid == cid)
		{
			return CIDS+i;
		}
	}
	return NULL;
}
static void del_cid(uint32_t cid)
{
	uint8_t i;
	for(i = 0; i < CID_MAX; i++)
	{
		if (CIDS[i].cid == cid)
		{
			CIDS[i].cid = 0;
			CIDS[i].busy = 0;
		}
	}
}

static uint8_t errbuf[HID_PACKET_SIZE];
static void stamp_error(uint32_t cid, uint8_t err)
{

	struct u2f_hid_msg * res = (struct u2f_hid_msg *)errbuf;
	memset(errbuf,0,sizeof(errbuf));
	res->cid = cid;
	res->pkt.init.cmd = U2FHID_ERROR;
	res->pkt.init.payload[0] = err;
	res->pkt.init.bcnth = 0;
	res->pkt.init.bcntl = 1;


	usb_write(res, HID_PACKET_SIZE);
	del_cid(cid);
}

static void start_buffering(struct u2f_hid_msg* req)
{
	_hid_in_session = 1;
	hid_layer.bytes_buffered = U2FHID_INIT_PAYLOAD_SIZE;
	hid_layer.req_len = U2FHID_LEN(req);
	memmove(hid_layer.buffer, req->pkt.init.payload, U2FHID_INIT_PAYLOAD_SIZE);
}

static int buffer_request(struct u2f_hid_msg* req)
{
	if (hid_layer.bytes_buffered + U2FHID_CONT_PAYLOAD_SIZE > BUFFER_SIZE)
	{
		set_app_error(ERROR_HID_BUFFER_FULL);
		stamp_error(req->cid, ERR_OTHER);
		return -1;
	}
	memmove(hid_layer.buffer + hid_layer.bytes_buffered, req->pkt.cont.payload, U2FHID_CONT_PAYLOAD_SIZE);
	hid_layer.bytes_buffered += U2FHID_CONT_PAYLOAD_SIZE;
	return 0;
}

// return 0 if finished
// return 1 if expecting more cont packets
static uint8_t hid_u2f_parse(struct u2f_hid_msg* req)
{
	uint16_t len = 0;
	uint8_t secs;
	struct u2f_hid_init_response * init_res = appdata.tmp;

	switch(hid_layer.current_cmd)
	{
		case U2FHID_INIT:
			if (U2FHID_LEN(req) != 8)
			{
				stamp_error(hid_layer.current_cid, ERR_INVALID_LEN);
				goto fail;
			}
			u2f_hid_set_len(17);


			if (hid_layer.current_cid == U2FHID_BROADCAST)
			{
				if (hid_layer.current_cid == 0)
				{
					set_app_error(ERROR_OUT_OF_CIDS);
					goto fail;
				}
				init_res->cid = get_new_cid();
			}
			else
			{
				init_res->cid = hid_layer.current_cid;
			}
			init_res->version_id = 2;
			init_res->version_major = 2;
			init_res->version_minor = 0;
			init_res->version_build = 0;
			init_res->cflags = CAPABILITY_WINK | CAPABILITY_LOCK;

			// write back the same data nonce
			u2f_hid_writeback(req->pkt.init.payload, 8);
			u2f_hid_writeback((uint8_t *)init_res, 9);
			u2f_hid_flush();

			hid_layer.current_cid = init_res->cid;



			break;
		case U2FHID_MSG:

			if (U2FHID_LEN(req) < 4)
			{
				stamp_error(hid_layer.current_cid, ERR_INVALID_LEN);
				goto fail;
			}
			// buffer 2 payloads (120 bytes) to get full U2F message
			// assuming key handle is < 45 bytes
			//		7 bytes for apdu header
			//		7 + 66 bytes + key handle for authenticate message
			//      7 + 64 for register message
			if (hid_layer.bytes_buffered == 0)
			{
				start_buffering(req);
				if (hid_layer.bytes_buffered >= U2FHID_LEN(req))
				{
					u2f_request((struct u2f_request_apdu *)hid_layer.buffer);
				}
			}
			else
			{
				buffer_request(req);
				if (hid_layer.bytes_buffered >= hid_layer.req_len)
				{
					u2f_request((struct u2f_request_apdu *)hid_layer.buffer);
				}
			}


			break;
		case U2FHID_PING:


			if (hid_layer.bytes_buffered == 0)
			{
				start_buffering(req);
				u2f_hid_set_len(U2FHID_LEN(req));
				if (hid_layer.bytes_buffered >= U2FHID_LEN(req))
				{
					u2f_hid_writeback(hid_layer.buffer,hid_layer.bytes_buffered);
					u2f_hid_flush();
				}
				else
				{
					return 1;
				}
			}
			else
			{
				if (hid_layer.bytes_buffered + U2FHID_CONT_PAYLOAD_SIZE > BUFFER_SIZE)
				{
					u2f_hid_writeback(hid_layer.buffer,hid_layer.bytes_buffered);
					hid_layer.bytes_buffered = 0;

				}

				buffer_request(req);
				if (hid_layer.bytes_buffered + hid_layer.bytes_written >= hid_layer.req_len)
				{
					u2f_hid_writeback(hid_layer.buffer,hid_layer.bytes_buffered);
					u2f_hid_flush();
				}
				else
				{
					return 1;
				}
			}


			break;

		case U2FHID_WINK:
			if (U2FHID_LEN(req) != 0)
			{
				// this one is safe
				stamp_error(hid_layer.current_cid, ERR_INVALID_LEN);
			}
			u2f_hid_set_len(0);
			u2f_hid_writeback(NULL, 0);
			u2f_hid_flush();
			app_wink(U2F_COLOR_WINK);
			break;
		case U2FHID_LOCK:

			secs = req->pkt.init.payload[0];
			if (secs > 10)
			{
				stamp_error(hid_layer.current_cid, ERR_INVALID_PAR);
			}
			else
			{
				if (secs)
				{
					_hid_lock_cid = hid_layer.current_cid;
					_hid_lockt = get_ms() + 1000 * secs;

				}
				else
				{
					_hid_lockt = get_ms();
					_hid_lock_cid = 0;
				}
				hid_layer.current_cmd = U2FHID_LOCK;
				u2f_hid_set_len(0);
				u2f_hid_writeback(NULL, 0);
				u2f_hid_flush();
			}
			break;
		default:
			set_app_error(ERROR_HID_INVALID_CMD);
			stamp_error(hid_layer.current_cid, ERR_INVALID_CMD);
			u2f_printb("invalid cmd: ",1,hid_layer.current_cmd);
	}

	return u2f_hid_busy();

	fail:
		u2f_prints("U2F HID FAIL\r\n");
	return 0;
}

void u2f_hid_check_timeouts()
{
	uint8_t i;
	for(i = 0; i < CID_MAX; i++)
	{
		if (CIDS[i].busy && ((get_ms() - CIDS[i].last_used) >= 750))
		{
			u2f_printlx("timeout cid ",2,CIDS[i].cid,get_ms());
			stamp_error(CIDS[i].cid, ERR_MSG_TIMEOUT);
			del_cid(CIDS[i].cid);
			u2f_hid_reset_packet();
		}
	}

}


void u2f_hid_request(struct u2f_hid_msg* req)
{
	static int8_t last_seq;
	struct CID* cid = NULL;

	cid = get_cid(req->cid);

	// Error checking
	if ((U2FHID_IS_INIT(req->pkt.init.cmd)))
	{
		if (U2FHID_LEN(req) > 7609)
		{
			stamp_error(req->cid, ERR_INVALID_LEN);
			return;
		}
		if (req->pkt.init.cmd != U2FHID_INIT && req->cid != hid_layer.current_cid && u2f_hid_busy())
		{
			stamp_error(req->cid, ERR_CHANNEL_BUSY);
			return;
		}
	}
	else if (cid == NULL || !cid->busy)
	{
		// ignore random cont packets
		return;
	}

	if (!req->cid)
	{
		stamp_error(req->cid, ERR_SYNC_FAIL);
		return;
	}



	if (req->cid == U2FHID_BROADCAST)
	{
		if (!(req->pkt.init.cmd == U2FHID_INIT))
		{
			stamp_error(req->cid, ERR_SYNC_FAIL);
			return;
		}
		cid = &BROADCAST_CID;
		BROADCAST_CID.cid = U2FHID_BROADCAST;
	}
	else if (U2FHID_IS_INIT(req->pkt.init.cmd) && cid == NULL)
	{
		add_new_cid(req->cid);
		cid = get_cid(req->cid);
		if (cid == NULL)
		{
			return;
		}
		cid->busy = 0;
	}




	// Reset init packets
	if (req->pkt.init.cmd == U2FHID_INIT)
	{
		cid->busy = 0;
	}

	hid_layer.current_cid = req->cid;
	hid_layer.last_buffered = get_ms();

	cid->last_used = get_ms();


	// ignore if we locked to a different cid
	if(hid_is_locked() && req->pkt.init.cmd != U2FHID_INIT)
	{
		if (!hid_is_lock_cid(req->cid))
		{
			stamp_error(req->cid, ERR_CHANNEL_BUSY);
			return;
		}
	}

	if ((req->pkt.init.cmd & TYPE_INIT) && !cid->busy)
	{
		cid->last_cmd = req->pkt.init.cmd;
		hid_layer.current_cmd = req->pkt.init.cmd;
		last_seq = -1;

	}
	else
	{


		// verify packets arrive in ascending order
		hid_layer.last_buffered = get_ms();
		if (last_seq + 1 != req->pkt.cont.seq)
		{
			stamp_error(hid_layer.current_cid, ERR_INVALID_SEQ);
			u2f_hid_reset_packet();
			return;
		}
		last_seq = req->pkt.cont.seq;

		hid_layer.current_cmd = cid->last_cmd;

	}

	cid->busy = hid_u2f_parse(req);

}

#endif
