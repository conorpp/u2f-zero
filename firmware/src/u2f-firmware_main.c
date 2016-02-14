#include <SI_EFM8UB1_Register_Enums.h>
#include <stdarg.h>

#include "efm8_usb.h"
#include "usb_0.h"
#include "InitDevice.h"
#include "descriptors.h"
#include "idle.h"
#include "bsp.h"
#include "app.h"
#include "u2f_hid.h"


uint8_t keySeqNo = 0;        // Current position in report table.
bool keyPushed = 0;          // Current pushbutton status.

bool readpacket = 1;

data struct APP_DATA appdata;
extern uint8_t* SMB_WRITE_BUF;
extern uint8_t SMB_WRITE_BUF_LEN;
extern uint8_t SMB_WRITE_BUF_OFFSET;
extern uint8_t SMB_READ_LEN;
extern uint8_t SMB_READ_OFFSET;
extern uint8_t* SMB_READ_BUF;

#ifdef U2F_PRINT
FIFO_CREATE(debug,struct debug_msg, 5)
#endif

static void init(struct APP_DATA* ap)
{
	memset(ap,0, sizeof(struct APP_DATA));

	ap->state = APP_NOTHING;
	debug_fifo_init();
	u2f_hid_init();

	SMB_WRITE_BUF = NULL;
	SMB_READ_BUF = NULL;
	SMB_READ_OFFSET = 0;
	SMB_WRITE_BUF_LEN = 0;
	SMB_WRITE_BUF_OFFSET = 0;
	SMB_READ_LEN = 0;

}

void listen_for_pkt(struct APP_DATA* ap)
{
	USBD_Read(EP1OUT, ap->hidmsgbuf, sizeof(ap->hidmsgbuf), true);
}

#define ms_since(ms,num) (((uint16_t)get_ms() - (ms)) >= num ? (1|(ms=(uint16_t)get_ms())):0)

void reset_i2c()
{
    SMB0CF &= ~0x80;                 // Reset communication
    SMB0CF |= 0x80;
    SMB0CN0_STA = 0;
    SMB0CN0_STO = 0;
    SMB0CN0_ACK = 0;
}

// SMB0DAT - read write data from i2c
// SMB0CN0_ACK = 0|1, 0 to nack, 1 to ack
// SMB0CN0_STO = 1, send stop
// SMB0CN0_STA = 1, send start

#define TX_BUSY (!(SMB0FCN1 & 0x40))
#define RX_EMPTY ((SMB0FCN1 & 0x04))


#ifdef U2F_PRINT
	static xdata struct debug_msg dbg;
#endif

static void flush_messages()
{
	while(debug_fifo_get(&dbg) == 0)
	{
		u2f_write_s(dbg.buf);
	}
}



void wakeup_i2c_slave()
{
	uint32_t i;
	for (i=0; i<15000; i++)
	{
		SMB0DAT = 0;

	}
}



static uint16_t crc16(uint8_t* buf, uint16_t len)
{
#define CRC16 0xa001
	uint16_t crc = 0;
	while (len--) {
		crc ^= *buf++;
		crc = crc & 1 ? (crc >> 1) ^ CRC16 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ CRC16 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ CRC16 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ CRC16 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ CRC16 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ CRC16 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ CRC16 : crc >> 1;
		crc = crc & 1 ? (crc >> 1) ^ CRC16 : crc >> 1;
	}

	// efficient bit reversal
	crc = (((crc & 0xaaaa) >> 1) | ((crc & 0x5555) << 1));
	crc = (((crc & 0xcccc) >> 2) | ((crc & 0x3333) << 2));
	crc = (((crc & 0xf0f0) >> 4) | ((crc & 0x0f0f) << 4));
	crc = (((crc & 0xff00) >> 8) | ((crc & 0x00ff) << 8));

	return crc;
}
volatile bit SMB_RW;
volatile bit SMB_BUSY;

uint8_t SMB_DATA_IN;                   // Global holder for SMBus data
                                       // All receive data is written here

uint8_t SMB_DATA_OUT;                  // Global holder for SMBus data.
                                       // All transmit data is read from here

uint8_t TARGET;                        // Target SMBus slave address
uint8_t NUM_ERRORS;                        // Target SMBus slave address


void SMB_Read (uint8_t addr, uint8_t* dest, uint8_t count)
{
   while(SMB_BUSY != 0);               // Wait for transfer to complete

   SMB_BUSY = 1;                       // Claim SMBus (set to busy)

   SMB_RW = 1;                         // Mark this transfer as a READ

   SMB_READ_OFFSET = 0;
   TARGET = addr;
   SMB_READ_LEN = count;
   SMB_READ_BUF = dest;
   SMB0CN0_STA = 1;                    // Start transfer


   while(SMB_BUSY);                    // Wait for transfer to complete
}


void SMB_Write (uint8_t addr, uint8_t* buf, uint8_t len)
{
   while(SMB_BUSY);                    // Wait for SMBus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 0;                         // Mark this transfer as a WRITE
   SMB_WRITE_BUF_LEN = len;
   SMB_WRITE_BUF = buf;
   SMB_WRITE_BUF_OFFSET = 0;
   TARGET = addr;
   SMB0CN0_STA = 1;                    // Start transfer

}

void send_508_command(uint8_t cmd, uint8_t p1, uint16_t p2,
					uint8_t * buf, uint8_t len)
{
	static uint8_t params[8];
	uint16_t crc;
	params[0] = 0x3;
	params[1] = 7;
	params[2] = cmd;
	params[3] = p1;
	params[4] = ((uint8_t*)&p2)[1];
	params[5] = ((uint8_t*)&p2)[0];
	crc = crc16(params+1,5);
	params[6] = ((uint8_t*)&crc)[1];
	params[7] = ((uint8_t*)&crc)[0];

	SMB_Write( 0xc0, params, sizeof(params) );
}

void test_i2c2()
{
	uint8_t i;
	uint8_t buf[10];

	flush_messages();

	memset(buf,0,sizeof(buf));

	send_508_command(0x24,0,1,NULL,0);
	SMB_Read( 0xc0,buf,10);

	flush_messages();
	for (i=0 ; i < 10 ; i++)
	{
		u2f_print(" %02bx",buf[i]);
		flush_messages();
	}
	u2f_print("\r\n");
	flush_messages();

}

/*
void test_i2c()
{
	int i;
	uint8_t rbuf[4];
	uint8_t params[] = {5,0x30,0,0,0};
	uint8_t numbytes = 4;
	uint16_t crc;
	// write

	u2f_print("1 packet\r\n");
	flush_messages();

	SMB0CN0_STA = 1;
	SMB0CN0_STA = 0;  // clear start
	//while (!SMB0CN0_STA){}

	u2f_print("2 packet\r\n");
	flush_messages();

	SMB0DAT = ECC508_ADDR | I2C_WRITE;

	while(!SMB0CN0_ACK){}

	u2f_print("3 packet\r\n");
	flush_messages();

	SMB0DAT = 0x3;
	while(!SMB0CN0_ACK){}

	u2f_print("wat packet\r\n");
	flush_messages();
	// repeat data/acks for each byte

	for (i=0; i<sizeof(params); i++)
	{
		SMB0DAT = params[i];
		while(!SMB0CN0_ACK){}
	}

	crc = crc16(params, 5);

	SMB0DAT = (uint8_t)crc;
	while(!SMB0CN0_ACK){}
	SMB0DAT = crc >> 8;
	while(!SMB0CN0_ACK){}


	SMB0CN0_STO = 1;
	while (!SMB0CN0_STO){}

	u2f_print("wrote packet\r\n");
	flush_messages();

	// read
	SMB0CN0_STA = 1;
	while (!SMB0CN0_STA){}
	SMB0DAT = ECC508_ADDR | I2C_READ;
	while(!SMB0CN0_ACK){}
	SMB0DAT = 0x3;
	while(!SMB0CN0_ACK){}

	for (i=0; i < numbytes + 2; i++)
	{
		while(RX_EMPTY){}
		rbuf[i] = SMB0DAT;
		SMB0CN0_ACK = 1;
	}
	while (!SMB0CN0_STO){}

	 u2f_print("got packet\r\n");
	 flush_messages();
	 //u2f_print("");
}
*/

int16_t main(void) {

	data uint8_t i = 0;
	data uint16_t last_ms = get_ms();
	data uint16_t ms_heart;
	data uint16_t ms_wink;
	uint8_t winks = 0;
	uint8_t test = 0;


	init(&appdata);

	enter_DefaultMode_from_RESET();

	// STDIO library requires TI to print
	SCON0_TI = 1;

	// Enable interrupts
	IE_EA = 1;
	   SMB0CF  &= ~0x80;                   // Disable SMBus
	   SMB0CF  |=  0x80;                   // Re-enable SMBus
	   TMR3CN0 &= ~0x80;                   // Clear Timer3 interrupt-pending flag
	   SMB0CN0_STA = 0;

	u2f_print("U2F ZERO\r\n");
	wakeup_i2c_slave();
	NUM_ERRORS = 0;

	while (1) {

		if (!test)
		{
			test_i2c2();
			test = 1;
		}

		if (ms_since(ms_heart,500))
		{
			u2f_print("ms %lu\r\n", get_ms());
			LED_G = !LED_G;

		}


		if ( USBD_GetUsbState() == USBD_STATE_CONFIGURED)
		{
			if (!USBD_EpIsBusy(EP1OUT))
			{
				listen_for_pkt(&appdata);
				u2f_print("read added\r\n");
			}

		}



		switch(appdata.state)
		{
			case APP_NOTHING:
				break;
			case APP_WINK:
				LED_B = 0;
				ms_wink = get_ms();
				appdata.state = _APP_WINK;
				break;
			case _APP_WINK:
				if (ms_since(ms_wink,150))
				{
					LED_B = !LED_B;
					winks++;
				}
				if (winks == 5)
				{
					winks = 0;
					appdata.state = APP_NOTHING;
				}

				break;
		}

#ifdef U2F_PRINT
		flush_messages();
#endif

	}
}


