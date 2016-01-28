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

static uint32_t CIDS[8];
static uint8_t CID_NUM = 0;
#define CID_MAX (sizeof(CIDS)/sizeof(uint32_t))

typedef enum
{
	HID_BUSY=0,
	HID_READY,
} HID_STATE;

static struct
{
	HID_STATE state;
	uint32_t current_cid;
	uint16_t current_pktl;
	uint16_t current_pktr;
} hid_layer;

#ifndef U2F_PRINT
static xdata uint8_t BUFFER[712];
#else
static xdata uint8_t BUFFER[294];
#endif


static void hid_u2f_parse(struct u2f_hid_msg* req, struct u2f_hid_msg* res);

void u2f_hid_init()
{
	BUFFER[0] = 0xff;
	memset(CIDS, 0, sizeof(CIDS));
	CID_NUM = 0;
}

uint32_t get_new_cid()
{
	static uint32_t base = 0xcafebabe;
	int i;
	for(i = 0; i < CID_NUM; i++)
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
	for(i = 0; i < CID_NUM; i++)
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

int hid_u2f_request(struct u2f_hid_msg* req, struct u2f_hid_msg* res)
{
	// Ignore CID's we did not allocate.
	if (!check_cid(req->cid))
	{
		u2f_print("ignoring pkt %lx\r\n",req->cid);
		return -1;
	}

	res->cid = req->cid;
	res->pkt.init.cmd = req->pkt.init.cmd;

	switch(hid_layer.state)
	{
		case HID_BUSY:

			if (req->cid == hid_layer.current_cid)
			{

			}
			else
			{
				// Current application may not be interrupted
				stamp_error(res, ERR_CHANNEL_BUSY);
				u2f_print("ERR_CHANNEL_BUSY\r\n");
				return 0;
			}
			break;
		case HID_READY:
			break;
	}

	hid_u2f_parse(req,res);

	return 0;
}
static void hid_u2f_parse(struct u2f_hid_msg* req, struct u2f_hid_msg* res)
{
	char nbuf[10];
	struct u2f_hid_init_response* init_res =
			(struct u2f_hid_init_response*) res->pkt.init.payload;

	uint16_t len = 0;

	switch(req->pkt.init.cmd)
	{
		case U2FHID_INIT:
			u2f_print("got init packet %lx\r\n",req->cid);
			memmove(init_res->nonce.nonce, req->pkt.init.payload, 8);
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
			u2f_print("u2f msg %lx %bx\r\n",req->cid, req->pkt.init.cmd);
			break;
		default:
			u2f_print("invalid cmd: %bx\r\n", req->pkt.init.cmd);
			break;
	}


	res->pkt.init.bcnth = len>>8;
	res->pkt.init.bcntl = len&0xff;
	return;

	fail:
		u2f_print("U2F HID FAIL\r\n");
	return;
}

