/*
 * u2f_hid.c
 *
 *  Created on: Jan 26, 2016
 *      Author: pp
 */
#include <stdint.h>
#include <string.h>

#include "app.h"
#include "bsp.h"
#include "u2f_hid.h"
#include "u2f.h"


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
//	uint16_t bytes_buffered;

	// number of payload bytes written in response
	uint16_t bytes_written;



	// total length of response in bytes
	uint16_t res_len;

#ifndef U2F_PRINT
	#define BUFFER_SIZE 300
	uint8_t buffer[BUFFER_SIZE];
#else
	#define BUFFER_SIZE 70
	uint8_t buffer[BUFFER_SIZE];
#endif

} hid_layer;

struct CID CIDS[5];
static uint8_t CID_NUM = 0;

static uint8_t _hid_pkt[HID_PACKET_SIZE];
static uint8_t _hid_offset = 0;
static uint8_t _hid_seq = 0;
static uint8_t _hid_in_session = 0;

#define u2f_hid_busy() (_hid_in_session)

#define MIN(a,b) ((a) < (b) ? (a):(b))

void u2f_hid_init()
{
	memset(CIDS, 0, sizeof(CIDS));
	memset(&hid_layer, 0, sizeof(hid_layer));
	CID_NUM = 0;
	_hid_offset = 0;
	_hid_seq = 0;
	_hid_in_session = 0;
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
static void u2f_hid_flush()
{
	if (_hid_offset)
	{

		usb_write(_hid_pkt, HID_PACKET_SIZE);

	}
	u2f_hid_reset_packet();
}


void u2f_hid_writeback(uint8_t * payload, uint8_t len)
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

static int check_cid(uint32_t cid)
{
	return (get_cid(cid) != NULL);
}

static void stamp_error(uint32_t cid, uint8_t err)
{
	struct u2f_hid_msg * res = (struct u2f_hid_msg *)_hid_pkt;
	res->cid = cid;
	res->pkt.init.cmd = U2FHID_ERROR;
	res->pkt.init.payload[0] = err;
	res->pkt.init.bcnth = 0;
	res->pkt.init.bcntl = 1;
	// TODO this is tramping on potential other response data
	usb_write(_hid_pkt, HID_PACKET_SIZE);
	del_cid(cid);
}

//// TODO double check if this is really needed
//static void start_buffering(struct hid_layer_param* hid, struct u2f_hid_msg* req)
//{
//	hid->bytes_buffered = U2FHID_INIT_PAYLOAD_SIZE;
//	hid->bytes_remain = U2FHID_LEN(req) - U2FHID_INIT_PAYLOAD_SIZE;
//	hid->last_buffered = get_ms();
//	memmove(hid->buffer, req->pkt.init.payload, U2FHID_INIT_PAYLOAD_SIZE);
//}
//
//static int buffer_request(struct hid_layer_param* hid, struct u2f_hid_msg* req)
//{
//	if (hid->bytes_remain < U2FHID_CONT_PAYLOAD_SIZE)
//	{
//		if (hid->bytes_buffered + hid->bytes_remain > BUFFER_SIZE)
//		{
//			goto fail;
//		}
//		memmove(hid->buffer + hid->bytes_buffered, req->pkt.cont.payload, hid->bytes_remain);
//		hid->bytes_buffered += hid->bytes_remain;
//		hid->bytes_remain = 0;
//		hid->state = HID_READY;
//	}
//	else
//	{
//		if (hid->bytes_buffered + U2FHID_CONT_PAYLOAD_SIZE > BUFFER_SIZE)
//		{
//			goto fail;
//		}
//		memmove(hid->buffer+ hid->bytes_buffered, req->pkt.cont.payload, U2FHID_CONT_PAYLOAD_SIZE);
//		hid->bytes_buffered += U2FHID_CONT_PAYLOAD_SIZE;
//		hid->bytes_remain -= U2FHID_CONT_PAYLOAD_SIZE;
//	}
//	hid->last_buffered = get_ms();
//	return 0;
//
//	fail:
//	hid->state = HID_READY;
//	u2f_prints("buffer full\r\n");
//	return -1;
//}

static void hid_u2f_parse(struct u2f_hid_msg* req)
{

	uint16_t len = 0;
	struct u2f_hid_init_response * init_res = appdata.tmp;

	switch(hid_layer.current_cmd)
	{
		case U2FHID_INIT:
			u2f_printlx("got init packet ",1,req->cid);
			if (U2FHID_LEN(req) != 8)
			{
				// this one is safe
				stamp_error(hid_layer.current_cid, ERR_INVALID_LEN);
				u2f_prints("invalid len init\r\n");
				goto fail;
			}
			hid_layer.res_len = 17;

			u2f_printlx("cid: ",1,hid_layer.current_cid);
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
				// TODO bad
				stamp_error(hid_layer.current_cid, ERR_INVALID_LEN);
				u2f_prints("invalid len msg\r\n");
				goto fail;
			}

			u2f_request((struct u2f_request_apdu *)req->pkt.init.payload);

			break;
		case U2FHID_PING:

			u2f_prints("U2F PING\r\n");

			if (!u2f_hid_busy())
			{

				hid_layer.res_len = U2FHID_LEN(req);
				u2f_hid_writeback(req->pkt.init.payload, MIN(hid_layer.res_len, U2FHID_INIT_PAYLOAD_SIZE));
			}
			else
			{
				u2f_hid_writeback(req->pkt.cont.payload, MIN(hid_layer.res_len - hid_layer.bytes_written, U2FHID_CONT_PAYLOAD_SIZE));
			}

			// u2f_printd("reslen byteswritten ",2, hid_layer.res_len,hid_layer.bytes_written);
			if (hid_layer.res_len == hid_layer.bytes_written) u2f_hid_flush();
			break;

		case U2FHID_WINK:
			u2f_prints("U2F WINK\r\n");
			if (U2FHID_LEN(req) != 0)
			{
				// this one is safe
				stamp_error(hid_layer.current_cid, ERR_INVALID_LEN);
				u2f_prints("invalid len wink but who cares\r\n");
			}
			hid_layer.res_len = 0;
			u2f_hid_writeback(NULL, 0);
			u2f_hid_flush();
			appdata.state = APP_WINK;
			break;
		case U2FHID_LOCK:
			// TODO
			u2f_prints("U2F LOCK\r\n");
			break;
		default:
			u2f_printb("invalid cmd: ", hid_layer.current_cmd);
			stamp_error(hid_layer.current_cid, ERR_INVALID_CMD);
			goto fail;
	}

	return;

	fail:
		u2f_prints("U2F HID FAIL\r\n");
	return;
}


void u2f_hid_request(struct u2f_hid_msg* req)
{
	uint8_t* payload = req->pkt.init.payload;
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
		u2f_printlx("ignoring pkt ",1,req->cid);
		return;
	}

	hid_layer.state = (u2f_hid_busy()) ? HID_BUSY : HID_READY;

	switch(hid_layer.state)
	{
		case HID_READY:
			if (req->pkt.init.cmd & TYPE_INIT)
			{
				if (U2FHID_LEN(req) > U2FHID_MAX_PAYLOAD_SIZE)
				{
					u2f_prints("length too big\r\n");
					stamp_error(req->cid, ERR_INVALID_LEN);
					return;
				}
				u2f_hid_reset_packet();
				hid_layer.current_cid = req->cid;
				hid_layer.current_cmd = req->pkt.init.cmd;
				hid_layer.last_buffered = get_ms();


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
					// TODO
					u2f_prints("this should resync but im lazy\r\n");
				}
				hid_layer.last_buffered = get_ms();
//				// TODO verify packets arrive in ascending order
//				if (buffer_request(&hid_layer, req) != 0)
//				{
//					// no suitable error code so ignore
//					u2f_prints("cant buffer request\r\n");
//					return;
//				}
//				u2f_printlx("buffered from ", 1, req->cid);
//				payload = hid_layer.buffer;

			}
			else if (U2FHID_TIMEOUT(&hid_layer))
			{
				// return timeout error for old channel and run again for new channel
				u2f_prints("timeout, switching\r\n");
				hid_layer.state = HID_READY;
				u2f_hid_reset_packet();
				stamp_error(hid_layer.current_cid, ERR_MSG_TIMEOUT);
				u2f_hid_request(req);
				return;
			}
			else
			{
				// Current application may not be interrupted
				// TODO this will be bad
				stamp_error(req->cid, ERR_CHANNEL_BUSY);
				u2f_printlx("ERR_CHANNEL_BUSY ", 2, req->cid, hid_layer.current_cid);
				return;
			}
			break;

	}

	hid_u2f_parse(req);


	return;
}

