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

static uint32_t CIDS[8];
static uint8_t CID_NUM = 0;
#define CID_MAX (sizeof(CIDS)/sizeof(uint32_t))

typedef enum
{
	HID_BUSY=0,
	HID_READY,
} HID_STATE;

static struct hid_layer_param
{
	HID_STATE state;
	uint32_t current_cid;
	uint16_t bytes_buffered;
	uint16_t bytes_remain;
	uint8_t current_cmd;

#ifndef U2F_PRINT
#define BUFFER_SIZE 600
uint8_t buffer[BUFFER_SIZE];
#else
#define BUFFER_SIZE 250
uint8_t buffer[BUFFER_SIZE];
#endif

} hid_layer;




static void hid_u2f_parse(struct u2f_hid_msg* req, struct u2f_hid_msg* res,
		uint8_t cmd, uint8_t* payload);

void u2f_hid_init()
{
	memset(CIDS, 0, sizeof(CIDS));
	hid_layer.state = HID_READY;
	CID_NUM = 0;
}

uint32_t get_new_cid()
{
	static uint32_t base = 0xcafebabe;
	int i;
	for(i = 0; i < CID_MAX; i++)
	{
		if (CIDS[i] == 0)
		{
			goto newcid;
		}
	}
	return 0;
	newcid:

	do
	{
		CIDS[i] = 0xcafebabe + CID_NUM++;
	}while(CIDS[i] == 0);

	return CIDS[i];
}

//todo
void remove_cid()
{

}

int check_cid(uint32_t cid)
{
	int i;
	if (cid == U2FHID_BROADCAST)
	{
		return 1;
	}
	for(i = 0; i < CID_MAX; i++)
	{
		if (CIDS[i] == cid)
		{
			return 1;
		}
	}
	return 0;
}

static void stamp_error(struct u2f_hid_msg* res, uint8_t err)
{
	res->pkt.init.cmd = U2FHID_ERROR;
	res->pkt.init.payload[0] = err;
	res->pkt.init.bcnth = 0;
	res->pkt.init.bcntl = 1;

}

static void start_buffering(struct hid_layer_param* hid, struct u2f_hid_msg* req)
{
	hid->state = HID_BUSY;
	hid->bytes_buffered = U2FHID_INIT_PAYLOAD_SIZE;
	hid->bytes_remain = U2FHID_LEN(req) - U2FHID_INIT_PAYLOAD_SIZE;
	hid->current_cid = req->cid;
	hid->current_cmd = req->pkt.init.cmd;
	memmove(hid->buffer, req->pkt.init.payload, U2FHID_INIT_PAYLOAD_SIZE);
}

static int buffer_request(struct hid_layer_param* hid, struct u2f_hid_msg* req)
{
	if (hid->bytes_remain < U2FHID_CONT_PAYLOAD_SIZE)
	{
		if (hid->bytes_buffered + hid->bytes_remain > BUFFER_SIZE)
		{
			goto fail;
		}
		memmove(hid->buffer, req->pkt.cont.payload, hid->bytes_remain);
		hid->bytes_buffered += hid->bytes_remain;
		hid->bytes_remain = 0;
		hid->state = HID_READY;
	}
	else
	{
		if (hid->bytes_buffered + U2FHID_CONT_PAYLOAD_SIZE > BUFFER_SIZE)
		{
			goto fail;
		}
		memmove(hid->buffer, req->pkt.cont.payload, U2FHID_CONT_PAYLOAD_SIZE);
		hid->bytes_buffered += U2FHID_CONT_PAYLOAD_SIZE;
		hid->bytes_remain -= U2FHID_CONT_PAYLOAD_SIZE;
	}

	return 0;

	fail:
	hid->state = HID_READY;
	u2f_print("buffer full\r\n");
	return -1;
}

int hid_u2f_request(struct u2f_hid_msg* req, struct u2f_hid_msg* res)
{
	uint8_t cmd = req->pkt.init.cmd;
	uint8_t* payload = req->pkt.init.payload;

	// Ignore CID's we did not allocate.
	if (!check_cid(req->cid))
	{
		u2f_print("ignoring pkt %lx\r\n",req->cid);
		return U2FHID_FAIL;
	}

	res->cid = req->cid;



	switch(hid_layer.state)
	{
		case HID_BUSY:

			// buffer long requests
			if (req->cid == hid_layer.current_cid)
			{
				if (buffer_request(&hid_layer, req) != 0)
				{
					// no real fitting error code so ignore
					u2f_print("cant buffer request\r\n");
					return U2FHID_FAIL;
				}
				cmd = hid_layer.current_cmd;
				payload = hid_layer.buffer;

			}
			else
			{
				// Current application may not be interrupted
				stamp_error(res, ERR_CHANNEL_BUSY);
				u2f_print("ERR_CHANNEL_BUSY %lx %lx\r\n", req->cid, hid_layer.current_cid);
				return U2FHID_REPLY;
			}
			break;
		case HID_READY:

			if (req->pkt.init.cmd & TYPE_INIT)
			{
				if (U2FHID_LEN(req) > U2FHID_INIT_PAYLOAD_SIZE)
				{
					start_buffering(&hid_layer, req);
				}
			}
			else
			{
				stamp_error(res, ERR_INVALID_CMD);
				u2f_print("ERR_INVALID_CMD\r\n");
				return U2FHID_REPLY;
			}

			break;
	}

	res->pkt.init.cmd = cmd;

	if (hid_layer.state == HID_READY)
	{
		hid_u2f_parse(req, res, cmd, payload);
	}
	else
	{
		// wait for request to buffer
		return U2FHID_WAIT;
	}


	return U2FHID_REPLY;
}
static void hid_u2f_parse(struct u2f_hid_msg* req, struct u2f_hid_msg* res,
		uint8_t cmd, uint8_t* payload)
{
	struct u2f_hid_init_response* init_res =
			(struct u2f_hid_init_response*) res->pkt.init.payload;

	uint16_t len = 0;

	switch(cmd)
	{
		case U2FHID_INIT:
			u2f_print("got init packet %lx\r\n",req->cid);
			memmove(init_res->nonce.nonce, payload, 8);
			init_res->cid = get_new_cid();
			if (init_res->cid == 0)
			{
				goto fail;
			}
			init_res->version_id = 1;
			init_res->version_major = 1;
			init_res->version_minor = 0;
			init_res->version_build = 0;
			init_res->cflags = 0;
			len = 17;

			break;
		case U2FHID_MSG:

			u2f_request((struct u2f_message*)payload,
					(struct u2f_message*)res->pkt.init.payload);

			break;
		default:
			u2f_print("invalid cmd: %bx\r\n", cmd);
			break;
	}

	U2FHID_SET_LEN(res, len);
	return;

	fail:
		u2f_print("U2F HID FAIL\r\n");
	return;
}

