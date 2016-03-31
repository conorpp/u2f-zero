/*
 * u2f_hid.c
 *
 *  Created on: Jan 26, 2016
 *      Author: pp
 */
#include "app.h"

#include <stdint.h>
#include <string.h>

#include "bsp.h"
#include "u2f_hid.h"
#include "u2f.h"

#ifndef U2F_HID_DISABLE

#define CID_MAX (sizeof(CIDS)/sizeof(uint32_t))


typedef enum
{
	HID_BUSY=0,
	HID_READY,
} HID_STATE;

struct CID
{
	uint32_t cid;
	uint32_t last_used;
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

	#define BUFFER_SIZE 200
	uint8_t buffer[BUFFER_SIZE];

} hid_layer;

uint32_t _hid_lockt = 0;
uint32_t _hid_lock_cid = 0;

struct CID CIDS[5];
static uint8_t CID_NUM = 0;

static uint8_t _hid_pkt[HID_PACKET_SIZE];
static uint8_t _hid_offset = 0;
static uint8_t _hid_seq = 0;
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


void u2f_hid_writeback(uint8_t * payload, uint16_t len)
{
	struct u2f_hid_msg * r = (struct u2f_hid_response *) _hid_pkt;
	_hid_in_session = 1;
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
			r->pkt.cont.seq = _hid_seq - 1;
			_hid_offset = 5;
			if (_hid_seq-1 > 127)
			{
				set_app_error(ERROR_SEQ_EXCEEDED);
				return;
			}
		}
	}
	while(len--)
	{
		_hid_pkt[_hid_offset++] = *payload++;
		hid_layer.bytes_written++;
		if (_hid_offset == HID_PACKET_SIZE)
		{
			_hid_offset = 0;
			_hid_seq++;

			usb_write(_hid_pkt, HID_PACKET_SIZE);
			memset(_hid_pkt, 0, HID_PACKET_SIZE);

			if (len)
			{
				u2f_hid_writeback(payload, len);
				return;
			}
			else break;
		}
	}

}

static uint8_t is_cid_free(struct CID* c)
{
	return (c->cid == 0 || get_ms() - c->last_used > 1000);
}

static void refresh_cid(struct CID* c)
{
	c->last_used = get_ms();
}


static uint32_t get_new_cid()
{
	static uint32_t base = 0xcafebabe;
	int i;
	for(i = 0; i < CID_MAX; i++)
	{
		if (is_cid_free(CIDS+i))
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

	refresh_cid(CIDS+i);

	return CIDS[i].cid;
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
		}
	}
}


static void stamp_error(uint32_t cid, uint8_t err)
{
	uint8_t errbuf[HID_PACKET_SIZE];
	struct u2f_hid_msg * res = (struct u2f_hid_msg *)errbuf;
	res->cid = cid;
	res->pkt.init.cmd = U2FHID_ERROR;
	res->pkt.init.payload[0] = err;
	res->pkt.init.bcnth = 0;
	res->pkt.init.bcntl = 1;

	usb_write(errbuf, HID_PACKET_SIZE);
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
}

static void hid_u2f_parse(struct u2f_hid_msg* req)
{

	uint16_t len = 0;
	uint8_t secs;
	struct u2f_hid_init_response * init_res = appdata.tmp;

	switch(hid_layer.current_cmd)
	{
		case U2FHID_INIT:
			//u2f_printlx("got init packet ",1,req->cid);
			if (U2FHID_LEN(req) != 8)
			{
				// this one is safe
				stamp_error(hid_layer.current_cid, ERR_INVALID_LEN);
				u2f_prints("invalid len init\r\n");
				goto fail;
			}
			u2f_hid_set_len(17);

			//u2f_printlx("cid: ",1,hid_layer.current_cid);
			if (hid_layer.current_cid == 0)
			{
				u2f_prints("out of cid's\r\n");
				goto fail;
			}

			init_res->cid = get_new_cid();
			init_res->version_id = 1;
			init_res->version_major = 1;
			init_res->version_minor = 0;
			init_res->version_build = 0;
			init_res->cflags = 0;

			// write back the same data nonce
			u2f_hid_writeback(req->pkt.init.payload, 8);
			u2f_hid_writeback(init_res, 9);
			u2f_hid_flush();
			hid_layer.current_cid = init_res->cid;

			break;
		case U2FHID_MSG:

			if (U2FHID_LEN(req) < 4)
			{
				stamp_error(hid_layer.current_cid, ERR_INVALID_LEN);
				u2f_prints("invalid len msg\r\n");
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

			//u2f_prints("U2F PING\r\n");

			if (!u2f_hid_busy())
			{
				u2f_hid_set_len(U2FHID_LEN(req));
				u2f_hid_writeback(req->pkt.init.payload, MIN(hid_layer.res_len, U2FHID_INIT_PAYLOAD_SIZE));
			}
			else
			{
				u2f_hid_writeback(req->pkt.cont.payload, MIN(hid_layer.res_len - hid_layer.bytes_written, U2FHID_CONT_PAYLOAD_SIZE));
			}

			if (hid_layer.res_len == hid_layer.bytes_written) u2f_hid_flush();
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
			set_app_state(APP_WINK);
			break;
		case U2FHID_LOCK:

			secs = req->pkt.init.payload[0];
			if (secs > 10)
			{
				stamp_error(hid_layer.current_cid, ERR_INVALID_PAR);
			}
			else
			{
				_hid_lock_cid = hid_layer.current_cid;
				_hid_lockt = get_ms() + 1000 * secs;
				u2f_hid_set_len(0);
				u2f_hid_writeback(NULL, 0);
				u2f_hid_flush();
			}

			break;
		default:
			set_app_error(ERROR_HID_INVALID_CMD);
			stamp_error(hid_layer.current_cid, ERR_INVALID_CMD);
	}

	return;

	fail:
		u2f_prints("U2F HID FAIL\r\n");
	return;
}


void u2f_hid_request(struct u2f_hid_msg* req)
{
	uint8_t* payload = req->pkt.init.payload;
	static int8_t last_seq = -1;
	struct CID* cid = get_cid(req->cid);


	if (cid != NULL)
	{
		refresh_cid(cid);
	}
	else if (req->cid == U2FHID_BROADCAST)
	{

	}
	else
	{
		// Ignore CID's we did not allocate.
		//u2f_printlx("ignoring pkt ",1,req->cid);
		return;
	}

	// ignore if we locked to a different cid
	if(hid_is_locked())
	{
		if (!hid_is_lock_cid(cid))
		{
			stamp_error(hid_layer.current_cid, ERR_CHANNEL_BUSY);
			return;
		}
	}

	hid_layer.state = (u2f_hid_busy()) ? HID_BUSY : HID_READY;

	switch(hid_layer.state)
	{
		case HID_READY:
			if (req->pkt.init.cmd & TYPE_INIT)
			{
				if (U2FHID_LEN(req) > U2FHID_MAX_PAYLOAD_SIZE)
				{
					//u2f_prints("length too big\r\n");
					stamp_error(req->cid, ERR_INVALID_LEN);
					return;
				}
				u2f_hid_reset_packet();
				hid_layer.current_cid = req->cid;
				hid_layer.current_cmd = req->pkt.init.cmd;
				hid_layer.last_buffered = get_ms();
				last_seq = -1;

			}
			else
			{
				stamp_error(req->cid, ERR_INVALID_CMD);
				u2f_prints("ERR_INVALID_CMD\r\n");
				return;
			}

			break;
		case HID_BUSY:
			// buffer long requests
			if (req->cid == hid_layer.current_cid)
			{
				if (req->pkt.init.cmd & TYPE_INIT)
				{
					u2f_hid_reset_packet();
					u2f_hid_request(req);
					return;
				}

				hid_layer.last_buffered = get_ms();

				// verify packets arrive in ascending order
				if (last_seq + 1 != req->pkt.cont.seq)
				{
					u2f_hid_reset_packet();
					stamp_error(hid_layer.current_cid, ERR_INVALID_SEQ);
					return;
				}
				last_seq = req->pkt.cont.seq;

			}
			else if (U2FHID_TIMEOUT(&hid_layer))
			{
				// return timeout error for old channel and run again for new channel
				//u2f_prints("timeout, switching\r\n");
				hid_layer.state = HID_READY;
				u2f_hid_reset_packet();
				stamp_error(hid_layer.current_cid, ERR_MSG_TIMEOUT);
				u2f_hid_request(req);
				return;
			}
			else
			{
				// Current application may not be interrupted
				stamp_error(req->cid, ERR_CHANNEL_BUSY);
				return;
			}
			break;

	}

	hid_u2f_parse(req);
	return;
}

#endif
