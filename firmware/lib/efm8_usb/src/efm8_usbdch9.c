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
// Function Prototypes

static USB_Status_TypeDef ClearFeature(void);
static USB_Status_TypeDef GetConfiguration(void);
static USB_Status_TypeDef GetDescriptor(void);
static USB_Status_TypeDef GetInterface(void);
static USB_Status_TypeDef GetStatus(void);
static USB_Status_TypeDef SetAddress(void);
static USB_Status_TypeDef SetConfiguration(void);
static USB_Status_TypeDef SetFeature(void);
static USB_Status_TypeDef SetInterface(void);
static void USBD_ActivateAllEps(bool forceIdle);
static void EP0_Write(uint8_t *dat, uint16_t numBytes);
void SendEp0Stall(void);

// -----------------------------------------------------------------------------
// Global Variables

extern SI_SEGMENT_VARIABLE(myUsbDevice, USBD_Device_TypeDef, MEM_MODEL_SEG);
SI_SEGMENT_VARIABLE(txZero[2], uint8_t, SI_SEG_CODE);

// -----------------------------------------------------------------------------
// Static Global Variables

static uint16_t pStatus;

// -----------------------------------------------------------------------------
// Chapter 9 Functions

/***************************************************************************//**
 * @brief       Processes Standard Request (Chapter 9 Command)
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
USB_Status_TypeDef USBDCH9_SetupCmd(void)
{
  USB_Status_TypeDef status = USB_STATUS_OK;

  switch (myUsbDevice.setup.bRequest)
  {
    case GET_STATUS:
      status = GetStatus();
      break;

    case CLEAR_FEATURE:
      status = ClearFeature();
      break;

    case SET_FEATURE:
      status = SetFeature();
      break;

    case SET_ADDRESS:
      status = SetAddress();
      break;

    case GET_DESCRIPTOR:
      status = GetDescriptor();
      break;

    case GET_CONFIGURATION:
      status = GetConfiguration();
      break;

    case SET_CONFIGURATION:
      status = SetConfiguration();
      break;

    case GET_INTERFACE:
      status = GetInterface();
      break;

    case SET_INTERFACE:
      status = SetInterface();
      break;

    default:
      status = USB_STATUS_REQ_ERR;
      break;
  }

  // Reset index to 0 in case one of the above commands modified it
  USB_SetIndex(0);

  // If the command resulted in an error, send a procedural stall
  if (status == USB_STATUS_REQ_ERR)
  {
    SendEp0Stall();
  }

  return status;
}

/***************************************************************************//**
 * @brief       Clears the requested feature
 * @details     Supports CLEAR_FEATURE for Remote Wakeup and Endpoint Halt
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static USB_Status_TypeDef ClearFeature(void)
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if (myUsbDevice.setup.wLength == 0)
  {
    switch (myUsbDevice.setup.bmRequestType.Recipient)
    {
  #if SLAB_USB_REMOTE_WAKEUP_ENABLED
      case USB_SETUP_RECIPIENT_DEVICE:
        if ((myUsbDevice.setup.wIndex == 0)
            && (myUsbDevice.setup.wValue == USB_FEATURE_DEVICE_REMOTE_WAKEUP)
            && (myUsbDevice.state >= USBD_STATE_ADDRESSED))
        {
          // Remote wakeup feature clear
          myUsbDevice.remoteWakeupEnabled = false;
          retVal = USB_STATUS_OK;
        }
        break;
  #endif // SLAB_USB_REMOTE_WAKEUP_ENABLED
      case USB_SETUP_RECIPIENT_ENDPOINT:
        if (myUsbDevice.setup.wValue == USB_FEATURE_ENDPOINT_HALT)
        {
          // Device does not support halting endpoint 0, but do not return
          // an error as this is a valid request
          if (((myUsbDevice.setup.wIndex & ~USB_EP_DIR_IN) == 0)
              && (myUsbDevice.state >= USBD_STATE_ADDRESSED))
          {
            retVal = USB_STATUS_OK;
          }
          else if (((myUsbDevice.setup.wIndex & ~USB_SETUP_DIR_D2H) < SLAB_USB_NUM_EPS_USED)
                   && (myUsbDevice.state == USBD_STATE_CONFIGURED))
          {
            retVal = USB_STATUS_OK;
            USB_SetIndex((myUsbDevice.setup.wIndex & 0xFF) & ~USB_SETUP_DIR_D2H);

#if (SLAB_USB_EP1IN_USED || SLAB_USB_EP2IN_USED || SLAB_USB_EP3IN_USED)
            if ((myUsbDevice.setup.wIndex & 0xFF) & USB_EP_DIR_IN)
            {
              USB_EpnInEndStallAndClearDataToggle();
            }
#endif
#if (SLAB_USB_EP1OUT_USED || SLAB_USB_EP2OUT_USED || SLAB_USB_EP3OUT_USED)
            if (((myUsbDevice.setup.wIndex & 0xFF) & USB_EP_DIR_IN) == 0)
            {
              USB_EpnOutEndStallAndClearDataToggle();
            }
#endif

            switch (myUsbDevice.setup.wIndex & 0xFF)
            {
#if SLAB_USB_EP1OUT_USED
              case (USB_EP_DIR_OUT | 1):
                if (myUsbDevice.ep1out.state != D_EP_RECEIVING)
                {
                  myUsbDevice.ep1out.state = D_EP_IDLE;
                }
                break;
#endif
#if SLAB_USB_EP2OUT_USED
              case (USB_EP_DIR_OUT | 2):
                if (myUsbDevice.ep2out.state != D_EP_RECEIVING)
                {
                  myUsbDevice.ep2out.state = D_EP_IDLE;
                }
                break;
#endif
#if SLAB_USB_EP3OUT_USED
              case (USB_EP_DIR_OUT | 3):
                if (myUsbDevice.ep3out.state != D_EP_RECEIVING)
                {
                  myUsbDevice.ep3out.state = D_EP_IDLE;
                }
                break;
#endif
#if SLAB_USB_EP1IN_USED
              case (USB_EP_DIR_IN | 1):
                if (myUsbDevice.ep1in.state != D_EP_TRANSMITTING)
                {
                  myUsbDevice.ep1in.state = D_EP_IDLE;
                }
                break;
#endif
#if SLAB_USB_EP2IN_USED
              case (USB_EP_DIR_IN | 2):
                if (myUsbDevice.ep2in.state != D_EP_TRANSMITTING)
                {
                  myUsbDevice.ep2in.state = D_EP_IDLE;
                }
                break;
#endif
#if SLAB_USB_EP3IN_USED
              case (USB_EP_DIR_IN | 3):
                if (myUsbDevice.ep3in.state != D_EP_TRANSMITTING)
                {
                  myUsbDevice.ep3in.state = D_EP_IDLE;
                }
                break;
#endif
            }
          }
        }
    }
  }
  return retVal;
}

/***************************************************************************//**
 * @brief       Gets the current configuration value
 * @details     Zero means the device is not configured, a non-zero value
 *              is the configuration value of the configured device.
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static USB_Status_TypeDef GetConfiguration(void)
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if ((myUsbDevice.setup.wIndex == 0)
      && (myUsbDevice.setup.wValue == 0)
      && (myUsbDevice.setup.wLength == 1)
      && (myUsbDevice.setup.bmRequestType.Direction == USB_SETUP_DIR_IN)
      && (myUsbDevice.setup.bmRequestType.Recipient == USB_SETUP_RECIPIENT_DEVICE))
  {
    if (myUsbDevice.state == USBD_STATE_ADDRESSED)
    {
      EP0_Write(txZero, 1);
      retVal = USB_STATUS_OK;
    }
    else if (myUsbDevice.state == USBD_STATE_CONFIGURED)
    {
      EP0_Write(&myUsbDevice.configurationValue, 1);
      retVal = USB_STATUS_OK;
    }
  }
  return retVal;
}

/***************************************************************************//**
 * @brief       Sends the requested USB Descriptor
 * @details     Supports single or multiple languages (configured by
 *              @ref SLAB_USB_NUM_LANGUAGES).
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static USB_Status_TypeDef GetDescriptor(void)
{
#if (SLAB_USB_NUM_LANGUAGES > 1)
  bool langSupported;
  uint8_t lang;
#endif

  uint8_t index;
  uint16_t length = 0;
  uint8_t *dat;
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if (*((uint8_t *)&myUsbDevice.setup.bmRequestType) ==
      (USB_SETUP_DIR_D2H | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_DEVICE))
  {
    index = myUsbDevice.setup.wValue & 0xFF;

    switch (myUsbDevice.setup.wValue >> 8)
    {
      case USB_DEVICE_DESCRIPTOR:
        if (index != 0)
        {
          break;
        }
        dat = (uint8_t *)myUsbDevice.deviceDescriptor;
        length = myUsbDevice.deviceDescriptor->bLength;
        break;

      case USB_CONFIG_DESCRIPTOR:
        if (index != 0)
        {
          break;
        }
        dat = (uint8_t *)myUsbDevice.configDescriptor;
        length = le16toh(myUsbDevice.configDescriptor->wTotalLength);
        break;

      case USB_STRING_DESCRIPTOR:
  #if (SLAB_USB_NUM_LANGUAGES == 1)

        dat = (uint8_t *)myUsbDevice.stringDescriptors[index];

        // Index 0 is the language string. If SLAB_USB_NUM_LANGUAGES == 1, we
        // know the length will be 4 and the format will be UTF16LE.
        if (index == 0)
        {
          length = 4;
          myUsbDevice.ep0String.encoding.type = USB_STRING_DESCRIPTOR_UTF16LE;
        }
        // Otherwise, verify the language is correct (either the value set as
        // SLAB_USB_LANGUAGE in usbconfig.h, or 0).
        else if ((myUsbDevice.setup.wIndex == 0) || (myUsbDevice.setup.wIndex == SLAB_USB_LANGUAGE))
        {
          // Verify the index is valid
          if (index < myUsbDevice.numberOfStrings)
          {
            length = *(dat + USB_STRING_DESCRIPTOR_LENGTH);
            myUsbDevice.ep0String.encoding.type = *(dat + USB_STRING_DESCRIPTOR_ENCODING);
            dat += USB_STRING_DESCRIPTOR_LENGTH;
            myUsbDevice.ep0String.encoding.init = true;
          }
        }
  #elif (SLAB_USB_NUM_LANGUAGES > 1)

        langSupported = false;

        // Index 0 is the language.
        if (index == 0)
        {
          dat = ((uint8_t *)myUsbDevice.stringDescriptors->languageArray[0][index]);
          length = *((uint8_t *)dat);
          myUsbDevice.ep0String.encoding.type = USB_STRING_DESCRIPTOR_UTF16LE;
        }
        else
        {
          // Otherwise, verify the language is one of the supported languages or 0.
          for (lang = 0; lang < SLAB_USB_NUM_LANGUAGES; lang++)
          {
            if ((myUsbDevice.stringDescriptors->languageIDs[lang] == myUsbDevice.setup.wIndex)
                || (myUsbDevice.stringDescriptors->languageIDs[lang] == 0))
            {
              langSupported = true;
              break;
            }
          }
          if ((langSupported == true) && (index < myUsbDevice.numberOfStrings))
          {
            dat = ((uint8_t *)myUsbDevice.stringDescriptors->languageArray[lang][index]);
            length = *(dat + USB_STRING_DESCRIPTOR_LENGTH);
            myUsbDevice.ep0String.encoding.type = *(dat + USB_STRING_DESCRIPTOR_ENCODING);
            dat += USB_STRING_DESCRIPTOR_LENGTH;

            if (myUsbDevice.ep0String.encoding.type == USB_STRING_DESCRIPTOR_UTF16LE_PACKED)
            {
              myUsbDevice.ep0String.encoding.init = true;
            }
            else
            {
              myUsbDevice.ep0String.encoding.init = false;
            }
          }
        }
  #endif // ( SLAB_USB_NUM_LANGUAGES == 1 )
    }

    // If there is a descriptor to send, get the proper length, then call
    // EP0_Write() to send.
    if (length)
    {
      if (length > myUsbDevice.setup.wLength)
      {
        length = myUsbDevice.setup.wLength;
      }

      EP0_Write(dat, length);

      retVal = USB_STATUS_OK;
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief       Sends the current interface alternate setting
 * @details     Sends 0x0000 if alternate interfaces are not supported.
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static USB_Status_TypeDef GetInterface(void)
{
  uint16_t interface = myUsbDevice.setup.wIndex;
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if ((interface < SLAB_USB_NUM_INTERFACES)
      && (myUsbDevice.setup.wLength == 1)
      && (myUsbDevice.setup.wValue == 0)
      && (*((uint8_t *)&myUsbDevice.setup.bmRequestType) ==
          (USB_SETUP_DIR_D2H | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_INTERFACE)))
  {
    if (myUsbDevice.state == USBD_STATE_CONFIGURED)
    {
#if (SLAB_USB_SUPPORT_ALT_INTERFACES)
      // Return the alternate setting for the specified interface
      EP0_Write(&myUsbDevice.interfaceAltSetting[interface], 1);
#else
      // Alternate interfaces are not supported, so return 0x0000.
      EP0_Write(&txZero, 1);
#endif
      retVal = USB_STATUS_OK;
    }
  }
  return retVal;
}

/***************************************************************************//**
 * @brief       Sends the requested Remote Wakeup, Self-Powered, or
 *              Endpoint Status
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static USB_Status_TypeDef GetStatus(void)
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if ((myUsbDevice.setup.wLength == 2)
      && (myUsbDevice.setup.wValue == 0)
      && (myUsbDevice.setup.bmRequestType.Direction == USB_SETUP_DIR_IN)
      && (myUsbDevice.state >= USBD_STATE_ADDRESSED))
  {
    pStatus = htole16(0);         // Default return value is 0x0000

    switch (myUsbDevice.setup.bmRequestType.Recipient)
    {
      case USB_SETUP_RECIPIENT_DEVICE:
        if (myUsbDevice.setup.wIndex == 0)
        {
  #if SLAB_USB_REMOTE_WAKEUP_ENABLED
          // Remote wakeup feature status
          if (myUsbDevice.remoteWakeupEnabled)
          {
            pStatus |= htole16(REMOTE_WAKEUP_ENABLED);
          }
  #endif // SLAB_USB_REMOTE_WAKEUP_ENABLED

  #if SLAB_USB_IS_SELF_POWERED_CB
          // Current self/bus power status
          if (USBD_IsSelfPoweredCb())
          {
            pStatus |= htole16(DEVICE_IS_SELFPOWERED);
          }
  #elif (SLAB_USB_BUS_POWERED == 0)
          pStatus |= htole16(DEVICE_IS_SELFPOWERED);
  #endif // SLAB_USB_IS_SELF_POWERED_CB

          retVal = USB_STATUS_OK;
        }
        break;

      case USB_SETUP_RECIPIENT_INTERFACE:
        if (myUsbDevice.setup.wIndex < SLAB_USB_NUM_INTERFACES)
        {
          retVal = USB_STATUS_OK;
        }
        break;


      case USB_SETUP_RECIPIENT_ENDPOINT:
        // Device does not support halting endpoint 0, but do not give
        // an error as this is a valid request
        if (((myUsbDevice.setup.wIndex & ~USB_EP_DIR_IN) == 0)
            && (myUsbDevice.state == USBD_STATE_ADDRESSED))
        {
          retVal = USB_STATUS_OK;
        }
        else if (myUsbDevice.state == USBD_STATE_CONFIGURED)
        {
          switch (myUsbDevice.setup.wIndex & 0xFF)
          {
  #if SLAB_USB_EP1OUT_USED
            case (USB_EP_DIR_OUT | 1):
              if (myUsbDevice.ep1out.state == D_EP_HALT)
              {
                pStatus = htole16(1);
              }
              retVal = USB_STATUS_OK;
              break;
  #endif
  #if SLAB_USB_EP2OUT_USED
            case (USB_EP_DIR_OUT | 2):
              if (myUsbDevice.ep2out.state == D_EP_HALT)
              {
                pStatus = htole16(1);
              }
              retVal = USB_STATUS_OK;
              break;
  #endif
  #if SLAB_USB_EP3OUT_USED
            case (USB_EP_DIR_OUT | 3):
              if (myUsbDevice.ep3out.state == D_EP_HALT)
              {
                pStatus = htole16(1);
              }
              retVal = USB_STATUS_OK;
              break;
  #endif
  #if SLAB_USB_EP1IN_USED
            case (USB_EP_DIR_IN | 1):
              if (myUsbDevice.ep1in.state == D_EP_HALT)
              {
                pStatus = htole16(1);
              }
              retVal = USB_STATUS_OK;
              break;
  #endif
  #if SLAB_USB_EP2IN_USED
            case (USB_EP_DIR_IN | 2):
              if (myUsbDevice.ep2in.state == D_EP_HALT)
              {
                pStatus = htole16(1);
              }
              retVal = USB_STATUS_OK;
              break;
  #endif
  #if SLAB_USB_EP3IN_USED
            case (USB_EP_DIR_IN | 3):
              if (myUsbDevice.ep3in.state == D_EP_HALT)
              {
                pStatus = htole16(1);
              }
              retVal = USB_STATUS_OK;
              break;
  #endif
          }
        }
        break;
    }

    // If the command was valid, send the requested status.
    if (retVal == USB_STATUS_OK)
    {
      EP0_Write((uint8_t *)&pStatus, 2);
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief       Sets the Address
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static USB_Status_TypeDef SetAddress(void)
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if ((myUsbDevice.setup.wValue < 128)
      && (myUsbDevice.setup.wLength == 0)
      && (myUsbDevice.setup.bmRequestType.Recipient == USB_SETUP_RECIPIENT_DEVICE)
      && (myUsbDevice.setup.wIndex == 0))
  {
    // If the device is in the Default state and the address is non-zero, put
    // the device in the Addressed state.
    if (myUsbDevice.state == USBD_STATE_DEFAULT)
    {
      if (myUsbDevice.setup.wValue != 0)
      {
        USBD_SetUsbState(USBD_STATE_ADDRESSED);
      }
      retVal = USB_STATUS_OK;
    }
    // If the device is already addressed and the address is zero, put the
    // device in the Default state.
    else if (myUsbDevice.state == USBD_STATE_ADDRESSED)
    {
      if (myUsbDevice.setup.wValue == 0)
      {
        USBD_SetUsbState(USBD_STATE_DEFAULT);
      }
      retVal = USB_STATUS_OK;
    }

    // Set the new address if the request was valid.
    if (retVal == USB_STATUS_OK)
    {
      USB_SetAddress(myUsbDevice.setup.wValue);
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief       Sets the Configuration
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static USB_Status_TypeDef SetConfiguration(void)
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if (((myUsbDevice.setup.wValue >> 8) == 0)
      && (myUsbDevice.setup.bmRequestType.Recipient == USB_SETUP_RECIPIENT_DEVICE)
      && (myUsbDevice.setup.wLength == 0)
      && (myUsbDevice.setup.wIndex == 0))
  {
    // If the device is in the Addressed state and a valid Configuration value
    // was sent, enter the Configured state.
    if (myUsbDevice.state == USBD_STATE_ADDRESSED)
    {
      if ((myUsbDevice.setup.wValue == 0)
          || (myUsbDevice.setup.wValue == myUsbDevice.configDescriptor->bConfigurationValue))
      {
        myUsbDevice.configurationValue = myUsbDevice.setup.wValue;
        if (myUsbDevice.setup.wValue == myUsbDevice.configDescriptor->bConfigurationValue)
        {
          USBD_ActivateAllEps(true);
          USBD_SetUsbState(USBD_STATE_CONFIGURED);
        }
        retVal = USB_STATUS_OK;
      }
    }
    // If the device is in the Configured state and Configuration zero is sent,
    // abort all transfer and enter the Addressed state.
    else if (myUsbDevice.state == USBD_STATE_CONFIGURED)
    {
      if ((myUsbDevice.setup.wValue == 0)
          || (myUsbDevice.setup.wValue == myUsbDevice.configDescriptor->bConfigurationValue))
      {
        myUsbDevice.configurationValue = myUsbDevice.setup.wValue;
        if (myUsbDevice.setup.wValue == 0)
        {
          USBD_SetUsbState(USBD_STATE_ADDRESSED);
          USBD_AbortAllTransfers();
        }
        else
        {
          // Reenable device endpoints, will reset data toggles
          USBD_ActivateAllEps(false);
        }
        retVal = USB_STATUS_OK;
      }
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief       Sets the Remote Wakeup or Endpoint Halt Feature
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static USB_Status_TypeDef SetFeature(void)
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if (myUsbDevice.setup.wLength == 0)
  {
    switch (myUsbDevice.setup.bmRequestType.Recipient)
    {
  #if SLAB_USB_REMOTE_WAKEUP_ENABLED
      case USB_SETUP_RECIPIENT_DEVICE:
        if ((myUsbDevice.setup.wIndex == 0)     // ITF no. 0
            && (myUsbDevice.setup.wValue == USB_FEATURE_DEVICE_REMOTE_WAKEUP)
            && (myUsbDevice.state == USBD_STATE_CONFIGURED))
        {
          myUsbDevice.remoteWakeupEnabled = true;
          retVal = USB_STATUS_OK;
        }
        break;
  #endif // SLAB_USB_REMOTE_WAKEUP_ENABLED
      case USB_SETUP_RECIPIENT_ENDPOINT:
        // Device does not support halting endpoint 0, but do not return
        // an error as this is a valid request
        if (((myUsbDevice.setup.wIndex & ~USB_EP_DIR_IN) == 0)
            && (myUsbDevice.state >= USBD_STATE_ADDRESSED))
        {
          retVal = USB_STATUS_OK;
        }
        else if ((((myUsbDevice.setup.wIndex) & ~USB_SETUP_DIR_D2H) < SLAB_USB_NUM_EPS_USED)
                 && (myUsbDevice.setup.wValue == USB_FEATURE_ENDPOINT_HALT)
                 && (myUsbDevice.state == USBD_STATE_CONFIGURED))
        {
          retVal = USB_STATUS_OK;
          USB_SetIndex((myUsbDevice.setup.wIndex & 0xFF) & ~USB_SETUP_DIR_D2H);

          // Enable Stalls on the specified endpoint.
#if (SLAB_USB_EP1IN_USED || SLAB_USB_EP2IN_USED || SLAB_USB_EP3IN_USED)
          if ((myUsbDevice.setup.wIndex & 0xFF) & USB_EP_DIR_IN)
          {
            USB_EpnInStall();
          }
#endif
#if (SLAB_USB_EP1OUT_USED || SLAB_USB_EP2OUT_USED || SLAB_USB_EP3OUT_USED)
          if (((myUsbDevice.setup.wIndex & 0xFF) & USB_EP_DIR_IN) == 0)
          {
            USB_EpnOutStall();
          }
#endif

          // Put the specified endpoint in the Halted state.
          switch (myUsbDevice.setup.wIndex & 0xFF)
          {
  #if SLAB_USB_EP1OUT_USED
            case (USB_EP_DIR_OUT | 1):
              myUsbDevice.ep1out.state = D_EP_HALT;
              break;
  #endif
  #if SLAB_USB_EP2OUT_USED
            case (USB_EP_DIR_OUT | 2):
              myUsbDevice.ep2out.state = D_EP_HALT;
              break;
  #endif
  #if SLAB_USB_EP3OUT_USED
            case (USB_EP_DIR_OUT | 3):
              myUsbDevice.ep3out.state = D_EP_HALT;
              break;
  #endif
  #if SLAB_USB_EP1IN_USED
            case (USB_EP_DIR_IN | 1):
              myUsbDevice.ep1in.state = D_EP_HALT;
              break;
  #endif
  #if SLAB_USB_EP2IN_USED
            case (USB_EP_DIR_IN | 2):
              myUsbDevice.ep2in.state = D_EP_HALT;
              break;
  #endif
  #if SLAB_USB_EP3IN_USED
            case (USB_EP_DIR_IN | 3):
              myUsbDevice.ep3in.state = D_EP_HALT;
              break;
  #endif
          }
        }
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief       Sets the Interface and Alternate Interface (if supported)
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static USB_Status_TypeDef SetInterface(void)
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;
  uint8_t interface = (uint8_t)myUsbDevice.setup.wIndex;
  uint8_t altSetting = (uint8_t)myUsbDevice.setup.wValue;

  if ((interface < SLAB_USB_NUM_INTERFACES)
      && (myUsbDevice.state == USBD_STATE_CONFIGURED)
      && (myUsbDevice.setup.wLength == 0)
#if (SLAB_USB_SUPPORT_ALT_INTERFACES == 0)
      && (altSetting == 0)
#endif
      && (myUsbDevice.setup.bmRequestType.Recipient == USB_SETUP_RECIPIENT_INTERFACE))
  {
#if (SLAB_USB_SUPPORT_ALT_INTERFACES)
    if (USBD_SetInterfaceCb(interface, altSetting) == USB_STATUS_OK)
    {
      myUsbDevice.interfaceAltSetting[interface] = altSetting;
      retVal = USB_STATUS_OK;
    }
#else
#if (SLAB_USB_NUM_INTERFACES == 1)
    // Reset data toggles on EP's
    USBD_ActivateAllEps(false);
#endif // ( SLAB_USB_NUM_INTERFACES == 1 )
    retVal = USB_STATUS_OK;
#endif // ( SLAB_USB_SUPPORT_ALT_INTERFACES )
  }

  return retVal;
}

// -----------------------------------------------------------------------------
// Utility Functions

/***************************************************************************//**
 * @brief       Enables all endpoints for data transfers
 * @return      Status of request (type @ref USB_Status_TypeDef)
 * @note        This function takes no parameters, but it uses the setup command
 *              stored in @ref myUsbDevice.setup.
 ******************************************************************************/
static void USBD_ActivateAllEps(bool forceIdle)
{
  if (forceIdle == true)
  {
#if SLAB_USB_EP1IN_USED
    myUsbDevice.ep1in.state = D_EP_IDLE;
#endif
#if SLAB_USB_EP2IN_USED
    myUsbDevice.ep2in.state = D_EP_IDLE;
#endif
#if SLAB_USB_EP3IN_USED
    myUsbDevice.ep3in.state = D_EP_IDLE;
#endif
#if SLAB_USB_EP1OUT_USED
    myUsbDevice.ep1out.state = D_EP_IDLE;
#endif
#if SLAB_USB_EP2OUT_USED
    myUsbDevice.ep2out.state = D_EP_IDLE;
#endif
#if SLAB_USB_EP3OUT_USED
    myUsbDevice.ep3out.state = D_EP_IDLE;
#endif
  }

#if SLAB_USB_EP1IN_USED
  USB_ActivateEp(1,                                                   // ep
                 SLAB_USB_EP1IN_MAX_PACKET_SIZE,                      // packetSize
                 1,                                                   // inDir
                 SLAB_USB_EP1OUT_USED,                                // splitMode
                 0);                                                  // isoMod
#endif // SLAB_USB_EP1IN_USED
#if SLAB_USB_EP2IN_USED
  USB_ActivateEp(2,                                                   // ep
                 SLAB_USB_EP2IN_MAX_PACKET_SIZE,                      // packetSize
                 1,                                                   // inDir
                 SLAB_USB_EP2OUT_USED,                                // splitMode
                 0);                                                  // isoMod
#endif // SLAB_USB_EP2IN_USED
#if SLAB_USB_EP3IN_USED
  USB_ActivateEp(3,                                                   // ep
                 SLAB_USB_EP3IN_MAX_PACKET_SIZE,                      // packetSize
                 1,                                                   // inDir
                 SLAB_USB_EP3OUT_USED,                                // splitMode
                 (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC));  // isoMod
#endif // SLAB_USB_EP3IN_USED
#if SLAB_USB_EP1OUT_USED
  USB_ActivateEp(1,                                                   // ep
                 SLAB_USB_EP1OUT_MAX_PACKET_SIZE,                     // packetSize
                 0,                                                   // inDir
                 SLAB_USB_EP1IN_USED,                                 // splitMode
                 0);                                                  // isoMod
#endif // SLAB_USB_EP1OUT_USED
#if SLAB_USB_EP2OUT_USED
  USB_ActivateEp(2,                                                   // ep
                 SLAB_USB_EP2OUT_MAX_PACKET_SIZE,                     // packetSize
                 0,                                                   // inDir
                 SLAB_USB_EP2IN_USED,                                 // splitMode
                 0);                                                  // isoMod
#endif // SLAB_USB_EP2OUT_USED
#if SLAB_USB_EP3OUT_USED
  USB_ActivateEp(3,                                                   // ep
                 SLAB_USB_EP3OUT_MAX_PACKET_SIZE,                     // packetSize
                 0,                                                   // inDir
                 SLAB_USB_EP3IN_USED,                                 // splitMode
                 (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC)); // isoMod
#endif // SLAB_USB_EP1OUT_USED
}

/***************************************************************************//**
 * @brief       Sets up an Endpoint 0 Write
 * @param       dat
 *              Data to transmit on Endpoint 0
 * @param       numBytes
 *              Number of bytes to transmit on Endpoint 0
 ******************************************************************************/
static void EP0_Write(uint8_t *dat, uint16_t numBytes)
{
  if (myUsbDevice.ep0.state == D_EP_IDLE)
  {
    myUsbDevice.ep0.buf = (uint8_t *)dat;
    myUsbDevice.ep0.remaining = numBytes;
    myUsbDevice.ep0.state = D_EP_TRANSMITTING;
    myUsbDevice.ep0.misc.c = 0;
  }
}
