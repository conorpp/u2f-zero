/**************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/
#if 0
#include <stdio.h>

#include "SI_EFM8UB1_Register_Enums.h"
#include "efm8_usb.h"
#include "descriptors.h"
#include "idle.h"
#include "bsp.h"

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
SI_SBIT(BSP_LED_G, SFR_P1, 4);
SI_SBIT(BSP_LED_B, SFR_P1, 5);
SI_SBIT(BSP_LED_R, SFR_P1, 6);

#define numLock     BSP_LED_G
#define capsLock    BSP_LED_R

// Endpoint address of the HID keyboard IN endpoint
#define KEYBOARD_IN_EP_ADDR   EP1IN

// Interface number of the HID keyboard
#define HID_KEYBOARD_IFC                  0

// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------
uint8_t tmpBuffer;
extern uint8_t keySeqNo;                // Current position in report table.
extern bool keyPushed;                  // Current pushbutton status.


uint8_t thebuf[32];
// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------
void USBD_EnterHandler(void)
{

	// printf("USBD_EnterHandler\r\n");
}

void USBD_ExitHandler(void)
{
	// printf("USBD_ExitHandler\r\n");
}

void USBD_ResetCb(void)
{
	printf("USBD_ResetCb\r\n");
}

void USBD_SofCb(uint16_t sofNr)
{


}

void USBD_DeviceStateChangeCb(USBD_State_TypeDef oldState,
                              USBD_State_TypeDef newState)
{
  bool numLockSave, capsLockSave;

  printf("USBD_DeviceStateChangeCb\r\n");

  // If not configured or in suspend, disable the LED
  if (newState < USBD_STATE_SUSPENDED)
  {
    // Disable the LED
    numLockSave = 1;
    capsLockSave = 1;
    numLock = 1;
    capsLock = 1;
  }
  // Entering suspend mode, power internal and external blocks down
  else if (newState == USBD_STATE_SUSPENDED)
  {
    // Disable the LED's
    numLockSave = numLock;
    capsLockSave = capsLock;
    numLock = 1;
    capsLock = 1;

    // Abort any pending transfer
    USBD_AbortTransfer(KEYBOARD_IN_EP_ADDR);
  }
  else if (newState == USBD_STATE_CONFIGURED)
  {
    idleTimerSet(POLL_RATE);
  }

  // Exiting suspend mode, power internal and external blocks up
  if (oldState == USBD_STATE_SUSPENDED)
  {
    // Restore the LED's to their previous values
    numLock = numLockSave;
    capsLock = capsLockSave;
  }
}

bool USBD_IsSelfPoweredCb(void)
{
  printf("USBD_IsSelfPoweredCb\r\n");
  return false;
}

void print_setup_packet(USB_Setup_TypeDef * setup)
{
	  uint16_t r = (uint8_t)setup->bmRequestType.Recipient;
	  uint16_t t = (uint8_t)setup->bmRequestType.Type;
	  uint16_t d = (uint8_t)setup->bmRequestType.Direction;
	  uint16_t req = (uint8_t)setup->bRequest;
	  uint16_t i = setup->wIndex;
	  uint16_t l = setup->wLength;
	  uint16_t val = setup->wValue;

	  printf("setup packet\r\n"
			  "	   direction 0x%x\r\n"
			  "    type 0x%x\r\n"
			  "    recip inter 0x%x\r\n"
			  "    bRequest 0x%x\r\n"
			  "    value 0x%x\r\n"
			  "    index 0x%x\r\n"
			  "    length 0x%x\r\n"
			  ,d
			  ,t
			  ,r
			  , req
			  , val
			  , i
			  , l

			  );
}

USB_Status_TypeDef USBD_SetupCmdCb(SI_VARIABLE_SEGMENT_POINTER(
                                     setup,
                                     USB_Setup_TypeDef,
                                     MEM_MODEL_SEG))
{

  USB_Status_TypeDef retVal = USB_STATUS_REQ_UNHANDLED;

  // print_setup_packet(setup);


  if ((setup->bmRequestType.Type == USB_SETUP_TYPE_STANDARD)
      && (setup->bmRequestType.Direction == USB_SETUP_DIR_IN)
      && (setup->bmRequestType.Recipient == USB_SETUP_RECIPIENT_INTERFACE))
  {
    // A HID device must extend the standard GET_DESCRIPTOR command
    // with support for HID descriptors.
    switch (setup->bRequest)
    {
      case GET_DESCRIPTOR:
        if ((setup->wValue >> 8) == USB_HID_REPORT_DESCRIPTOR)
        {
          switch (setup->wIndex)
          {
            case 0: // Interface 0

              USBD_Write(EP0,
                         ReportDescriptor0,
                         EFM8_MIN(sizeof(ReportDescriptor0), setup->wLength),
                         false);
              retVal = USB_STATUS_OK;
              printf("USBD_SetupCmdCb report D\r\n");
              break;

            default: // Unhandled Interface
            	printf("USBD_SetupCmdCb unhandled get descriptor %d\r\n", setup->wIndex);
              break;
          }
        }
        else if ((setup->wValue >> 8) == USB_HID_DESCRIPTOR)
        {
          switch (setup->wIndex)
          {
            case 0: // Interface 0

              USBD_Write(EP0,
                         (&configDesc[18]),
                         EFM8_MIN(USB_HID_DESCSIZE, setup->wLength),
                         false);
              retVal = USB_STATUS_OK;
              printf("get hid descsize\r\n");
              break;

            default: // Unhandled Interface
            	printf("Unhandled Interface\r\n");
              break;
          }
        }
        else
        {
        	printf("unhandled set up value %d\r\n", setup->wValue);
        }
        break;
      default:
    	  printf("USBD_SetupCmdCb setup->bRequest %d\r\n", setup->bRequest);
    	  break;
    }
  }
  else if ((setup->bmRequestType.Type == USB_SETUP_TYPE_CLASS)
           && (setup->bmRequestType.Recipient == USB_SETUP_RECIPIENT_INTERFACE)
           && (setup->wIndex == HID_KEYBOARD_IFC))
  {
    // Implement the necessary HID class specific commands.
    switch (setup->bRequest)
    {
      case USB_HID_SET_REPORT:
    	  printf("output report\r\n");
        break;

      case USB_HID_GET_REPORT:
    	  printf("input report\r\n");


        break;

      case USB_HID_SET_IDLE:
        if (((setup->wValue & 0xFF) == 0)             // Report ID
            && (setup->wLength == 0)
            && (setup->bmRequestType.Direction != USB_SETUP_DIR_IN))
        {
        	printf("set idle\r\n");
          idleTimerSet(setup->wValue >> 8);
          retVal = USB_STATUS_OK;
        }
        else printf("unhandled USB_HID_SET_IDLE\r\n");
        break;

      case USB_HID_GET_IDLE:
        if ((setup->wValue == 0)                      // Report ID
            && (setup->wLength == 1)
            && (setup->bmRequestType.Direction == USB_SETUP_DIR_IN))
        {
        	printf("get idle\r\n");
          tmpBuffer = idleGetRate();
          USBD_Write(EP0, &tmpBuffer, 1, false);
          retVal = USB_STATUS_OK;
        }
        else printf("unhandled USB_HID_GET_IDLE\r\n");
        break;
      default:
    	  printf("unhandled setup->bRequest\r\n");
    }
  }
  else
  {
	  if (setup->bmRequestType.Recipient == USB_SETUP_RECIPIENT_ENDPOINT)
		  printf("endpoint called!\n");
	  // printf("NOT HANDLED\r\n");
  }

  return retVal;
}



uint16_t USBD_XferCompleteCb(uint8_t epAddr,
                             USB_Status_TypeDef status,
                             uint16_t xferred,
                             uint16_t remaining)
{

	int i = 0;
	UNREFERENCED_ARGUMENT(xferred);
  UNREFERENCED_ARGUMENT(remaining);

  printf("USBD_XferCompleteCb\r\n");
  for (i=0; i < 32 ; i++)
  {

	  printf("%hhx", thebuf[i]);

  }
  printf("\r\n");

  if (status == USB_STATUS_OK)
  {
    // The only output reported supported is the SetReport to enable
    // Num Key and Caps Lock LED's.
    if (epAddr == EP0)
    {
      numLock = !((bool) (tmpBuffer & 0x01));
      capsLock = !((bool) (tmpBuffer & 0x02));
    }
  }
  // USBD_Read(EP0, thebuf, 32, true);
  return 0;
}
#endif
