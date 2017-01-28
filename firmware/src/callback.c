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

 */
#include <SI_EFM8UB1_Register_Enums.h>
#include <efm8_usb.h>
#include <stdio.h>
#include "app.h"
#include "bsp.h"
#include "descriptors.h"
#include "u2f_hid.h"

#define UNUSED(expr) do { (void)(expr); } while (0)

#define HID_INTERFACE_INDEX 0

uint8_t tmpBuffer;

//#define PRINT_EVENTS

#ifdef PRINT_EVENTS

#define u2f_print_ev(s) u2f_prints(s)

#else
#define u2f_print_ev(x)
#endif


void USBD_ResetCb(void) {
	u2f_print_ev("USBD_ResetCb\r\n");
}


void USBD_DeviceStateChangeCb(USBD_State_TypeDef oldState,
		USBD_State_TypeDef newState) {

	UNUSED(oldState);
	UNUSED(newState);

	u2f_print_ev("USBD_DeviceStateChangeCb\r\n");
}

bool USBD_IsSelfPoweredCb(void) {
	return false;
}

// Necessary routine for USB HID
USB_Status_TypeDef USBD_SetupCmdCb(
		SI_VARIABLE_SEGMENT_POINTER(setup, USB_Setup_TypeDef, MEM_MODEL_SEG)) {

	USB_Status_TypeDef retVal = USB_STATUS_REQ_UNHANDLED;


	if ((setup->bmRequestType.Type == USB_SETUP_TYPE_STANDARD)
			&& (setup->bmRequestType.Direction == USB_SETUP_DIR_IN)
			&& (setup->bmRequestType.Recipient == USB_SETUP_RECIPIENT_INTERFACE)) {
		// A HID device must extend the standard GET_DESCRIPTOR command
		// with support for HID descriptors.

		switch (setup->bRequest) {
		case GET_DESCRIPTOR:
			if (setup->wIndex == 0)
			{
				if ((setup->wValue >> 8) == USB_HID_REPORT_DESCRIPTOR) {

						USBD_Write(EP0, ReportDescriptor0,
								EFM8_MIN(sizeof(ReportDescriptor0), setup->wLength),
								false);
						retVal = USB_STATUS_OK;

				} else if ((setup->wValue >> 8) == USB_HID_DESCRIPTOR) {

						USBD_Write(EP0, (&configDesc[18]),
								EFM8_MIN(USB_HID_DESCSIZE, setup->wLength), false);
						retVal = USB_STATUS_OK;

				}
			}
			break;
		}
	}
	else if ((setup->bmRequestType.Type == USB_SETUP_TYPE_CLASS)
	           && (setup->bmRequestType.Recipient == USB_SETUP_RECIPIENT_INTERFACE)
	           && (setup->wIndex == HID_INTERFACE_INDEX))
	  {
	    // Implement the necessary HID class specific commands.
	    switch (setup->bRequest)
	    {
	      case USB_HID_SET_IDLE:
	        if (((setup->wValue & 0xFF) == 0)             // Report ID
	            && (setup->wLength == 0)
	            && (setup->bmRequestType.Direction != USB_SETUP_DIR_IN))
	        {
	          retVal = USB_STATUS_OK;
	        }
	        break;

	      case USB_HID_GET_IDLE:
	        if ((setup->wValue == 0)                      // Report ID
	            && (setup->wLength == 1)
	            && (setup->bmRequestType.Direction == USB_SETUP_DIR_IN))
	        {
	          tmpBuffer = 24;
	          USBD_Write(EP0, &tmpBuffer, 1, false);
	          retVal = USB_STATUS_OK;
	        }
	        break;
	      default:
	    	  break;
	    }
	  }

	return retVal;
}




uint8_t hidmsgbuf[64];
uint16_t USBD_XferCompleteCb(uint8_t epAddr, USB_Status_TypeDef status,
		uint16_t xferred, uint16_t remaining ) {

	UNUSED(status);
	UNUSED(xferred);
	UNUSED(remaining);


	if (epAddr == EP1OUT)
	{
		set_app_u2f_hid_msg((struct u2f_hid_msg *) hidmsgbuf );
	}
	return 0;
}


