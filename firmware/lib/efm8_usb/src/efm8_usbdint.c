/**************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "si_toolchain.h"
#include "efm8_usb.h"
#include <stdint.h>
#include <endian.h>

// -----------------------------------------------------------------------------
// Global variables

extern SI_SEGMENT_VARIABLE(myUsbDevice, USBD_Device_TypeDef, MEM_MODEL_SEG);
extern SI_SEGMENT_VARIABLE(txZero[2], uint8_t, SI_SEG_CODE);

// -----------------------------------------------------------------------------
// Function prototypes

static void handleUsbEp0Int(void);
static void handleUsbResetInt(void);
static void handleUsbSuspendInt(void);
static void handleUsbResumeInt(void);
static void handleUsbEp0Tx(void);
static void handleUsbEp0Rx(void);
static void USB_ReadFIFOSetup(void);

#if (SLAB_USB_EP1IN_USED)
void handleUsbIn1Int(void);
#endif // SLAB_USB_EP1IN_USED
#if (SLAB_USB_EP2IN_USED)
void handleUsbIn2Int(void);
#endif // SLAB_USB_EP2IN_USED
#if (SLAB_USB_EP3IN_USED)
void handleUsbIn3Int(void);
#endif // SLAB_USB_EP3IN_USED

#if (SLAB_USB_EP1OUT_USED)
void handleUsbOut1Int(void);
#endif // SLAB_USB_EP1OUT_USED
#if (SLAB_USB_EP2OUT_USED)
void handleUsbOut2Int(void);
#endif // SLAB_USB_EP2OUT_USED
#if (SLAB_USB_EP3OUT_USED)
void handleUsbOut3Int(void);
#endif // SLAB_USB_EP3OUT_USED

void SendEp0Stall(void);

// -----------------------------------------------------------------------------
// Functions

/***************************************************************************//**
 * @brief       First-level handler for USB peripheral interrupt
 * @details     If @ref SLAB_USB_POLLED_MODE is 1, this becomes a regular
 *              function instead of an ISR and must be called by the application
 *              periodically.
 ******************************************************************************/
#if (SLAB_USB_POLLED_MODE == 0)
SI_INTERRUPT(usbIrqHandler, USB0_IRQn)
#else
void usbIrqHandler(void)
#endif
{
  uint8_t statusCommon, statusIn, statusOut, indexSave;

#if SLAB_USB_HANDLER_CB
  // Callback to user before processing
  USBD_EnterHandler();
#endif

  // Get the interrupt sources
  statusCommon = USB_GetCommonInts();
  statusIn = USB_GetInInts();
  statusOut = USB_GetOutInts();

#if SLAB_USB_POLLED_MODE
  if ((statusCommon == 0) && (statusIn == 0) && (statusOut == 0))
  {
    return;
  }
#endif

  // Save the current index
  indexSave = USB_GetIndex();

  // Check Common USB Interrupts
  if (USB_IsSofIntActive(statusCommon))
  {
#if SLAB_USB_SOF_CB
    USBD_SofCb(USB_GetSofNumber());
#endif // SLAB_USB_SOF_CB

    // Check for unhandled USB packets on EP0 and set the corresponding IN or
    // OUT interrupt active flag if necessary.
    if (((myUsbDevice.ep0.misc.bits.outPacketPending == true) && (myUsbDevice.ep0.state == D_EP_RECEIVING)) ||
        ((myUsbDevice.ep0.misc.bits.inPacketPending == true) && (myUsbDevice.ep0.state == D_EP_TRANSMITTING)))
    {
      USB_SetEp0IntActive(statusIn);
    }
    // Check for unhandled USB OUT packets and set the corresponding OUT
    // interrupt active flag if necessary.
#if SLAB_USB_EP1OUT_USED
    if ((myUsbDevice.ep1out.misc.bits.outPacketPending == true) && (myUsbDevice.ep1out.state == D_EP_RECEIVING))
    {
      USB_SetOut1IntActive(statusOut);
    }
#endif
#if SLAB_USB_EP2OUT_USED
    if ((myUsbDevice.ep2out.misc.bits.outPacketPending == true) && (myUsbDevice.ep2out.state == D_EP_RECEIVING))
    {
      USB_SetOut2IntActive(statusOut);
    }
#endif
#if SLAB_USB_EP3OUT_USED
    if ((myUsbDevice.ep3out.misc.bits.outPacketPending == true) && (myUsbDevice.ep3out.state == D_EP_RECEIVING))
    {
      USB_SetOut3IntActive(statusOut);
    }
#endif
#if (SLAB_USB_EP3IN_USED && (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC))
    if ((myUsbDevice.ep3in.misc.bits.inPacketPending == true) && (myUsbDevice.ep3in.state == D_EP_TRANSMITTING))
    {
      USB_SetIn3IntActive(statusIn);
    }
#endif
  }

  if (USB_IsResetIntActive(statusCommon))
  {
    handleUsbResetInt();

    // If VBUS is not present on detection of a USB reset, enter suspend mode.
#if (SLAB_USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONVBUSOFF)
    if (USB_IsVbusOn() == false)
    {
      USB_SetSuspendIntActive(statusCommon);
    }
#endif
  }

  if (USB_IsResumeIntActive(statusCommon))
  {
    handleUsbResumeInt();
  }

  if (USB_IsSuspendIntActive(statusCommon))
  {
    handleUsbSuspendInt();
  }

#if SLAB_USB_EP3IN_USED
  if (USB_IsIn3IntActive(statusIn))
  {
    handleUsbIn3Int();
  }
#endif  // EP3IN_USED

#if SLAB_USB_EP3OUT_USED
  if (USB_IsOut3IntActive(statusOut))
  {
    handleUsbOut3Int();
  }
#endif  // EP3OUT_USED

#if SLAB_USB_EP2IN_USED
  if (USB_IsIn2IntActive(statusIn))
  {
    handleUsbIn2Int();
  }
#endif  // EP2IN_USED

#if SLAB_USB_EP1IN_USED
  if (USB_IsIn1IntActive(statusIn))
  {
    handleUsbIn1Int();
  }
#endif  // EP1IN_USED

#if SLAB_USB_EP2OUT_USED
  if (USB_IsOut2IntActive(statusOut))
  {
    handleUsbOut2Int();
  }
#endif  // EP2OUT_USED

#if SLAB_USB_EP1OUT_USED
  if (USB_IsOut1IntActive(statusOut))
  {
    handleUsbOut1Int();
  }
#endif  // EP1OUT_USED

  // Check USB Endpoint 0 Interrupt
  if (USB_IsEp0IntActive(statusIn))
  {
    handleUsbEp0Int();
  }

  // Restore index
  USB_SetIndex(indexSave);

#if SLAB_USB_HANDLER_CB
  // Callback to user before exiting
  USBD_ExitHandler();
#endif
}

/***************************************************************************//**
 * @brief       Handles Endpoint 0 transfer interrupt
 ******************************************************************************/
static void handleUsbEp0Int(void)
{
  USB_SetIndex(0);

  if (USB_Ep0SentStall() || USB_GetSetupEnd())
  {
    USB_Ep0ClearSentStall();
    USB_ServicedSetupEnd();
    myUsbDevice.ep0.state = D_EP_IDLE;
    myUsbDevice.ep0.misc.c = 0;
  }
  if (USB_Ep0OutPacketReady())
  {
    if (myUsbDevice.ep0.misc.bits.waitForRead == true)
    {
      myUsbDevice.ep0.misc.bits.outPacketPending = true;
    }
    else if (myUsbDevice.ep0.state == D_EP_IDLE)
    {
      myUsbDevice.ep0String.c = USB_STRING_DESCRIPTOR_UTF16LE;
      USB_ReadFIFOSetup();

      // Vendor unique, Class or Standard setup commands override?
#if SLAB_USB_SETUP_CMD_CB
      if (USBD_SetupCmdCb(&myUsbDevice.setup) == USB_STATUS_REQ_UNHANDLED)
      {
#endif
      if (myUsbDevice.setup.bmRequestType.Type == USB_SETUP_TYPE_STANDARD)
      {
        USBDCH9_SetupCmd();
      }
      else
      {
        SendEp0Stall();
      }
#if SLAB_USB_SETUP_CMD_CB
    }
    else
    {
      // If in-packet but callback didn't setup a USBD_Read and we are expecting a data byte then
      // we need to wait for the read to be setup and nack packets till USBD_Read is called.
      if ((myUsbDevice.setup.bmRequestType.Direction == USB_SETUP_DIR_OUT)
          && (myUsbDevice.ep0.state != D_EP_RECEIVING)
          && (myUsbDevice.setup.wLength)
          )
      {
        myUsbDevice.ep0.misc.bits.waitForRead = true;
      }
    }
#endif
    }
    else if (myUsbDevice.ep0.state == D_EP_RECEIVING)
    {
      handleUsbEp0Rx();
    }
    else
    {
      myUsbDevice.ep0.misc.bits.outPacketPending = true;
    }
  }
  if ((myUsbDevice.ep0.state == D_EP_TRANSMITTING) && (USB_Ep0InPacketReady() == 0))
  {
    handleUsbEp0Tx();
  }
}

/***************************************************************************//**
 * @brief       Reads and formats a setup packet
 ******************************************************************************/
static void USB_ReadFIFOSetup(void)
{
  uint16_t MEM_MODEL_SEG *ptr = &myUsbDevice.setup;

  USB_ReadFIFO(0, 8, (uint8_t *)ptr);

  USB_Ep0ServicedOutPacketReady();

  // Modify for Endian-ness of the compiler
  ptr[1] = le16toh(ptr[1]);
  ptr[2] = le16toh(ptr[2]);
  ptr[3] = le16toh(ptr[3]);
}

/***************************************************************************//**
 * @brief       Handles USB port reset interrupt
 * @details     After receiving a USB reset, halt all endpoints except for
 *              Endpoint 0, set the device state, and configure USB hardware.
 ******************************************************************************/
static void handleUsbResetInt(void)
{
  // Setup EP0 to receive SETUP packets
  myUsbDevice.ep0.state = D_EP_IDLE;

  // Halt all other endpoints
#if SLAB_USB_EP1IN_USED
  myUsbDevice.ep1in.state = D_EP_HALT;
#endif
#if SLAB_USB_EP2IN_USED
  myUsbDevice.ep2in.state = D_EP_HALT;
#endif
#if SLAB_USB_EP3IN_USED
  myUsbDevice.ep3in.state = D_EP_HALT;
#endif
#if SLAB_USB_EP1OUT_USED
  myUsbDevice.ep1out.state = D_EP_HALT;
#endif
#if SLAB_USB_EP2OUT_USED
  myUsbDevice.ep2out.state = D_EP_HALT;
#endif
#if SLAB_USB_EP3OUT_USED
  myUsbDevice.ep3out.state = D_EP_HALT;
#endif

  // After a USB reset, some USB hardware configurations will be reset and must
  // be reconfigured.

  // Re-enable clock recovery
#if SLAB_USB_CLOCK_RECOVERY_ENABLED
#if SLAB_USB_FULL_SPEED
  USB_EnableFullSpeedClockRecovery();
#else
  USB_EnableLowSpeedClockRecovery();
#endif
#endif

  // Re-enable USB interrupts
  USB_EnableSuspendDetection();
  USB_EnableDeviceInts();

  // If VBUS is preset, put the device in the Default state.
  // Otherwise, put it in the Attached state.
#if (!(SLAB_USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONVBUSOFF))
  if (USB_IsVbusOn())
  {
    USBD_SetUsbState(USBD_STATE_DEFAULT);
  }
  else
  {
    USBD_SetUsbState(USBD_STATE_ATTACHED);
  }
#else
  USBD_SetUsbState(USBD_STATE_DEFAULT);
#endif  // (!(SLAB_USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONVBUSOFF))

#if SLAB_USB_RESET_CB
  // Make the USB Reset Callback
  USBD_ResetCb();
#endif
}

/***************************************************************************//**
 * @brief       Handle USB port suspend interrupt
 * @details     After receiving a USB reset, set the device state and
 *              call @ref USBD_Suspend() if configured to do so in
 *              @ref SLAB_USB_PWRSAVE_MODE
 ******************************************************************************/
static void handleUsbSuspendInt(void)
{
  if (myUsbDevice.state >= USBD_STATE_POWERED)
  {
    USBD_SetUsbState(USBD_STATE_SUSPENDED);

#if (SLAB_USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONSUSPEND)
    USBD_Suspend();
#endif
  }
}

/***************************************************************************//**
 * @brief       Handles USB port resume interrupt
 * @details     Restore the device state to its previous value.
 ******************************************************************************/
static void handleUsbResumeInt(void)
{
  USBD_SetUsbState(myUsbDevice.savedState);
}

/***************************************************************************//**
 * @brief       Handles transmit data phase on Endpoint 0
 ******************************************************************************/
static void handleUsbEp0Tx(void)
{
  uint8_t count, count_snapshot, i;
  bool callback = myUsbDevice.ep0.misc.bits.callback;

  // The number of bytes to send in the next packet must be less than or equal
  // to the maximum EP0 packet size.
  count = (myUsbDevice.ep0.remaining >= USB_EP0_SIZE) ?
           USB_EP0_SIZE : myUsbDevice.ep0.remaining;

  // Save the packet size for future use.
  count_snapshot = count;

  // Strings can use the USB_STRING_DESCRIPTOR_UTF16LE_PACKED type to pack
  // UTF16LE data without the zero's between each character.
  // If the current string is of type USB_STRING_DESCRIPTOR_UTF16LE_PACKED,
  // unpacket it by inserting a zero between each character in the string.
  if (myUsbDevice.ep0String.encoding.type == USB_STRING_DESCRIPTOR_UTF16LE_PACKED)
  {
    // If ep0String.encoding.init is true, this is the beginning of the string.
    // The first two bytes of the string are the bLength and bDescriptorType
    // fields. These are no packed like the reset of the string, so write them
    // to the FIFO and set ep0String.encoding.init to false.
    if (myUsbDevice.ep0String.encoding.init == true)
    {
      USB_WriteFIFO(0, 2, myUsbDevice.ep0.buf, false);
      myUsbDevice.ep0.buf += 2;
      count -= 2;
      myUsbDevice.ep0String.encoding.init = false;
    }

    // Insert a 0x00 between each character of the string.
    for (i = 0; i < count / 2; i++)
    {
      USB_WriteFIFO(0, 1, myUsbDevice.ep0.buf, false);
      myUsbDevice.ep0.buf++;
      USB_WriteFIFO(0, 1, &txZero, false);
    }
  }
  // For any data other than USB_STRING_DESCRIPTOR_UTF16LE_PACKED, just send the
  // data normally.
  else
  {
    USB_WriteFIFO(0, count, myUsbDevice.ep0.buf, false);
    myUsbDevice.ep0.buf += count;
  }

  myUsbDevice.ep0.misc.bits.inPacketPending = false;
  myUsbDevice.ep0.remaining -= count_snapshot;

  // If the last packet of the transfer is exactly the maximum EP0 packet size,
  // we will have to send a ZLP (zero-length packet) after the last data packet
  // to signal to the host that the transfer is complete.
  // Check for the ZLP packet case here.
  if ((myUsbDevice.ep0.remaining == 0) && (count_snapshot != USB_EP0_SIZE))
  {
    USB_Ep0SetLastInPacketReady();
    myUsbDevice.ep0.state = D_EP_IDLE;
    myUsbDevice.ep0String.c = USB_STRING_DESCRIPTOR_UTF16LE;
    myUsbDevice.ep0.misc.c = 0;
  }
  else
  {
    // Do not call USB_Ep0SetLastInPacketReady() because we still need to send
    // the ZLP.
    USB_Ep0SetInPacketReady();
  }
  // Make callback if requested
  if (callback == true)
  {
    USBD_XferCompleteCb(EP0, USB_STATUS_OK, count_snapshot, myUsbDevice.ep0.remaining);
  }
}

/***************************************************************************//**
 * @brief       Handles receive data phase on Endpoint 0
 ******************************************************************************/
void handleUsbEp0Rx(void)
{
  uint8_t count;
  USB_Status_TypeDef status;
  bool callback = myUsbDevice.ep0.misc.bits.callback;

  // Get the number of bytes received
  count = USB_Ep0GetCount();

  // If the call to USBD_Read() did not give a large enough buffer to hold this
  // data, set the outPacketPending flag and signal an RX overrun.
  if (myUsbDevice.ep0.remaining < count)
  {
    myUsbDevice.ep0.state = D_EP_IDLE;
    myUsbDevice.ep0.misc.bits.outPacketPending = true;
    status = USB_STATUS_EP_RX_BUFFER_OVERRUN;
  }
  else
  {
    USB_ReadFIFO(0, count, myUsbDevice.ep0.buf);
    myUsbDevice.ep0.buf += count;
    myUsbDevice.ep0.remaining -= count;
    status = USB_STATUS_OK;

    // If the last packet of the transfer is exactly the maximum EP0 packet
    // size, we will must wait to receive a ZLP (zero-length packet) after the
    // last data packet. This signals that the host has completed the transfer.
    // Check for the ZLP packet case here.
    if ((myUsbDevice.ep0.remaining == 0) && (count != USB_EP0_SIZE))
    {
      USB_Ep0SetLastOutPacketReady();
      myUsbDevice.ep0.state = D_EP_IDLE;
      myUsbDevice.ep0.misc.bits.callback = false;
    }
    else
    {
      // Do not call USB_Ep0SetLastOutPacketReady() until we get the ZLP.
      USB_Ep0ServicedOutPacketReady();
    }
  }

  // Make callback if requested
  if (callback == true)
  {
    USBD_XferCompleteCb(EP0, status, count, myUsbDevice.ep0.remaining);
  }
}


/***************************************************************************//**
 * @brief       Send a procedural stall on Endpoint 0
 ******************************************************************************/
void SendEp0Stall(void)
{
  USB_SetIndex(0);
  myUsbDevice.ep0.state = D_EP_STALL;
  USB_Ep0SendStall();
}

// This function is called from USBD_Init(). It forces the user project to pull
// this module from the library so that the declared ISR can be seen and
// included. If this is not done then this entire module by never be included
// and the ISR will not be present.
void forceModuleLoad_usbint(void){}
