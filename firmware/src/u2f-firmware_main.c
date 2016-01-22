//-----------------------------------------------------------------------------
// main.c
//-----------------------------------------------------------------------------
// Copyright 2014 Silicon Laboratories, Inc.
// http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
//
// Program Description:
//
// This program enumerates as a USB keyboard. Each time a button is pressed
// a character is sent to the host. A series of presses will spell out
// "HID Keyboard ". The status of the Caps Lock and Num Lock on the host will
// be indicated by the color of the LED.
//
// Resources:
// SYSCLK - 48 MHz HFOSC1 / 1
// USB0   - Full speed
// P0.2 - push button
// P0.3 - push button
// P2.3 - Display enable
//
//-----------------------------------------------------------------------------
// How To Test: EFM8UB1 STK
//-----------------------------------------------------------------------------
// 1) Place the switch in "AEM" mode.
// 2) Connect the EFM8UB1 STK board to a PC using a mini USB cable.
// 3) Compile and download code to the EFM8UB1 STK board.
//    In Simplicity Studio IDE, select Run -> Debug from the menu bar,
//    click the Debug button in the quick menu, or press F11.
// 4) Run the code.
//    In Simplicity Studio IDE, select Run -> Resume from the menu bar,
//    click the Resume button in the quick menu, or press F8.
// 5) The HID keyboard demo should start.
// 6) Connect a micro USB cable from the PC to the STK.
// 7) The device should enumerate on the PC as a HID keyboard.
// 8) Press either push-button (PB0 or PB1) to send one character in the
//    string "HID Keyboard ".
// 9) Pressing Caps Lock or Num Lock on the host keyboard will change the color
//    of the LED.
//
// Target:         EFM8UB1
// Tool chain:     Generic
//
// Release 0.1 (JM)
//    - Initial Revision
//    - 26 JAN 2015
//
#include <SI_EFM8UB1_Register_Enums.h>
#include <stdio.h>

#include "efm8_usb.h"
#include "usb_0.h"
#include "descriptors.h"
#include "idle.h"
#include "bsp.h"
#include "InitDevice.h"

uint8_t keySeqNo = 0;        // Current position in report table.
bool keyPushed = 0;          // Current pushbutton status.

void Delay(int16_t ms) {
	int16_t x;
	int16_t y;
	for (y = 0; y < ms; y++) {
		for (x = 0; x < 1000; x) {
			x++;
		}
	}
}

int16_t main(void) {
	int i = 0;

	enter_DefaultMode_from_RESET();

	SCON0_TI = 1;                       // This STDIO library requires TI to
										// be set for prints to occur
	printf("wat \r\n");

	IE_EA = 1;

	printf("welcome \r\n");

	while (1) {
		++i;

		printf("%d\r\n", i);
		Delay(1000);

	}
}

SI_INTERRUPT(PMATCH_IrqHandler, PMATCH_IRQn) {
	keyPushed = 1;
}
