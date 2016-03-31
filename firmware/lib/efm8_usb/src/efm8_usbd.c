/**************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "si_toolchain.h"
#include "efm8_usb.h"
//#include "assert.h"
#include <stdint.h>
#define SLAB_ASSERT(x)
// -----------------------------------------------------------------------------
// Global Variables

/// Tracks the state of the USB device and endpoints and contains pointers
/// to all descriptors.
SI_SEGMENT_VARIABLE(myUsbDevice, USBD_Device_TypeDef, MEM_MODEL_SEG);

// -----------------------------------------------------------------------------
// Macros

/// Returns the requested endpoint object of type USBD_Ep_TypeDef
/// This macro does not check that epAddr is valid, so the calling function
/// should verify epAddr before using the macro.
#define GetEp(epAddr)  (&myUsbDevice.ep0 + epAddr)


#if SLAB_USB_POLLED_MODE
#define DISABLE_USB_INTS  {}
#define ENABLE_USB_INTS   {}

#else
/// Saves the current state of the USB Interrupt Enable to a variable called
/// usbIntsEnabled, then disables USB interrupts.
#define DISABLE_USB_INTS  { usbIntsEnabled = USB_GetIntsEnabled(); USB_DisableInts(); }

/// Sets the USB Interrupt Enable bit to the value of usbIntsEnabled.
/// @ref DISABLE_USB_INTS must be used before this macro is used.
#define ENABLE_USB_INTS   { if (usbIntsEnabled) {USB_EnableInts(); } }
#endif  // SLAB_USB_POLLED_MODE

// Function in efm8_usbdint.c to force load the module for libraries
extern void forceModuleLoad_usbint(void);

// -----------------------------------------------------------------------------
// USB API Functions

void USBD_AbortAllTransfers(void)
{
  uint8_t i;
  bool usbIntsEnabled;

  USB_SaveSfrPage();
  DISABLE_USB_INTS;

  // Call USBD_AbortTransfer() for each endpoint
  for (i = 1; i < SLAB_USB_NUM_EPS_USED; i++)
  {
    USBD_AbortTransfer(i);
  }

  ENABLE_USB_INTS;
  USB_RestoreSfrPage();
}

int8_t USBD_AbortTransfer(uint8_t epAddr)
{
  USBD_Ep_TypeDef MEM_MODEL_SEG *ep;
  uint8_t retVal = USB_STATUS_OK;
  bool usbIntsEnabled;

  USB_SaveSfrPage();

  // Verify this is a valid endpoint address and is not Endpoint 0.
  if ((epAddr == EP0) || (epAddr >= SLAB_USB_NUM_EPS_USED))
  {
    SLAB_ASSERT(false);
    retVal = USB_STATUS_ILLEGAL;
  }
  else
  {
    DISABLE_USB_INTS;
    ep = GetEp(epAddr);

    // If the state of the endpoint is already idle, there is not need to abort
    // a transfer
    if (ep->state != D_EP_IDLE)
    {
      switch (epAddr)
      {
  #if SLAB_USB_EP1IN_USED
        case EP1IN:
          USB_AbortInEp(1);
          break;
  #endif
  #if SLAB_USB_EP2IN_USED
        case EP2IN:
          USB_AbortInEp(2);
          break;
  #endif
  #if SLAB_USB_EP3IN_USED
        case EP3IN:
          USB_AbortInEp(3);
          break;
  #endif
  #if SLAB_USB_EP1OUT_USED
        case EP1OUT:
          USB_AbortOutEp(1);
          break;
  #endif
  #if SLAB_USB_EP2OUT_USED
        case EP2OUT:
          USB_AbortOutEp(2);
          break;
  #endif
  #if SLAB_USB_EP3OUT_USED
        case EP3OUT:
          USB_AbortOutEp(3);
          break;
  #endif
      }

      // Set the endpoint state to idle and clear out endpoint state variables
      ep->state = D_EP_IDLE;
      ep->misc.c = 0;
    }
  }

  ENABLE_USB_INTS;
  USB_RestoreSfrPage();

  return retVal;
}

void USBD_Connect(void)
{
  USB_SaveSfrPage();
  myUsbDevice.ep0.state = D_EP_IDLE;
  USB_EnablePullUpResistor();
  USB_EnableTransceiver();
  USB_RestoreSfrPage();
}

void USBD_Disconnect(void)
{
  USB_SaveSfrPage();
  USB_DisablePullUpResistor();
  USB_RestoreSfrPage();
}

bool USBD_EpIsBusy(uint8_t epAddr)
{
  USBD_Ep_TypeDef MEM_MODEL_SEG *ep;

  // Verify this is a valid endpoint address
  if (epAddr >= SLAB_USB_NUM_EPS_USED)
  {
    SLAB_ASSERT(false);
    return true;
  }

  ep = GetEp(epAddr);

  if (ep->state == D_EP_IDLE)
  {
    return false;
  }

  return true;
}

USBD_State_TypeDef USBD_GetUsbState(void)
{
  return myUsbDevice.state;
}

int8_t USBD_Init(const USBD_Init_TypeDef *p)
{
  uint8_t i;

  USB_SaveSfrPage();
  USB_DisableInts();

  // This forces the liner to bring in the contents efm8_usbdint
  // It is place here since all users MUST call this function
  // for the library to work properly
  forceModuleLoad_usbint();


  // Zero out the myUsbDevice struct, then initialize all non-zero members
  for (i = 0; i < sizeof(myUsbDevice); i++)
  {
    *((uint8_t MEM_MODEL_SEG *)&myUsbDevice + i) = 0;
  }

  // Get the USB descriptors from p
  myUsbDevice.deviceDescriptor = p->deviceDescriptor;
  myUsbDevice.configDescriptor = (USB_ConfigurationDescriptor_TypeDef *)p->configDescriptor;
  myUsbDevice.stringDescriptors = p->stringDescriptors;
  myUsbDevice.numberOfStrings = p->numberOfStrings;

  // Enable USB clock
#if SLAB_USB_FULL_SPEED
  USB_SetClockIntOsc();
  USB_SelectFullSpeed();
#else
  USB_SetClockIntOscDiv8();
  USB_SelectLowSpeed();
#endif // SLAB_USB_FULL_SPEED

  // Enable or disable VBUS detection
#if SLAB_USB_BUS_POWERED
  USB_VbusDetectDisable();
#else
  USB_VbusDetectEnable();
#endif

  USB_ForceReset();
  USB_EnableDeviceInts();
  USBD_Connect();

  // If VBUS is present, the state should be Default.
  // Otherwise, it is Attached.
#if SLAB_USB_BUS_POWERED
  myUsbDevice.state = USBD_STATE_DEFAULT;
#else
  if (USB_IsVbusOn())
  {
    myUsbDevice.state = USBD_STATE_DEFAULT;
  }
  else
  {
    myUsbDevice.state = USBD_STATE_ATTACHED;
  }
#endif

  // Only enable USB interrupts when not in polled mode
#if (SLAB_USB_POLLED_MODE == 0)
  USB_EnableInts();
#endif

  USB_RestoreSfrPage();
  USB_DisableInhibit();

  return USB_STATUS_OK;
}

int8_t USBD_Read(uint8_t epAddr,
                 uint8_t *dat,
                 uint16_t byteCount,
                 bool callback)
{
  bool usbIntsEnabled;
  USBD_Ep_TypeDef MEM_MODEL_SEG *ep;

  USB_SaveSfrPage();

  // Verify the endpoint address is valid.
  switch (epAddr)
  {
    case EP0:
#if SLAB_USB_EP1OUT_USED
    case EP1OUT:
#endif
#if SLAB_USB_EP2OUT_USED
    case EP2OUT:
#endif
#if SLAB_USB_EP3OUT_USED
    case EP3OUT:
#endif
      break;
#if SLAB_USB_EP1IN_USED
    case EP1IN:
#endif
#if SLAB_USB_EP2IN_USED
    case EP2IN:
#endif
#if SLAB_USB_EP3IN_USED
    case EP3IN:
#endif
    default:
      SLAB_ASSERT(false);
      return USB_STATUS_ILLEGAL;
  }

  // If the device has not been configured, we cannot start a transfer.
  if ((epAddr != EP0) && (myUsbDevice.state != USBD_STATE_CONFIGURED))
  {
    return USB_STATUS_DEVICE_UNCONFIGURED;
  }

  ep = GetEp(epAddr);

  // If the endpoint is not idle, we cannot start a new transfer.
  // Return the appropriate error code.
  if (ep->state != D_EP_IDLE)
  {
    if (ep->state == D_EP_STALL)
    {
      return USB_STATUS_EP_STALLED;
    }
    else
    {
      return USB_STATUS_EP_BUSY;
    }
  }

  DISABLE_USB_INTS;

  ep->buf = dat;
  ep->remaining = byteCount;
  ep->state = D_EP_RECEIVING;
  ep->misc.bits.callback = callback;
  ep->misc.bits.waitForRead = false;

  // If isochronous, set the buffer index to 0
#if ((SLAB_USB_EP3OUT_USED) && (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC))
  if (epAddr == EP3OUT)
  {
    myUsbDevice.ep3outIsoIdx = 0;
  }
#endif

  ENABLE_USB_INTS;
  USB_RestoreSfrPage();

  return USB_STATUS_OK;
}

#if SLAB_USB_REMOTE_WAKEUP_ENABLED
int8_t USBD_RemoteWakeup(void)
{
  // The device must be suspended and Remote Wakeup must have been previously
  // configured with a SET_FEATURE (Remote Wakeup) command.
  if ((myUsbDevice.state != USBD_STATE_SUSPENDED) ||
      (myUsbDevice.remoteWakeupEnabled == false))
  {
    return USB_STATUS_ILLEGAL;
  }

  USB_ForceResume();
  USBD_RemoteWakeupDelay();   // Application will provide the delay between
                              // starting and stopping the resume signal.
  USB_ClearResume();

  return USB_STATUS_OK;
}
#endif // SLAB_USB_REMOTE_WAKEUP_ENABLED

#if SLAB_USB_POLLED_MODE
void USBD_Run(void)
{
  usbIrqHandler();
}
#endif // SLAB_USB_POLLED_MODE

int8_t USBD_StallEp(uint8_t epAddr)
{
  bool usbIntsEnabled;

  USB_SaveSfrPage();

  // Verify the endpoint address is valid and not Endpoint 0.
  if ((epAddr == EP0) || (epAddr >= SLAB_USB_NUM_EPS_USED))
  {
    SLAB_ASSERT(false);
    return USB_STATUS_ILLEGAL;
  }

  DISABLE_USB_INTS;

  // Halt the appropriate endpoint by sending a stall and setting the endpoint
  // state to Halted (D_EP_HALT).
  switch (epAddr)
  {
#if SLAB_USB_EP1IN_USED
    case (EP1IN):
      myUsbDevice.ep1in.state = D_EP_HALT;
      USB_SetIndex(1);
      USB_EpnInStall();
      break;
#endif
#if SLAB_USB_EP2IN_USED
    case (EP2IN):
      myUsbDevice.ep2in.state = D_EP_HALT;
      USB_SetIndex(2);
      USB_EpnInStall();
      break;
#endif
#if SLAB_USB_EP3IN_USED
    case (EP3IN):
      myUsbDevice.ep3in.state = D_EP_HALT;
      USB_SetIndex(3);
      USB_EpnInStall();
      break;
#endif
#if SLAB_USB_EP1OUT_USED
    case (EP1OUT):
      myUsbDevice.ep1out.state = D_EP_HALT;
      USB_SetIndex(1);
      USB_EpnOutStall();
      break;
#endif
#if SLAB_USB_EP2OUT_USED
    case (EP2OUT):
      myUsbDevice.ep2out.state = D_EP_HALT;
      USB_SetIndex(2);
      USB_EpnOutStall();
      break;
#endif
#if SLAB_USB_EP3OUT_USED
    case (EP3OUT):
      myUsbDevice.ep3out.state = D_EP_HALT;
      USB_SetIndex(3);
      USB_EpnOutStall();
      break;
#endif
  }

  ENABLE_USB_INTS;
  USB_RestoreSfrPage();

  return USB_STATUS_OK;
}

void USBD_Stop(void)
{
  USB_DisableInts();
  USBD_Disconnect();
  USBD_SetUsbState(USBD_STATE_NONE);
}

void USBD_Suspend(void)
{
  uint8_t i;
  bool regulatorEnabled, prefetchEnabled;

  USB_SaveSfrPage();

  // If the USB_PWRSAVE_MODE_ONVBUSOFF is enabled, we can enter suspend if VBUS
  // is not present even if the USB has not detected a suspend event.
#if ((!(SLAB_USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONVBUSOFF)) || \
       (SLAB_USB_BUS_POWERED))
  if (USB_IsSuspended() == true)
#else
  if ((USB_IsSuspended() == true) || (USB_IsVbusOn() == false))
#endif
  {
    USB_SuspendTransceiver();

#if SLAB_USB_FULL_SPEED
    USB_SetSuspendClock();
#endif

    // Get the state of the prefetch engine enable bit and disable the prefetch
    // engine
    prefetchEnabled = USB_IsPrefetchEnabled();
    USB_DisablePrefetch();

    // Get the state of the internal regulator before suspending it.
    if (USB_IsRegulatorEnabled() == true)
    {
      regulatorEnabled = true;

#if (SLAB_USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_FASTWAKE)
      USB_SuspendRegulatorFastWake();
#else
      USB_SuspendRegulator();

      // Wait at least 12 clock instructions before halting the internal oscillator
      for (i = 0; i < 3; i++)
      {
      }
#endif
    }
    else
    {
      regulatorEnabled = false;
    }

    do
    {
      USB_SuspendOscillator();

      // When we arrive here, the device has waked from suspend mode.

#if SLAB_USB_REMOTE_WAKEUP_ENABLED
      // If remote wakeup is enabled, query the application if the remote
      // wakeup event occurred. If so, exit USBD_Suspend().
      if (USB_IsSuspended() == true)
      {
        if (USBD_RemoteWakeupCb() == true)
        {
          break;
        }
      }
#endif
#if ((!(SLAB_USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONVBUSOFF)) && \
       (SLAB_USB_BUS_POWERED == 0))
      // If the USB_PWRSAVE_MODE_ONVBUSOFF mode is disabled, VBUS has been
      // removed, so exit USBD_Suspend().
      if (USB_IsVbusOn() == false)
      {
        break;
      }
#endif
    } while (USB_IsSuspended() == true);

    // Restore the internal regulator
    if (regulatorEnabled == true)
    {
      USB_UnsuspendRegulator();
    }

    // Restore the prefetch engine
    if (prefetchEnabled == true)
    {
      USB_EnablePrefetch();
    }

#if SLAB_USB_FULL_SPEED
    // Restore the clock
    USB_SetNormalClock();
#endif
    USB_EnableTransceiver();
  }

  USB_RestoreSfrPage();
}

int8_t USBD_UnStallEp(uint8_t epAddr)
{
  bool usbIntsEnabled;

  USB_SaveSfrPage();

  // Verify the endpoint address is valid and not Endpoint 0.
  if ((epAddr == EP0) || (epAddr >= SLAB_USB_NUM_EPS_USED))
  {
    SLAB_ASSERT(false);
    return USB_STATUS_ILLEGAL;
  }
  else
  {
    DISABLE_USB_INTS;

    // End the stall condition and set the endpoint state to idle.
    switch (epAddr)
    {
#if SLAB_USB_EP1IN_USED
      case (EP1IN):
        myUsbDevice.ep1in.state = D_EP_IDLE;
        USB_SetIndex(1);
        USB_EpnInEndStall();
        break;
#endif
#if SLAB_USB_EP2IN_USED
      case (EP2IN):
        myUsbDevice.ep2in.state = D_EP_IDLE;
        USB_SetIndex(2);
        USB_EpnInEndStall();
        break;
#endif
#if SLAB_USB_EP3IN_USED
      case (EP3IN):
        myUsbDevice.ep3in.state = D_EP_IDLE;
        USB_SetIndex(3);
        USB_EpnInEndStall();
        break;
#endif
#if SLAB_USB_EP1OUT_USED
      case (EP1OUT):
        myUsbDevice.ep1out.state = D_EP_IDLE;
        USB_SetIndex(1);
        USB_EpnOutEndStall();
        break;
#endif
#if SLAB_USB_EP2OUT_USED
      case (EP2OUT):
        myUsbDevice.ep2out.state = D_EP_IDLE;
        USB_SetIndex(2);
        USB_EpnOutEndStall();
        break;
#endif
#if SLAB_USB_EP3OUT_USED
      case (EP3OUT):
        myUsbDevice.ep3out.state = D_EP_IDLE;
        USB_SetIndex(3);
        USB_EpnOutEndStall();
        break;
#endif
    }

    ENABLE_USB_INTS;
    USB_RestoreSfrPage();
  }

  return USB_STATUS_OK;
}

int8_t USBD_Write(uint8_t epAddr,
                  uint8_t *dat,
                  uint16_t byteCount,
                  bool callback)
{
  bool usbIntsEnabled;
  USBD_Ep_TypeDef MEM_MODEL_SEG *ep;

  USB_SaveSfrPage();

  // Verify the endpoint address is valid.
  switch (epAddr)
  {
    case EP0:
#if SLAB_USB_EP1IN_USED
    case EP1IN:
#endif
#if SLAB_USB_EP2IN_USED
    case EP2IN:
#endif
#if SLAB_USB_EP3IN_USED
    case EP3IN:
#endif
      break;
#if SLAB_USB_EP1OUT_USED
    case EP1OUT:
#endif
#if SLAB_USB_EP2OUT_USED
    case EP2OUT:
#endif
#if SLAB_USB_EP3OUT_USED
    case EP3OUT:
#endif
    default:
      SLAB_ASSERT(false);
      return USB_STATUS_ILLEGAL;
  }

  // If the device is not configured and it is not Endpoint 0, we cannot begin
  // a transfer.
  if ((epAddr != EP0) && (myUsbDevice.state != USBD_STATE_CONFIGURED))
  {
    return USB_STATUS_DEVICE_UNCONFIGURED;
  }

  ep = GetEp(epAddr);

  // If the endpoint is not idle, we cannot start a new transfer.
  // Return the appropriate error code.
  if (ep->state != D_EP_IDLE)
  {
    if (ep->state == D_EP_STALL)
    {
      return USB_STATUS_EP_STALLED;
    }
    else
    {
      return USB_STATUS_EP_BUSY;
    }
  }

  DISABLE_USB_INTS;

  ep->buf = dat;
  ep->remaining = byteCount;
  ep->state = D_EP_TRANSMITTING;
  ep->misc.bits.callback = callback;

  switch (epAddr)
  {
    // For Endpoint 0, set the inPacketPending flag to true. The USB handler
    // will see this on the next SOF and begin the transfer.
    case (EP0):
      myUsbDevice.ep0.misc.bits.inPacketPending = true;
      break;

    // For data endpoints, we will call USB_WriteFIFO here to reduce latency
    // between the call to USBD_Write() and the first packet being sent.
#if SLAB_USB_EP1IN_USED
    case (EP1IN):
      USB_WriteFIFO(1,
                    (byteCount > SLAB_USB_EP1IN_MAX_PACKET_SIZE) ? SLAB_USB_EP1IN_MAX_PACKET_SIZE : byteCount,
                    myUsbDevice.ep1in.buf,
                    true);
      break;
#endif // SLAB_USB_EP1IN_USED
#if SLAB_USB_EP2IN_USED
    case (EP2IN):
      USB_WriteFIFO(2,
                    (byteCount > SLAB_USB_EP2IN_MAX_PACKET_SIZE) ? SLAB_USB_EP2IN_MAX_PACKET_SIZE : byteCount,
                    myUsbDevice.ep2in.buf,
                    true);
      break;
#endif // SLAB_USB_EP2IN_USED
#if SLAB_USB_EP3IN_USED
    case (EP3IN):
#if  ((SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_BULK) || (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_INTR))
      USB_WriteFIFO(3,
                    (byteCount > SLAB_USB_EP3IN_MAX_PACKET_SIZE) ? SLAB_USB_EP3IN_MAX_PACKET_SIZE : byteCount,
                    myUsbDevice.ep3in.buf,
                    true);
#elif (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC)
      myUsbDevice.ep3in.misc.bits.inPacketPending = true;
      myUsbDevice.ep3inIsoIdx = 0;
#endif
      break;
#endif // SLAB_USB_EP3IN_USED
  }

  ENABLE_USB_INTS;
  USB_RestoreSfrPage();

  return USB_STATUS_OK;
}

// -----------------------------------------------------------------------------
// UtilityFunctions

void USBD_SetUsbState(USBD_State_TypeDef newState)
{
#if (SLAB_USB_SUPPORT_ALT_INTERFACES)
  uint8_t i;
#endif
  USBD_State_TypeDef currentState;

  currentState = myUsbDevice.state;

  // If the device is un-configuring, disable the data endpoints and clear out
  // alternate interface settings
  if ((currentState >= USBD_STATE_SUSPENDED)
       && (newState < USBD_STATE_SUSPENDED))
  {
    USBD_AbortAllTransfers();

#if (SLAB_USB_SUPPORT_ALT_INTERFACES)
    for (i = 0; i < SLAB_USB_NUM_INTERFACES; i++)
    {
      myUsbDevice.interfaceAltSetting[i] = 0;
    }
#endif
  }
  if (newState == USBD_STATE_SUSPENDED)
  {
    myUsbDevice.savedState = currentState;
  }

  myUsbDevice.state = newState;

#if SLAB_USB_STATE_CHANGE_CB
  if (currentState != newState)
  {
    USBD_DeviceStateChangeCb(currentState, newState);
  }
#endif
}
