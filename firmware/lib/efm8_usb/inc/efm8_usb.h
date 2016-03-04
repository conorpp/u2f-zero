/***************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 ******************************************************************************/

#ifndef __SILICON_LABS_EFM8_USB_H__
#define __SILICON_LABS_EFM8_USB_H__

#include "si_toolchain.h"
#include "usbconfig.h"
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Efm8_usb
 * @brief USB Device Protocol Stack for EFM8 devices
 * @{
 *
 *   @section usb_device_contents Contents
 *
 *   @li @ref usb_device_intro
 *   @li @ref usb_device_api
 *   @li @ref usb_device_conf
 *   @li @ref usb_device_powersave
 *   @li @ref usb_device_transfers
 *   @li @ref usb_device_pitfalls
 *
 *   @n @section usb_device_intro Introduction
 *
 *   The USB device protocol stack provides an API which makes it possible to
 *   create USB devices with a minimum of effort. The device stack supports Control,
 *   Bulk, Interrupt, and Isochronous transfers.
 *
 *   The stack is highly configurable to suit various needs and includes
 *   demonstration projects to get you started fast.
 *
 *   We recommend that you read through this documentation, then proceed to build
 *   and test a few example projects before you start designing your own device.
 *
 *   @n @section usb_library_architecture_diagram Library Architecture Diagram
 *
 *   @image html USB_Library_Architecture.png
 *
 *   @n @section usb_device_api The EFM8 USB Library API
 *
 *   This section contains brief descriptions of the functions in the API. You will
 *   find detailed information on input and output parameters and return values by
 *   clicking on the hyper-linked function names. It is also a good idea to study
 *   the code in the USB demonstration projects.
 *
 *   Your application code must include one header file: @em efm8_usb.h.
 *
 *   All functions defined in the API can be called from within interrupt handlers.
 *
 *  @subsection usb_device_api_functions API Functions
 *
 *   @ref USBD_Init() @n
 *    This function is called to register your device and all its properties with
 *    the device stack. The application must fill in a @ref USBD_Init_TypeDef
 *    structure prior to calling. When this function has been called your device
 *    is ready to be enumerated by the USB host.
 *
 *   @ref USBD_Read(), @ref USBD_Write() @n
 *    These functions initiate data transfers.
 *    @n @htmlonly USBD_Read() @endhtmlonly initiate a transfer of data @em
 *    from host @em to device (an @em OUT transfer in USB terminology).
 *    @n @htmlonly USBD_Write() @endhtmlonly initiate a transfer of data @em from
 *    device @em to host (an @em IN transfer).
 *
 *    When the USB host actually performs the transfer, your application will be
 *    notified by means of a call to the @ref USBD_XferCompleteCb() callback
 *    function (optionally). Refer to @ref TransferCallback for details of the
 *    callback functionality.
 *
 *   @ref USBD_AbortTransfer(), @ref USBD_AbortAllTransfers() @n
 *    These functions terminate transfers that are initiated with @htmlonly
 *    USBD_Read() or USBD_Write() @endhtmlonly but that have not completed yet.
 *    These functions will deactivate the transfer setup to make the USB device
 *    endpoint hardware ready for new (and potentially) different transfers.
 *
 *   @ref USBD_Connect(), @ref USBD_Disconnect() @n
 *    These functions turn the data-line (D+ or D-) pull-up on or off. They can
 *    be used to force re-enumeration. It's good practice to delay at least one
 *    second between @htmlonly USBD_Disconnect() and USBD_Connect() @endhtmlonly
 *    to allow the USB host to unload the currently active device driver.
 *
 *   @ref USBD_EpIsBusy() @n
 *    Checks if an endpoint is busy.
 *
 *   @ref USBD_StallEp(), @ref USBD_UnStallEp() @n
 *    These functions stall or un-stall an endpoint. This functionality may not
 *    be needed by your application. They may be useful when implementing some
 *    USB classes, e.g. a mass storage devices use them extensively.
 *
 *   @ref USBD_Stop() @n
 *    Removes the data-line (D+ or D-) pull-up and disables the USB block. The
 *    application should call @htmlonly USBD_Init() after calling
 *    USBD_Stop() @endhtmlonly to restart USB operation.
 *
 *   @ref USBD_Suspend() @n
 *    Puts the device in its low-power suspend mode. This function will not exit
 *    until a wakeup event (resume signaling, VBUS attachment/removal, or remote
 *    wakeup source interrupt) occurs. The USB Library can be configured to
 *    automatically call this function by configuring @ref SLAB_USB_PWRSAVE_MODE.
 *
 *   @ref USBD_RemoteWakeup() @n
 *    Used in SUSPENDED state (see @ref USB_Status_TypeDef) to signal resume to
 *    host. It's the applications responsibility to adhere to the USB standard
 *    which states that a device can not signal resume before it has been
 *    SUSPENDED for at least 5 ms. The function will also check that the host
 *    has sent a SET_FEATURE request to enable Remote Wakeup before issuing the
 *    resume.
 *
 *   @ref USBD_GetUsbState() @n
 *    Returns the device USB state (see @ref USBD_State_TypeDef). Refer to
 *    Figure 9-1. "Device State Diagram" in the USB revision 2.0 specification.
 *
 *   @ref USBD_Run() @n
 *    When @ref SLAB_USB_POLLED_MODE is set to 1, the USB interrupt is disabled
 *    and the application must periodically call @htmlonly USBD_Run()
 *    @endhtmlonly to handle USB events.
 *
 *  @n @subsection usb_device_api_callback Callback Functions
 *
 *  @subsubsection usb_device_api_mandatory_callbacks Mandatory Callback Functions
 *
 *    @n @anchor TransferCallback
 *    @ref USBD_XferCompleteCb() is called each time a packet is sent or
 *    received. It is called with three parameters, the status of the transfer,
 *    the number of bytes transferred and the number of bytes remaining. The
 *    transfer complete callback can be enabled or disabled by setting the
 *    <em>callback</em> parameters of @ref USBD_Write() and @ref USBD_Read() to
 *    true or false.
 *    @note This callback is called from within the USB interrupt handler if
 *        @ref SLAB_USB_POLLED_MODE is set to 1. Otherwise, it is called from
 *        @ref USBD_Run().
 *
 *  @n
 *  @subsubsection usb_device_api_optional_callbacks Optional Callback Functions
 *
 *    @n These callbacks are all optional, and it is up to the application
 *    programmer to decide if the application needs the functionality they
 *    provide. Each callback is enabled or disabled by setting a constant in
 *    <em>usbconfig.h</em>.
 *    @note These callbacks are called from within the USB interrupt handler if
 *          @ref SLAB_USB_POLLED_MODE is set to 1. Otherwise, they are called
 *          from @ref USBD_Run().
 *
 *   @n USBD_ResetCb() is called each time reset signaling is sensed on the USB
 *    wire.
 *
 *   @n USBD_SofCb() is called with the frame number as a parameter on each SOF
 *    interrupt.
 *
 *   @n USBD_DeviceStateChangeCb() is called whenever the device state changes.
 *    Some uses of this include detecting that a USB suspend has been issued
 *    in order to reduce current consumption or calling USBD_Read() after
 *    entering the Configured state. The USB HID keyboard example
 *    project has a good example on how to use this callback.
 *
 *   @n USBD_IsSelfPoweredCb() is called by the device stack when host
 *    queries the device with a GET_STATUS command to check if the device is
 *    currently self-powered or bus-powered. This feature is only applicable on
 *    self-powered devices which can also operate when only bus power is
 *    available.
 *
 *   @n USBD_SetupCmdCb() is called each time a setup command is received from
 *    the host. Use this callback to override or extend the default handling of
 *    standard setup commands, and to implement class- or vendor-specific setup
 *    commands. The USB HID keyboard example project has a good example of how
 *    to use this callback.
 *
 *   @n @section usb_device_conf Configuring the Library
 *
 *   Your application must provide a header file named @em usbconfig.h. This file
 *   must contain the following \#define's. See @ref usb_config for
 *   documentation of these constants.@n @n
 *   @code
 * // -----------------------------------------------------------------------------
 * // Specify bus- or self-powered
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_BUS_POWERED              0
 *
 * // -----------------------------------------------------------------------------
 * // Specify USB speed
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_FULL_SPEED               1
 *
 * // -----------------------------------------------------------------------------
 * // Enable or disable the clock recovery
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_CLOCK_RECOVERY_ENABLED   1
 *
 * // -----------------------------------------------------------------------------
 * // Enable or disable remote wakeup
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_REMOTE_WAKEUP_ENABLED    0
 *
 * // -----------------------------------------------------------------------------
 * // Specify number of interfaces and whether any interfaces support alternate
 * // settings
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_NUM_INTERFACES           1
 * #define SLAB_USB_SUPPORT_ALT_INTERFACES   0
 *
 * // -----------------------------------------------------------------------------
 * // Enable or disable each endpoint
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_EP1IN_USED               1
 * #define SLAB_USB_EP1OUT_USED              0
 * #define SLAB_USB_EP2IN_USED               0
 * #define SLAB_USB_EP2OUT_USED              0
 * #define SLAB_USB_EP3IN_USED               0
 * #define SLAB_USB_EP3OUT_USED              0
 *
 * // -----------------------------------------------------------------------------
 * // Specify the maximum packet size for each endpoint
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_EP1IN_MAX_PACKET_SIZE    64
 * #define SLAB_USB_EP1OUT_MAX_PACKET_SIZE   0
 * #define SLAB_USB_EP2IN_MAX_PACKET_SIZE    0
 * #define SLAB_USB_EP2OUT_MAX_PACKET_SIZE   0
 * #define SLAB_USB_EP3IN_MAX_PACKET_SIZE    0
 * #define SLAB_USB_EP3OUT_MAX_PACKET_SIZE   0
 *
 * // -----------------------------------------------------------------------------
 * // Specify transfer type of each endpoint
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_EP1IN_TRANSFER_TYPE      USB_EPTYPE_INTR
 * #define SLAB_USB_EP1OUT_TRANSFER_TYPE     USB_EPTYPE_BULK
 * #define SLAB_USB_EP2IN_TRANSFER_TYPE      USB_EPTYPE_INTR
 * #define SLAB_USB_EP2OUT_TRANSFER_TYPE     USB_EPTYPE_BULK
 * #define SLAB_USB_EP3IN_TRANSFER_TYPE      USB_EPTYPE_ISOC
 * #define SLAB_USB_EP3OUT_TRANSFER_TYPE     USB_EPTYPE_ISOC
 *
 * // -----------------------------------------------------------------------------
 * // Enable or disable callback functions
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_RESET_CB                 1
 * #define SLAB_USB_SOF_CB                   1
 * #define SLAB_USB_STATE_CHANGE_CB          1
 * #define SLAB_USB_IS_SELF_POWERED_CB       1
 * #define SLAB_USB_SETUP_CMD_CB             1
 * #define SLAB_USB_HANDLER_CB               0
 *
 * // -----------------------------------------------------------------------------
 * // Specify number of languages supported by string descriptors
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_NUM_LANGUAGES            1
 *
 * // -----------------------------------------------------------------------------
 * // If only one descriptor language is supported, specify that language here.
 * // If multiple descriptor languages are supported, this value is ignored and
 * // the supported languages must listed in the
 * // myUsbStringTableLanguageIDsDescriptor structure.
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_LANGUAGE                 USB_LANGID_ENUS
 *
 * // -----------------------------------------------------------------------------
 * // Set the power saving mode
 * //
 * // SLAB_USB_PWRSAVE_MODE configures when the device will automatically enter
 * // the USB power-save mode. It is a bitmask constant with bit values:
 * //
 * //   USB_PWRSAVE_MODE_OFF       - No energy saving mode selected
 * //   USB_PWRSAVE_MODE_ONSUSPEND - Enter USB power-save mode on USB suspend
 * //   USB_PWRSAVE_MODE_ONVBUSOFF - Enter USB power-save mode when not attached
 * //                                to the USB host.
 * //   USB_PWRSAVE_MODE_FASTWAKE  - Exit USB power-save mode more quickly.
 * //                                This is useful for some applications that
 * //                                support remote wakeup.
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_PWRSAVE_MODE             (USB_PWRSAVE_MODE_ONVBUSOFF \
 *                                            | USB_PWRSAVE_MODE_ONSUSPEND)
 *
 * // -----------------------------------------------------------------------------
 * // Enable or disable polled mode
 * //
 * // When enabled, the application must call USBD_Run() periodically to process
 * // USB events.
 * // When disabled, USB events will be handled automatically by an interrupt
 * // handler.
 * // -----------------------------------------------------------------------------
 * #define SLAB_USB_POLLED_MODE              0
 * @endcode
 *
 *   @n @section usb_device_powersave Energy-saving options
 *
 *   The device stack provides built-in energy saving options.These options
 *   are configured by setting flags in @ref SLAB_USB_PWRSAVE_MODE in @em
 *   usbconfig.h. These flags are bitmasks and can be or'd together.@n@n
 *
 *   <b>Energy-Saving Option Flags:</b>
 *
 *   @ref USB_PWRSAVE_MODE_OFF@n The device will not automatically enter its
 *   low-power suspned mode after detecting a USB suspend. The application
 *   firmware may still call @ref USBD_Suspend() to manually enter suspend mode.
 *
 *   @ref USB_PWRSAVE_MODE_ONSUSPEND@n Enter a low-power suspend mode
 *   when a USB suspend is detected. When resume signaling is detected,
 *   the stack will exit the low-power mode.
 *
 *   @ref USB_PWRSAVE_MODE_ONVBUSOFF@n Enter the low-power suspend
 *   mode any time the device detects that VBUS is not present. When VBUS is
 *   attached, the stack will exit the low-power mode. The USB Specification
 *   does not define the state of the device when VBUS is not present, but it
 *   may be desirable for some applications to enter suspend mode when in this
 *   undefined state.
 *
 *   @ref USB_PWRSAVE_MODE_FASTWAKE@n Keep the internal regulator at
 *   its normal operating state while in the low-power suspend state. This
 *   allows the device to wake from suspend more quickly than it would from its
 *   suspend state. This option can be useful in applications that support
 *   remote wakeup and need to exit suspend in time to recognize some external
 *   signal (i.e. a byte received on the UART). The device will still consume
 *   low enough power to meet the USB Suspend Current Specification, but it will
 *   be slightly higher than it would otherwise be.
 *
 *   The USB HID Keyboard device example project demonstrates some of these
 *   energy-saving options.
 *
 *   <b>Example:</b>
 *   Leave all energy saving to the stack, the device enters low-power mode on
 *   suspend and when detached from host. @n
 *   In usbconfig.h:
 *   @code
 *   #define SLAB_USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONSUSPEND | USB_PWRSAVE_MODE_ONVBUSOFF)
 *   @endcode
 *
 *   @n @section usb_device_transfers Transfer Operation
 *
 *   @subsection usb_device_transfers_overview Overview
 *
 *   A USB transfer consists of one or more packets. For an IN transfer, the
 *   packets are sent from the device to the host. For an OUT transfer, the
 *   packets are sent from the host to the device.  @ref USBD_Write() initiates
 *   an IN transfer, while @ref USBD_Read() initiates an OUT transfer.
 *
 *   @subsection usb_device_transfers_types Transfer Types
 *
 *   There are four USB transfer types: @ref usb_device_transfer_types_control,
 *   @ref usb_device_transfer_types_bulk, @ref usb_device_transfer_types_interrupt,
 *   and @ref usb_device_transfer_types_isochronous.
 *
 *   @subsubsection usb_device_transfer_types_control Control
 *
 *   Control transfers are used to send configuration and status
 *   information, and also to send vendor-defined data. The USB Library only
 *   supports control transfers on Endpoint 0. @n @n
 *   The application firmware can handle control requests by looking at the
 *   contents of the setup packet in @ref USBD_SetupCmdCb(). If the application
 *   supports a particular request, it can call @ref USBD_Read() or @ref
 *   USBD_Write() with <em>epAddr</em> set to <em>EP0</em> and return @ref
 *   USB_STATUS_OK. If it does not need to handle the request, it should return
 *   @ref USB_STATUS_REQ_UNHANDLED. This will notify the library that it should
 *   try to handle the setup command. The library will automatically service
 *   Standard (i.e. Chapter 9) requests, so @ref USBD_SetupCmdCb() should return
 *   @ref USB_STATUS_REQ_UNHANDLED unless it is a class- or vendor-specific
 *   request. If neither the library nor the application supports a setup
 *   request, the library will issue a stall.
 *
 *   @subsubsection usb_device_transfer_types_bulk Bulk
 *
 *   Bulk transfers are used to send large, non-periodic data. Examples include
 *   sending a file to a Mass Storage Device or a print-job to a printer. A bulk
 *   transfer may consist of one or more packets.
 *
 *   Endpoints are configured for bulk mode in <em>usbconfig.h</em>. As an
 *   example: @code
 *   #define SLAB_USB_EP1OUT_TRANSFER_TYPE     USB_EPTYPE_BULK@endcode
 *   configures Endpout 1 OUT transfers for bulk mode.
 *
 *   The @ref byteCount parameter of @ref USBD_Write() and @ref USBD_Read()
 *   configures the maximum length for a given bulk transfer. The transfer will
 *   complete when the device sends or receives either:
 *     1. A packet less than its maximum packet size
 *     2. Exactly the number of bytes specified in @ref byteCount
 *   Note that @ref USBD_XferCompleteCb() will be called for each packet sent or
 *   received for the duration of a transfer.
 *
 *   @subsubsection usb_device_transfer_types_interrupt Interrupt
 *
 *   Interrupt transfers are used to send low-bandwidth, hight-latency data at
 *   a non-periodic rate. Examples include input devices, such as mice,
 *   keyboards, and joysticks. An interrupt transfer may consist of one or more
 *   packets.
 *
 *   Endpoints are configured for interrupt mode in <em>usbconfig.h</em>. As an
 *   example: @code
 *   #define SLAB_USB_EP1OUT_TRANSFER_TYPE     USB_EPTYPE_INTR@endcode
 *   configures Endpout 1 OUT transfers for interrupt mode.
 *
 *   Interrupt transfers work identically to bulk transfer in the USB Library.
 *   Refer to @ref usb_device_transfer_types_bulk for more information.
 *
 *   @subsubsection usb_device_transfer_types_isochronous Isochronous
 *
 *   Isochronous transfers are used to send periodic, continuous data. Automatic
 *   error-checking is not included with isochronous transfers as it is with all
 *   other transfer types. Examples include streaming audio and video. As
 *   isochronous data is sent at a continuous rate, it typically consists of
 *   one IN and/or OUT packet per frame.
 *
 *   Endpoint 3 is the only endpoint in the USB Library that supports
 *   isochronous transfers. Endpoints are configured for isochronous mode in
 *   <em>usbconfig.h</em>. As an example: @code
 *   #define SLAB_USB_EP3OUT_TRANSFER_TYPE     USB_EPTYPE_ISOC@endcode
 *   configures Endpout 3 OUT transfers for isochronous mode.
 *
 *   The library works differently for isochronous transfers. The application
 *   must define a circular buffer to hold isochronous data. When calling
 *   USBD_Read() or USBD_Write(), <em>dat</em> is the first address of this
 *   buffer and <em>byteCount</em> is its length. The library will read from or
 *   write to this buffer as soon as the host issues a request, so it is the
 *   responsibility of the application firmware to ensure that this buffer is
 *   fed or consumed at the correct rate to prevent an underrun/overrun
 *   condition.
 *
 *   The parameters of @ref USBD_XferCompleteCb() take on a different meaning in
 *   isochronous mode. For OUT transfers, <em>xferred</em> is the number of
 *   bute received in the last packet and <em>remaining</em> is the current
 *   index into the circular buffer. For IN transfers, <em>xferred</em> is
 *   ignored, <em>remaining</em> is the current index into the circular buffer,
 *   and the return value is the number of bytes to transmit in the next
 *   packet.
 *
 *   @n @section usb_device_pitfalls Pitfalls
 *
 *   @subsection usb_device_pitfalls_nonreentrancy Non-Reentrancy
 *
 *    Due to the non-reentrant architecture of the 8051, it is recommended
 *    that all calls to a particular API function be made from functions of the
 *    same interrupt priority (main loop, low priority, or high priority).
 *
 *    The interrupt priority of the USB callback functions is determined by the
 *    constant @ref SLAB_USB_POLLED_MODE. When 0, the callbacks are called from
 *    the USB Interrupt Handler. When 1, the callbacks are called from
 *    USBD_Run(), which is typically called from the main loop.
 *    If an API function must be called from functions of differing interrupt
 *    priorities, there are a number of ways to ensure that the calls are made
 *    safely:
 *
 *      1. Disable the interrupt source of the higher-priority function before
 *         making the call. Restore the interrupt enable setting after the call
 *         returns:
 *
 *         (Assuming @htmlonly USBD_Write() is called from main() and
 *         USBD_XferCompleteCb() @endhtmlonly, the call from main() should
 *         disable and restore the USB interrupt):
 *         @code
 *         bool usbIntsEnabled = USB_GetIntsEnabled();
 *
 *         USB_DisableInts();
 *
 *         USBD_Write(EP1IN, myBuf, 1, true);
 *
 *         if (usbIntsEnabled)
 *         {
 *           USB_EnableInts();
 *         }
 *         @endcode
 *
 *      2. Add the compiler-specific reentrant keyword to the function
 *         definition(s) in efm8_usbd.c:
 *         @code
 *         int8_t USBD_AbortTransfer(uint8_t epAddr) reentrant
 *         @endcode
 *         and to the function prototype definition(s) in efm8_usb.h:
 *         @code
 *         int8_t USBD_AbortTransfer(uint8_t epAddr) reentrant;
 *         @endcode
 *         Using the reentrant keyword may require the application to provide
 *         a heap for local variable allocation. Additionally, it will reduce
 *         the performance and increase the code size of the modified function.
 *      3. Make a copy of the function(s) and rename it. Call the original
 *         function in once context, and the renamed version in another.
 *
 *    @subsection usb_device_pitfalls_buffer_allocation Buffer Allocation
 *
 *    Dynamically allocated buffers passed to @ref USBD_Write() and @ref
 *    USBD_Read() must not be freed until the transfer completes.
 *
 ******************************************************************************/

/***************************************************************************//**
 *  @addtogroup efm8_usb_constants Constants
 *  @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Global Constants

// SETUP request, direction of data stage
#define USB_SETUP_DIR_OUT       0               ///< Setup request data stage OUT direction value.
#define USB_SETUP_DIR_IN        1               ///< Setup request data stage IN direction value.
#define USB_SETUP_DIR_MASK      0x80            ///< Setup request data stage direction mask.
#define USB_SETUP_DIR_D2H       0x80            ///< Setup request data stage IN direction mask.
#define USB_SETUP_DIR_H2D       0x00            ///< Setup request data stage OUT direction mask.

// SETUP request type
#define USB_SETUP_TYPE_STANDARD       0         ///< Standard setup request value.
#define USB_SETUP_TYPE_CLASS          1         ///< Class setup request value.
#define USB_SETUP_TYPE_VENDOR         2         ///< Vendor setup request value.
#define USB_SETUP_TYPE_STANDARD_MASK  0x00      ///< Standard setup request mask.
#define USB_SETUP_TYPE_CLASS_MASK     0x20      ///< Class setup request mask.
#define USB_SETUP_TYPE_VENDOR_MASK    0x40      ///< Vendor setup request mask.

// SETUP request recipient
#define USB_SETUP_RECIPIENT_DEVICE    0         ///< Setup request device recipient value.
#define USB_SETUP_RECIPIENT_INTERFACE 1         ///< Setup request interface recipient value.
#define USB_SETUP_RECIPIENT_ENDPOINT  2         ///< Setup request endpoint recipient value.
#define USB_SETUP_RECIPIENT_OTHER     3         ///< Setup request other recipient value.

// bmRequestType bitmasks
#define USB_BMREQUESTTYPE_RECIPIENT     0x1F    ///< Recipient is bmRequestType[4:0]
#define USB_BMREQUESTTYPE_TYPE          0x60    ///< Type is bmRequestType[6:5]
#define USB_BMREQUESTTYPE_DIRECTION     0x80    ///< Recipient is bmRequestType[7]

// SETUP standard request codes for Full Speed devices
#define GET_STATUS                0             ///< Standard setup request GET_STATUS.
#define CLEAR_FEATURE             1             ///< Standard setup request CLEAR_FEATURE.
#define SET_FEATURE               3             ///< Standard setup request SET_FEATURE.
#define SET_ADDRESS               5             ///< Standard setup request SET_ADDRESS.
#define GET_DESCRIPTOR            6             ///< Standard setup request GET_DESCRIPTOR.
#define SET_DESCRIPTOR            7             ///< Standard setup request SET_DESCRIPTOR.
#define GET_CONFIGURATION         8             ///< Standard setup request GET_CONFIGURATION.
#define SET_CONFIGURATION         9             ///< Standard setup request SET_CONFIGURATION.
#define GET_INTERFACE             10            ///< Standard setup request GET_INTERFACE.
#define SET_INTERFACE             11            ///< Standard setup request SET_INTERFACE.
#define SYNCH_FRAME               12            ///< Standard setup request SYNCH_FRAME.

// SETUP class request codes
#define USB_HID_GET_REPORT        0x01          ///< HID class setup request GET_REPORT.
#define USB_HID_GET_IDLE          0x02          ///< HID class setup request GET_IDLE.
#define USB_HID_SET_REPORT        0x09          ///< HID class setup request SET_REPORT.
#define USB_HID_SET_IDLE          0x0A          ///< HID class setup request SET_IDLE.
#define USB_HID_SET_PROTOCOL      0x0B          ///< HID class setup request SET_PROTOCOL.
#define USB_CDC_SETLINECODING     0x20          ///< CDC class setup request SET_LINE_CODING.
#define USB_CDC_GETLINECODING     0x21          ///< CDC class setup request GET_LINE_CODING.
#define USB_CDC_SETCTRLLINESTATE  0x22          ///< CDC class setup request SET_CONTROL_LINE_STATE.
#define USB_MSD_BOTRESET          0xFF          ///< MSD class setup request Bulk only transfer reset.
#define USB_MSD_GETMAXLUN         0xFE          ///< MSD class setup request Get Max LUN.

// SETUP command GET/SET_DESCRIPTOR descriptor types
#define USB_DEVICE_DESCRIPTOR             1     ///< DEVICE descriptor value.
#define USB_CONFIG_DESCRIPTOR             2     ///< CONFIGURATION descriptor value.
#define USB_STRING_DESCRIPTOR             3     ///< STRING descriptor value.
#define USB_INTERFACE_DESCRIPTOR          4     ///< INTERFACE descriptor value.
#define USB_ENDPOINT_DESCRIPTOR           5     ///< ENDPOINT descriptor value.
#define USB_DEVICE_QUALIFIER_DESCRIPTOR   6     ///< DEVICE_QUALIFIER descriptor value.
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR 7     ///< OTHER_SPEED_CONFIGURATION descriptor value.
#define USB_INTERFACE_POWER_DESCRIPTOR    8     ///< INTERFACE_POWER descriptor value.
#define USB_HUB_DESCRIPTOR                0x29  ///< HUB descriptor value.
#define USB_HID_DESCRIPTOR                0x21  ///< HID descriptor value.
#define USB_HID_REPORT_DESCRIPTOR         0x22  ///< HID REPORT descriptor value.
#define USB_CS_INTERFACE_DESCRIPTOR       0x24  ///< Audio Class-specific Descriptor Type.

#define USB_DEVICE_DESCSIZE               18    ///< Device descriptor size.
#define USB_CONFIG_DESCSIZE               9     ///< Configuration descriptor size.
#define USB_INTERFACE_DESCSIZE            9     ///< Interface descriptor size.
#define USB_ENDPOINT_DESCSIZE             7     ///< Endpoint descriptor size.
#define USB_DEVICE_QUALIFIER_DESCSIZE     10    ///< Device qualifier descriptor size.
#define USB_OTHER_SPEED_CONFIG_DESCSIZE   9     ///< Device other speed configuration descriptor size.
#define USB_HID_DESCSIZE                  9     ///< HID descriptor size.
#define USB_CDC_HEADER_FND_DESCSIZE       5     ///< CDC Header functional descriptor size.
#define USB_CDC_CALLMNG_FND_DESCSIZE      5     ///< CDC Call Management functional descriptor size.
#define USB_CDC_ACM_FND_DESCSIZE          4     ///< CDC Abstract Control Management functional descriptor size.

// String descriptor locations
#define USB_STRING_DESCRIPTOR_ENCODING    0     ///< Denotes whether string descriptor is UTF-8 or binary
#define USB_STRING_DESCRIPTOR_LENGTH      1     ///< Length of string descriptor
#define USB_STRING_DESCRIPTOR_TYPE        2     ///< Type of string descriptor (USB_STRING_DESCRIPTOR)
#define USB_STRING_DESCRIPTOR_NAME        3     ///< The string encoded as per USB_STRING_DESCRIPTOR_PACKED

// String descriptor encoding types
#define USB_STRING_DESCRIPTOR_UTF16LE        0  ///< The string is in UTF-16LE encoding
#define USB_STRING_DESCRIPTOR_UTF16LE_PACKED 1  ///< The string is in packed UTF-16LE encoding (the 0x00
                                                ///  characters between ASCII characters are omitted)
#define USB_STRING_DESCRIPTOR_UTF8           2  ///< The string is in UTF-8 encoding

// Misc. USB definitions
#define USB_FULL_EP0_SIZE                 64      ///< The size of endpoint 0 at full speed.
#define USB_FULL_INT_BULK_MAX_EP_SIZE     64      ///< The max size of any full speed bulk/interrupt endpoint.
#define USB_FULL_ISOC_MAX_EP_SIZE         1023    ///< The max size of any full speed isochronous endpoint.
#define USB_LOW_EP0_SIZE                  8       ///< The size of endpoint 0 at low speed.
#define USB_LOW_INT_BULK_MAX_EP_SIZE      8       ///< The max size of any low speed bulk/interrupt endpoint.
#define USB_LOW_ISOC_MAX_EP_SIZE          0       ///< The max size of any low speed isochronous endpoint.
#define USB_EPTYPE_CTRL                   0       ///< Endpoint type control.
#define USB_EPTYPE_ISOC                   1       ///< Endpoint type isochronous.
#define USB_EPTYPE_BULK                   2       ///< Endpoint type bulk.
#define USB_EPTYPE_INTR                   3       ///< Endpoint type interrupt.
#define USB_EP_DIR_IN                     0x80    ///< Endpoint IN direction mask.
#define USB_EP_DIR_OUT                    0x00    ///< Endponit OUT direction mask.
#define USB_SETUP_PKT_SIZE                8       ///< Setup request packet size.
#define USB_EPNUM_MASK                    0x0F    ///< Endpoint number mask.
#define USB_LANGID_ENUS                   0x0409  ///< English-United States language id.
#define USB_LANGID_NOBO                   0x0414  ///< Norwegian-Bokmal language id.
#define USB_MAX_DEVICE_ADDRESS            127     ///< Maximum allowable device address.
#define MAX_USB_EP_NUM                    15      ///< Limit imposed by the USB standard
#define USB_VENDOR_ID_SILICON_LABS        0x10C4  ///< Silicon Labs VID

#define CONFIG_DESC_BM_REMOTEWAKEUP 0x20        ///< Configuration descriptor attribute macro.
#define CONFIG_DESC_BM_SELFPOWERED  0x40        ///< Configuration descriptor attribute macro.
#define CONFIG_DESC_BM_RESERVED_D7  0x80        ///< Configuration descriptor attribute macro.
#define CONFIG_DESC_BM_TRANSFERTYPE 0x03        ///< Configuration descriptor transfer type bitmask.
#define CONFIG_DESC_MAXPOWER_mA(x)  (((x)+1)/2) ///< Configuration descriptor power macro.

#define DEVICE_IS_SELFPOWERED       0x0001      ///< Standard request GET_STATUS bitmask.
#define REMOTE_WAKEUP_ENABLED       0x0002      ///< Standard request GET_STATUS bitmask.
#define USB_FEATURE_ENDPOINT_HALT         0     ///< Standard request CLEAR/SET_FEATURE bitmask.
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP  1     ///< Standard request CLEAR/SET_FEATURE bitmask.

#define HUB_FEATURE_PORT_RESET            4     ///< HUB class request CLEAR/SET_PORT_FEATURE feature selector.
#define HUB_FEATURE_PORT_POWER            8     ///< HUB class request CLEAR/SET_PORT_FEATURE feature selector.
#define HUB_FEATURE_C_PORT_CONNECTION     16    ///< HUB class request CLEAR/SET_PORT_FEATURE feature selector.
#define HUB_FEATURE_C_PORT_RESET          20    ///< HUB class request CLEAR/SET_PORT_FEATURE feature selector.
#define HUB_FEATURE_PORT_INDICATOR        22    ///< HUB class request CLEAR/SET_PORT_FEATURE feature selector.

#define USB_CLASS_CDC                     2     ///< CDC device/interface class code.
#define USB_CLASS_CDC_DATA                0x0A  ///< CDC Data interface class code.
#define USB_CLASS_CDC_ACM                 2     ///< CDC Abstract Control Model interface subclass code.
#define USB_CLASS_CDC_HFN                 0     ///< CDC class Header Functional Descriptor subtype.
#define USB_CLASS_CDC_CMNGFN              1     ///< CDC class Call Management Functional Descriptor subtype.
#define USB_CLASS_CDC_ACMFN               2     ///< CDC class Abstract Control Management Functional Descriptor subtype.
#define USB_CLASS_CDC_UNIONFN             6     ///< CDC class Union Functional Descriptor subtype.

#define USB_CLASS_HID                     3     ///< HID device/interface class code.
#define USB_CLASS_HID_KEYBOARD            1     ///< HID keyboard interface protocol code.
#define USB_CLASS_HID_MOUSE               2     ///< HID mouse interface protocol code.

#define USB_CLASS_HUB                     9     ///< HUB device/interface class code.

#define USB_CLASS_MSD                     8     ///< MSD device/interface class code.
#define USB_CLASS_MSD_BOT_TRANSPORT       0x50  ///< MSD Bulk Only Transport protocol.
#define USB_CLASS_MSD_SCSI_CMDSET         6     ///< MSD Subclass SCSI transparent command set.
#define USB_CLASS_MSD_CSW_CMDPASSED       0     ///< MSD BOT Command status wrapper command passed code.
#define USB_CLASS_MSD_CSW_CMDFAILED       1     ///< MSD BOT Command status wrapper command failed code.
#define USB_CLASS_MSD_CSW_PHASEERROR      2     ///< MSD BOT Command status wrapper cmd phase error code.

#define USB_CLASS_VENDOR_SPECIFIC         0xFF  ///< Vendor Specific class
#define USB_SUBCLASS_VENDOR_SPECIFIC      0xFF  ///< Vendor Specific sub-class

/// @brief USB power save modes
#define USB_PWRSAVE_MODE_OFF       0            ///< No energy saving option selected.
#define USB_PWRSAVE_MODE_ONSUSPEND 1            ///< Enter USB power-save mode on suspend.
#define USB_PWRSAVE_MODE_ONVBUSOFF 2            ///< Enter USB power-save mode when not attached to the USB host.
#define USB_PWRSAVE_MODE_FASTWAKE  4            ///< Exit USB power-save mode more quickly. This is useful for
                                                ///< some applications that support remote wakeup.

/// @brief Endpoint 0 packet size
#if SLAB_USB_FULL_SPEED
#define USB_EP0_SIZE    USB_FULL_EP0_SIZE
#else
#define USB_EP0_SIZE    USB_LOW_EP0_SIZE
#endif // SLABS_USB_FULL_SPEED

/// @brief Total number of USB endpoints used by the device
#define SLAB_USB_NUM_EPS_USED    (SLAB_USB_EP1IN_USED   \
                                  + SLAB_USB_EP1OUT_USED \
                                  + SLAB_USB_EP2IN_USED  \
                                  + SLAB_USB_EP2OUT_USED \
                                  + SLAB_USB_EP3IN_USED  \
                                  + SLAB_USB_EP3OUT_USED \
                                  + 1)
/**  @} (end addtogroup efm8_usb_constants Constants) */

/***************************************************************************//**
 *  @addtogroup efm8_usb_macros Macros
 *  @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Global Macros

/// Macro for getting minimum value.
#ifndef EFM8_MIN
#define EFM8_MIN(a, b)    ((a) < (b) ? (a) : (b))
#endif

/// Macro for getting maximum value.
#ifndef EFM8_MAX
#define EFM8_MAX(a, b)    ((a) > (b) ? (a) : (b))
#endif

#ifndef UNREFERENCED_ARGUMENT
/// Macro for removing unreferenced arguments from compiler warnings
#define UNREFERENCED_ARGUMENT(arg) (0, arg)
#endif

/***************************************************************************//**
 * @brief       Macro for creating USB-compliant UTF-16LE UNICODE string
 *              descriptor from a C string.
 * @details     This macro should be used for UTF-8 strings in which all
 *              characters are represented by a single ASCII byte (i.e.
 *              U.S. English strings).
 *              The USB Library will expand variables created with this macro
 *              by inserting a 0x00 between each character. This allows the
 *              string to be stored in a "packed", or compressed, format.
 * @n@n         This example sends "Silicon Labs" as the Manufacturer String:
 *
 *              #define MFR_STRING    "Silicon Labs"
 *
 *              UTF16LE_PACKED_STATIC_CONST_STRING_DESC(manufacturer[], \
 *                                                      MFR_STRING);
 * @param       __name
 *              The name of the variable that holds the string descriptor
 * @param       __val
 *              The value of the string descriptor
 ******************************************************************************/
#define UTF16LE_PACKED_STATIC_CONST_STRING_DESC(__name, __val) \
  SI_SEGMENT_VARIABLE(__name,  static const USB_StringDescriptor_TypeDef, SI_SEG_CODE) = \
    { USB_STRING_DESCRIPTOR_UTF16LE_PACKED, sizeof(__val) * 2, USB_STRING_DESCRIPTOR, __val }

/***************************************************************************//**
 * @brief       Macro for creating USB-compliant UTF-16LE UNICODE string
 *              descriptor from a C array initializer.
 * @details     This macro should be used for converting an array initializer
 *              into a string descriptor. Unlike @ref
 *              UTF16LE_PACKED_STATIC_CONST_STRING_DESC(), the library will not
 *              attempt to unpack variables created with this macro, so the
 *              array will be sent exactly as it is defined.
 * @n@n         This example sends "Mouse" as the Product String:
 *
 *              #define PROD_STRING   'M',0,'o',0,'u',0,'s',0,'e',0
 *
 *              ARRAY_STATIC_CONST_STRING_DESC(product[], \
 *                                             10, \
 *                                             PROD_STRING);
 * @param       __name
 *              The name of the variable that holds the string descriptor
 * @param       __len
 *              Number of characters (including nulls) in the string to send
 * @param       __val
 *              The array initializer.
 ******************************************************************************/
#define ARRAY_STATIC_CONST_STRING_DESC(__name, __len, __val) \
  SI_SEGMENT_VARIABLE(__name,  static const USB_StringDescriptor_TypeDef, SI_SEG_CODE) = \
    { USB_STRING_DESCRIPTOR_UTF16LE, __len + 2, USB_STRING_DESCRIPTOR, __val }

/***************************************************************************//**
 * @brief       Macro for creating USB-compliant UTF-16LE UNICODE string
 *              descriptor from a UTF-16 C string.
 * @details     This macro should be used for strings which are already
 *              represented in UTF-16. This is an advanced option that should
 *              only be used for some foreign languages.
 * @param       __name
 *              The name of the variable that holds the string descriptor
 * @param       __val
 *              The value of the string descriptor
 ******************************************************************************/
#define UTF16LE_STATIC_CONST_STRING_DESC(__name, __val) \
  SI_SEGMENT_VARIABLE(__name,  static const USB_StringDescriptor_TypeDef, SI_SEG_CODE) = \
    { USB_STRING_DESCRIPTOR_UTF16LE, sizeof(__val) + 2, USB_STRING_DESCRIPTOR, __val }

/***************************************************************************//**
 * @brief       Macro for creating Language ID's String Descriptor (String
 *              Descriptor 0)
 * @details     This macro should be used to create the Language ID String
 *              Descriptor.
 * @n@n         This example USB device only support U.S. English:
 *
 *              #define LANG_STRING   htole16(SLAB_USB_LANGUAGE)
 *
 *              LANGID_STATIC_CONST_STRING_DESC(langDesc[], LANG_STRING);
 *
 *              This example USB device support Norwegian U.S. English:
 *
 *              #define LANG_STRING   htole16(USB_LANGID_NOBO), \
 *                                    htole16(USB_LANGID_ENUS)
 *
 *              LANGID_STATIC_CONST_STRING_DESC(langDesc[], LANG_STRING);
 *
 * @param       __name
 *              The name of the variable that holds the string descriptor
 * @param       __val
 *              The value of the string descriptor
 ******************************************************************************/
#define LANGID_STATIC_CONST_STRING_DESC(__name, __val) \
  SI_SEGMENT_VARIABLE(__name,  static const USB_LangId_StringDescriptor_Typedef, __code) = \
    { (((SLAB_USB_NUM_LANGUAGES * 2) + 2) << 8) + USB_STRING_DESCRIPTOR, __val }

/**  @} (end addtogroup efm8_usb_macros Macros) */

/***************************************************************************//**
 *  @addtogroup efm8_usb_typedefs Typedefs
 *  @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Typedefs

/// @brief USB transfer status enumerator.
typedef enum
{
  USB_STATUS_OK = 0,                            ///< No errors detected.
  USB_STATUS_REQ_ERR = -1,                      ///< Setup request error.
  USB_STATUS_EP_BUSY = -2,                      ///< Endpoint is busy.
  USB_STATUS_REQ_UNHANDLED = -3,                ///< Setup request not handled.
  USB_STATUS_ILLEGAL = -4,                      ///< Illegal operation attempted.
  USB_STATUS_EP_STALLED = -5,                   ///< Endpoint is stalled.
  USB_STATUS_EP_ABORTED = -6,                   ///< Endpoint transfer was aborted.
  USB_STATUS_EP_ERROR = -7,                     ///< Endpoint transfer error.
  USB_STATUS_EP_NAK = -8,                       ///< Endpoint NAK'ed transfer request.
  USB_STATUS_DEVICE_UNCONFIGURED = -9,          ///< Device is unconfigured.
  USB_STATUS_DEVICE_SUSPENDED = -10,            ///< Device is suspended.
  USB_STATUS_DEVICE_RESET = -11,                ///< Device is/was reset.
  USB_STATUS_TIMEOUT = -12,                     ///< Transfer timeout.
  USB_STATUS_DEVICE_REMOVED = -13,              ///< Device was removed.
  USB_STATUS_EP_RX_BUFFER_OVERRUN = -14         ///< Not enough data in the Rx buffer to hold the
                                                ///< last received packet
} USB_Status_TypeDef;

/// @brief USB device state enumerator.
typedef enum
{
  USBD_STATE_NONE = 0,               ///< Device state is undefined/unknown.
  USBD_STATE_ATTACHED = 1,           ///< Device state is ATTACHED.
  USBD_STATE_POWERED = 2,            ///< Device state is POWERED.
  USBD_STATE_DEFAULT = 3,            ///< Device state is DEFAULT.
  USBD_STATE_ADDRESSED = 4,          ///< Device state is ADDRESSED.
  USBD_STATE_SUSPENDED = 5,          ///< Device state is SUSPENDED.
  USBD_STATE_CONFIGURED = 6,         ///< Device state is CONFIGURED.
  USBD_STATE_LASTMARKER = 7,         ///< Device state enum end marker.
} USBD_State_TypeDef;

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/// @brief Endpoint states
typedef enum
{
  D_EP_DISABLED = 0,                 ///< Endpoint is disabled
  D_EP_IDLE = 1,                     ///< Endpoint is idle
  D_EP_TRANSMITTING = 2,             ///< Endpoint is transmitting data
  D_EP_RECEIVING = 3,                ///< Endpoint is receiving data
  D_EP_STATUS = 4,                   ///< Endpoint is in status stage
  D_EP_STALL = 5,                    ///< Endpoint is stalling
  D_EP_HALT = 6,                     ///< Endpoint is halted
  D_EP_LASTMARKER = 7                ///< End of EpState enum
} USBD_EpState_TypeDef;
/// @endcond DO_NOT_INCLUDE_WITH_DOXYGEN

/// @brief Endpoint access address
typedef enum
{
  EP0,
#if (SLAB_USB_EP1IN_USED)
  EP1IN,
#endif
#if (SLAB_USB_EP2IN_USED)
  EP2IN,
#endif
#if (SLAB_USB_EP3IN_USED)
  EP3IN,
#endif
#if (SLAB_USB_EP1OUT_USED)
  EP1OUT,
#endif
#if (SLAB_USB_EP2OUT_USED)
  EP2OUT,
#endif
#if (SLAB_USB_EP3OUT_USED)
  EP3OUT,
#endif
}USB_EP_Index_TypeDef;

/// @brief USB Setup type.
typedef struct
{
  struct
  {
    uint8_t Recipient : 5;           ///< Request recipient (device, interface, endpoint, other)
    uint8_t Type : 2;                ///< Request type (standard, class or vendor).
    uint8_t Direction : 1;           ///< Transfer direction of SETUP data phase.
  } bmRequestType;

  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} USB_Setup_TypeDef;

/// @brief USB Setup Union type.
typedef union
{
  USB_Setup_TypeDef setup;
  uint8_t c[8];
  uint16_t i[4];
} USB_Setup_UnionDef;

/// @brief USB Device Descriptor.
typedef struct
{
  uint8_t bLength;                   ///< Size of this descriptor in bytes
  uint8_t bDescriptorType;           ///< Constant DEVICE Descriptor Type
  uint16_t bcdUSB;                   ///< USB Specification Release Number in BCD
  uint8_t bDeviceClass;              ///< Class code (assigned by the USB-IF)
  uint8_t bDeviceSubClass;           ///< Subclass code (assigned by the USB-IF)
  uint8_t bDeviceProtocol;           ///< Protocol code (assigned by the USB-IF)
  uint8_t bMaxPacketSize0;           ///< Maximum packet size for endpoint zero
  uint16_t idVendor;                 ///< Vendor ID (assigned by the USB-IF)
  uint16_t idProduct;                ///< Product ID (assigned by the manufacturer)
  uint16_t bcdDevice;                ///< Device release number in binary-coded decimal
  uint8_t iManufacturer;             ///< Index of string descriptor describing manufacturer
  uint8_t iProduct;                  ///< Index of string descriptor describing product
  uint8_t iSerialNumber;             ///< Index of string descriptor describing the serial number
  uint8_t bNumConfigurations;        ///< Number of possible configurations
} USB_DeviceDescriptor_TypeDef;


/// @brief USB Configuration Descriptor.
typedef struct
{
  uint8_t bLength;                   ///< Size of this descriptor in bytes
  uint8_t bDescriptorType;           ///< Constant CONFIGURATION Descriptor Type
  uint16_t wTotalLength;             ///< Total length of data returned for this
                                     ///< configuration. Includes the combined length of all
                                     ///< descriptors (configuration, interface, endpoint,
                                     ///< and class- or vendor-specific) returned for this
                                     ///< configuration.
  uint8_t bNumInterfaces;            ///< Number of interfaces supported by this
                                     ///< configuration
  uint8_t bConfigurationValue;       ///< Value to use as an argument to the
                                     ///< SetConfiguration request to select this
                                     ///< configuration.
  uint8_t iConfiguration;            ///< Index of string descriptor describing this
                                     ///< configuration.
  uint8_t bmAttributes;              ///< Configuration characteristics.
                                     ///< @n D7: Reserved (set to one)
                                     ///< @n D6: Self-powered
                                     ///< @n D5: Remote Wakeup
                                     ///< @n D4...0: Reserved (reset to zero)
  uint8_t bMaxPower;                 ///< Maximum power consumption of the USB device, unit
                                     ///< is 2mA per LSB
} USB_ConfigurationDescriptor_TypeDef;


/// @brief USB Interface Descriptor.
typedef struct
{
  uint8_t bLength;                   ///< Size of this descriptor in bytes.
  uint8_t bDescriptorType;           ///< Constant INTERFACE Descriptor Type.
  uint8_t bInterfaceNumber;          ///< Number of this interface. Zero-based value
                                     ///< identifying the index in the array of concurrent
                                     ///< interfaces supported by this configuration.
  uint8_t bAlternateSetting;         ///< Value used to select this alternate setting for
                                     ///< the interface identified in the prior field.
  uint8_t bNumEndpoints;             ///< Number of endpoints used by this interface
                                     ///< (excluding endpoint zero). If this value is zero,
                                     ///< this interface only uses the Default Control Pipe.
  uint8_t bInterfaceClass;           ///< Class code (assigned by the USB-IF). A value
                                     ///< of zero is reserved for future standardization. If
                                     ///< this field is set to FFH, the interface class is
                                     ///< vendor-specific. All other values are reserved for
                                     ///< assignment by the USB-IF.
  uint8_t bInterfaceSubClass;        ///< Subclass code (assigned by the USB-IF). These codes
                                     ///< are qualified by the value of the bInterfaceClass
                                     ///< field. If the bInterfaceClass field is reset to
                                     ///< zero, this field must also be reset to zero. If
                                     ///< the bInterfaceClass field is not set to FFH, all
                                     ///< values are reserved for assignment by the USB-IF.
  uint8_t bInterfaceProtocol;        ///< Protocol code (assigned by the USB). These codes
                                     ///< are qualified by the value of the bInterfaceClass
                                     ///< and the bInterfaceSubClass fields. If an interface
                                     ///< supports class-specific requests, this code
                                     ///< identifies the protocols that the device uses as
                                     ///< defined by the specification of the device class.
                                     ///< If this field is reset to zero, the device does
                                     ///< not use a class-specific protocol on this
                                     ///< interface. If this field is set to FFH, the device
                                     ///< uses a vendor-specific protocol for this interface
  uint8_t iInterface;                ///< Index of string descriptor describing this
                                     ///< interface.
} USB_InterfaceDescriptor_TypeDef;


/// @brief USB Endpoint Descriptor.
typedef struct
{
  uint8_t bLength;                   ///< Size of this descriptor in bytes
  uint8_t bDescriptorType;           ///< Constant ENDPOINT Descriptor Type
  uint8_t bEndpointAddress;          ///< The address of the endpoint
  uint8_t bmAttributes;              ///< This field describes the endpoint attributes
  uint16_t wMaxPacketSize;           ///< Maximum packet size for the endpoint
  uint8_t bInterval;                 ///< Interval for polling EP for data transfers
} USB_EndpointDescriptor_TypeDef;

/// @brief USB String Descriptor.
typedef uint8_t USB_StringDescriptor_TypeDef;         ///< The string descriptor

/// @brief USB Language ID String Descriptor.
typedef uint16_t USB_LangId_StringDescriptor_Typedef; ///< The language ID string descriptor

#if (SLAB_USB_NUM_LANGUAGES == 1)
/// @brief USB String Table Structure.
typedef USB_StringDescriptor_TypeDef * *USB_StringTable_TypeDef;
#elif (SLAB_USB_NUM_LANGUAGES > 1)
typedef struct
{
  uint16_t *languageIDs;
  USB_StringDescriptor_TypeDef * * *languageArray;
} USB_StringTable_TypeDef;
#endif // ( SLAB_USB_NUM_LANGUAGES == 1 )

/// @brief USB Device stack initialization structure.
///  @details This structure is passed to @ref USBD_Init() when starting up
///  the device.
typedef struct
{
  USB_DeviceDescriptor_TypeDef *deviceDescriptor; ///< Pointer to the device descriptor
  uint8_t *configDescriptor;                      ///< Pointer to the configuration descriptor
  USB_StringTable_TypeDef *stringDescriptors;     ///< Pointer to an array of string descriptor pointers
  uint8_t numberOfStrings;                        ///< Number of strings in string descriptor array
} USBD_Init_TypeDef;

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
// Endpoint structure
typedef struct
{
  uint8_t *buf;
  uint16_t remaining;
  USBD_EpState_TypeDef state;
  union
  {
    struct
    {
      uint8_t callback         : 1;
      uint8_t outPacketPending : 1;
      uint8_t inPacketPending  : 1;
      uint8_t waitForRead      : 1;
    } bits;
    uint8_t c;
  } misc;
} USBD_Ep_TypeDef;

// USB Device structure
typedef struct
{
  uint8_t configurationValue;
#if SLAB_USB_REMOTE_WAKEUP_ENABLED
  uint8_t remoteWakeupEnabled;
#endif
  uint8_t numberOfStrings;
  USBD_State_TypeDef state;
  USBD_State_TypeDef savedState;
  USB_Setup_TypeDef setup;
  union
  {
    struct
    {
      uint8_t type : 7;
      uint8_t init : 1;
    } encoding;
    uint8_t c;
  } ep0String;
  USBD_Ep_TypeDef ep0;
#if SLAB_USB_EP1IN_USED
  USBD_Ep_TypeDef ep1in;
#endif
#if SLAB_USB_EP2IN_USED
  USBD_Ep_TypeDef ep2in;
#endif
#if SLAB_USB_EP3IN_USED
  USBD_Ep_TypeDef ep3in;
#endif
#if SLAB_USB_EP1OUT_USED
  USBD_Ep_TypeDef ep1out;
#endif
#if SLAB_USB_EP2OUT_USED
  USBD_Ep_TypeDef ep2out;
#endif
#if SLAB_USB_EP3OUT_USED
  USBD_Ep_TypeDef ep3out;
#endif
#if ((SLAB_USB_EP3IN_USED) && (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC))
  uint16_t ep3inIsoIdx;
#endif
#if ((SLAB_USB_EP3OUT_USED) && (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC))
  uint16_t ep3outIsoIdx;
#endif
#if SLAB_USB_SUPPORT_ALT_INTERFACES
  uint8_t interfaceAltSetting[SLAB_USB_NUM_INTERFACES];
#endif
  USB_DeviceDescriptor_TypeDef *deviceDescriptor;
  USB_ConfigurationDescriptor_TypeDef *configDescriptor;
  USB_StringTable_TypeDef *stringDescriptors;
} USBD_Device_TypeDef;
/// @endcond DO_NOT_INCLUDE_WITH_DOXYGEN

/**  @} (end addtogroup efm8_usb_typedefs Typedefs) */

/***************************************************************************//**
 *  @addtogroup efm8_usb_constants Constants
 *  @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Compiler-specific memory segment definitions

#ifndef MEM_MODEL_SEG

// -----------------------------------------------------------------------------
//  Memory Model-Specific Location
//
//  MEM_MODEL_SEG is the default memory segment used for a given
//  memory model. Some variables use this symbol in order to reduce the amount
//  of code space and number of cycles it takes to access them.
//  The user can override this value by defining it in his usbconfig.h file.
//  For example:
//
//  #define MEM_MODEL_LOC    SI_SEG_XDATA
//
//  will place these variables in XRAM regardless of the memory model used to
//  build the project.
// -----------------------------------------------------------------------------

#if (defined SDCC) || (defined __SDCC)

#if (__SDCC_MODEL_SMALL)
#define MEM_MODEL_SEG  SI_SEG_IDATA
#elif defined __SDCC_MODEL_MEDIUM
#define MEM_MODEL_SEG  SI_SEG_PDATA
#elif (defined __SDCC_MODEL_LARGE) || (defined __SDCC_MODEL_HUGE)
#define MEM_MODEL_SEG  SI_SEG_XDATA
#else
#error "Illegal memory model setting."
#endif

#elif defined __RC51__

#if (__MEMORY_MODEL__ == 0)     // TINY
#define MEM_MODEL_SEG  SI_SEG_IDATA
#elif (__MEMORY_MODEL__ == 1)   // SMALL
#define MEM_MODEL_SEG  SI_SEG_IDATA
#elif (__MEMORY_MODEL__ == 2)   // COMPACT
#define MEM_MODEL_SEG  SI_SEG_PDATA
#elif (__MEMORY_MODEL__ == 3)   // LARGE
#define MEM_MODEL_SEG  SI_SEG_XDATA
#elif (__MEMORY_MODEL__ == 4)   // HUGE
#define MEM_MODEL_SEG  SI_SEG_PDATA
#else
#error "Illegal memory model setting."
#endif

#elif defined __C51__

#if (__MODEL__ == 0)     // SMALL
#define MEM_MODEL_SEG  SI_SEG_IDATA
#elif (__MODEL__ == 1)   // COMPACT
#define MEM_MODEL_SEG  SI_SEG_PDATA
#elif (__MODEL__ == 2)   // LARGE
#define MEM_MODEL_SEG  SI_SEG_XDATA
#else
#error "Illegal memory model setting."
#endif

#elif defined _CC51

#if (_MODEL == 's')     // SMALL
#define MEM_MODEL_SEG  SI_SEG_IDATA
#elif (_MODEL == 'a')   // AUXPAGE
#define MEM_MODEL_SEG  SI_SEG_PDATA
#elif (_MODEL == 'l')   // LARGE
#define MEM_MODEL_SEG  SI_SEG_XDATA
#elif (_MODEL == 'r')   // REENTRANT
#define MEM_MODEL_SEG  SI_SEG_XDATA
#else
#error "Illegal memory model setting."
#endif

#elif defined __ICC8051__
#if (__DATA_MODEL__ == 0)     // TINY
#define MEM_MODEL_SEG  SI_SEG_IDATA
#elif (__DATA_MODEL__ == 1)   // SMALL
#define MEM_MODEL_SEG  SI_SEG_IDATA
#elif (__DATA_MODEL__ == 2)   // LARGE
#define MEM_MODEL_SEG  SI_SEG_XDATA
#elif (__DATA_MODEL__ == 3)   // GENERIC
#define MEM_MODEL_SEG  SI_SEG_XDATA
#elif (__DATA_MODEL__ == 4)   // FAR
#define MEM_MODEL_SEG  SI_SEG_XDATA
#else
#error "Illegal memory model setting."
#endif

#endif
#endif  // #ifndef MEM_MODEL_SEG

/**  @} (end addtogroup efm8_usb_constants Constants) */

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
void USBD_SetUsbState(USBD_State_TypeDef newState);
USB_Status_TypeDef USBDCH9_SetupCmd(void);
/// @endcond DO_NOT_INCLUDE_WITH_DOXYGEN

// -----------------------------------------------------------------------------
// Library Configuration Definitions

/**************************************************************************//**
 * @addtogroup efm8_usb_config Library Configuration
 * @{
 *
 * @details Library configuration constants read from usbconfig.h.
 *
 *  This library will look for configuration constants in **usbconfig.h**.
 *  This file is provided/written by the user and should be
 *  located in a directory that is part of the include path.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_BUS_POWERED
 * @brief Configures the USB device for bus-powered or self-powered mode.
 * @details
 * When '1' the USB device is bus-powered.
 * When '0' the USB device is self-powered.
 *
 * Default setting is '1' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_FULL_SPEED
 * @brief Configures the USB device for full-speed or low-speed operation.
 * @details
 * When '1' the USB device is full-speed
 * When '0' the USB device is low-speed
 *
 * Default setting is '1' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_CLOCK_RECOVERY_ENABLED
 * @brief Enables/disables the USB Clock Recovery
 * @details USB Clock Recovery uses the incoming USB dat stream to adjust the
 *  internal oscillator. This allows the internal oscillator to meet the
 *  requirements for USB clock tolerance.
 *
 * When '1' the USB clock recovery is enabled
 * When '0' the USB clock recovery is disabled
 *
 * Default setting is '1' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_REMOTE_WAKEUP_ENABLED
 * @brief Enables/disables remote wakeup capability
 * @details Remote wakeup allow the USB device to wake the host from suspend.
 *   When enabled, the library will call @ref USBD_RemoteWakeupCb() to determine
 *   if the remote wakeup source caused the device to wake up. If it was, the
 *   library will exit suspend mode and the application should call
 *   @ref USBD_RemoteWakeup() to wake up the host.
 *
 * When '1' remote wakeup is enabled
 * When '0' remote wakeup is disabled
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_NUM_INTERFACES
 * @brief The number of interfaces available in the configuration
 * @details
 * Default setting is '1' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_SUPPORT_ALT_INTERFACES
 * @brief Enables/disables alternate interface settings
 * @details If any of the interfaces support alternate settings, this should be
 *  set to '1'. Upon receiveing a SET_INTERFACE request, the library will call
 *  @ref USBD_SetInterfaceCb(), which should return @ref USB_STATUS_OK if the
 *  alternate setting is valid or @ref USB_STATUS_REQ_ERR if it is not.
 *
 * When '1' alternate inteface settings are supported
 * When '0' alternate interface settings are not supported, and the library will
 *  respond to any SET_INTERFACE request with a procedural stall
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP1IN_USED
 * @brief Enables/disables Endpoint 1 IN
 * @details
 * When '1' Endpoint 1 IN is enabled
 * When '0' Endpoint 1 IN is disabled
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP1OUT_USED
 * @brief Enables/disables Endpoint 1 OUT
 * @details
 * When '1' Endpoint 1 OUT is enabled
 * When '0' Endpoint 1 OUT is disabled
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP2IN_USED
 * @brief Enables/disables Endpoint 2 IN
 * @details
 * When '1' Endpoint 2 IN is enabled
 * When '0' Endpoint 2 IN is disabled
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP2OUT_USED
 * @brief Enables/disables Endpoint 2 OUT
 * @details
 * When '1' Endpoint 2 OUT is enabled
 * When '0' Endpoint 2 OUT is disabled
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP3IN_USED
 * @brief Enables/disables Endpoint 3 IN
 * @details
 * When '1' Endpoint 3 IN is enabled
 * When '0' Endpoint 3 IN is disabled
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP3OUT_USED
 * @brief Enables/disables Endpoint 3 OUT
 * @details
 * When '1' Endpoint 3 OUT is enabled
 * When '0' Endpoint 3 OUT is disabled
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP1IN_MAX_PACKET_SIZE
 * @brief The maximum packet size that can be received on Endpoint 1 IN
 * @details
 * Default setting is '64' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP1OUT_MAX_PACKET_SIZE
 * @brief The maximum packet size that can be transmitted on Endpoint 1 OUT
 * @details
 * Default setting is '64' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP2IN_MAX_PACKET_SIZE
 * @brief The maximum packet size that can be received on Endpoint 2 IN
 * @details
 * Default setting is '64' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP2OUT_MAX_PACKET_SIZE
 * @brief The maximum packet size that can be transmitted on Endpoint 2 OUT
 * @details
 * Default setting is '64' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP3IN_MAX_PACKET_SIZE
 * @brief The maximum packet size that can be received on Endpoint 3 IN
 * @details
 * Default setting is '64' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP3OUT_MAX_PACKET_SIZE
 * @brief The maximum packet size that can be transmitted on Endpoint 3 OUT
 * @details
 * Default setting is '64' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP1IN_TRANSFER_TYPE
 * @brief Transfer type on Endpoint 1 IN
 * @details
 * May take one of the following values:
 *  USB_EPTYPE_INTR - Interrupt
 *  USB_EPTYPE_BULK - Bulk
 *
 * Default setting is @ref USB_EPTYPE_INTR and may be overridden by defining in
 * 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP1OUT_TRANSFER_TYPE
 * @brief Transfer type on Endpoint 1 OUT
 * @details
 * May take one of the following values:
 *  USB_EPTYPE_INTR - Interrupt
 *  USB_EPTYPE_BULK - Bulk
 *
 * Default setting is @ref USB_EPTYPE_INTR and may be overridden by defining in
 * 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP2IN_TRANSFER_TYPE
 * @brief Transfer type on Endpoint 2 IN
 * @details
 * May take one of the following values:
 *  USB_EPTYPE_INTR - Interrupt
 *  USB_EPTYPE_BULK - Bulk
 *
 * Default setting is @ref USB_EPTYPE_INTR and may be overridden by defining in
 * 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP2OUT_TRANSFER_TYPE
 * @brief Transfer type on Endpoint 2 OUT
 * @details
 * May take one of the following values:
 *  USB_EPTYPE_INTR - Interrupt
 *  USB_EPTYPE_BULK - Bulk
 *
 * Default setting is @ref USB_EPTYPE_INTR and may be overridden by defining in
 * 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP3IN_TRANSFER_TYPE
 * @brief Transfer type on Endpoint 3 IN
 * @details
 * May take one of the following values:
 *  USB_EPTYPE_INTR - Interrupt
 *  USB_EPTYPE_BULK - Bulk
 *  USB_EPTYPE_ISOC - Isochronous
 *
 * Default setting is @ref USB_EPTYPE_INTR and may be overridden by defining in
 * 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_EP3OUT_TRANSFER_TYPE
 * @brief Transfer type on Endpoint 3 OUT
 * @details
 * May take one of the following values:
 *  USB_EPTYPE_INTR - Interrupt
 *  USB_EPTYPE_BULK - Bulk
 *  USB_EPTYPE_ISOC - Isochronous
 *
 * Default setting is @ref USB_EPTYPE_INTR and may be overridden by defining in
 * 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_RESET_CB
 * @brief Enables/disables the USB Reset callback function
 * @details
 * When '1' @ref USBD_ResetCb() is called upon reception of a USB Reset
 * When '0' @ref USBD_ResetCb() is not called upon reception of a USB Reset
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_SOF_CB
 * @brief Enables/disables the USB Start-Of-Frame callback function
 * @details
 * When '1' @ref USBD_SofCb() is called upon reception of a Start-of-Frame
 *  packet
 * When '0' @ref USBD_SofCb() is not called upon reception of a Start-of-Frame
 *  packet
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_STATE_CHANGE_CB
 * @brief Enables/disables the USB State Change callback function
 * @details
 * When '1' @ref USBD_DeviceStateChangeCb() is called when the USB device state
 *  changes
 * When '0' @ref USBD_DeviceStateChangeCb() is not called when the USB device
 *  state changes
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_IS_SELF_POWERED_CB
 * @brief Enables/disables the USB Self-Powered callback function
 * @details
 * When '1' @ref USBD_IsSelfPoweredCb() is called upon reception of a
 *  GET_STATUS (Self-Powered) request
 * When '0' @ref USBD_IsSelfPoweredCb() is not called upon reception of a
 *  GET_STATUS (Self-Powered) request
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_SETUP_CMD_CB
 * @brief Enables/disables the USB Setup Command callback function
 * @details
 * When '1' @ref USBD_SetupCmdCb() is called upon reception of a Setup Request
 * When '0' @ref USBD_SetupCmdCb() is not called upon reception of a Setup
 *  Request
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_HANDLER_CB
 * @brief Enables/disables the USB Handler Entry and Exit callback functions
 * @details
 * When '1' @ref USBD_EnterHandler() will be called before the USB handler
 *  executes and @ref USBD_ExitHandler() will be called after the USB handler
 *  completes
 * When '0' @ref USBD_EnterHandler() and @ref USBD_ExitHandler() will not be
 *  called
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_NUM_LANGUAGES
 * @brief Number of languages supported by the USB string descriptors
 * @details
 * Default setting is '1' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_LANGUAGE
 * @brief
 *  Defines the language of the USB string descriptors when
 *  @ref SLAB_USB_NUM_LANGUAGES is '1'.
 *
 * @details
 * When @ref SLAB_USB_NUM_LANGUAGES is greater than '1', the supported languages
 *  must be defined in a separate table, and a structure of type
 *  @ref USB_StringDescriptor_TypeDef must be defined for each supported
 *  language
 *
 * Default setting is @ref USB_LANGID_ENUS and may be overridden by defining in
 *  'usbconfig.h'.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_PWRSAVE_MODE
 * @brief Configures the power-saving options supported by the device
 *
 * @details
 * Default setting is @ref USB_PWRSAVE_MODE_ONSUSPEND and may be overridden by
 *  defining in 'usbconfig.h'.
 *
 *  SLAB_USB_PWRSAVE_MODE configures when the device will automatically enter
 *  the USB power-save mode. It is a bitmask constant with bit values:
 *
 *   @ref USB_PWRSAVE_MODE_OFF       - No energy saving option selected
 *   @ref USB_PWRSAVE_MODE_ONSUSPEND - Enter USB power-save mode on USB suspend
 *   @ref USB_PWRSAVE_MODE_ONVBUSOFF - Enter USB power-save mode when not
 *                                     attached to the USB host.
 *   @ref USB_PWRSAVE_MODE_FASTWAKE  - Exit USB power-save mode more quickly.
 *                                     This is useful for some applications that
 *                                     support remote wakeup.
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_USB_POLLED_MODE
 * @brief Enables/disables USB library polled mode
 * @details
 * When '1' the library will run in polled mode
 * When '0' the library will run in interrupt mode
 *
 * Default setting is '0' and may be overridden by defining in 'usbconfig.h'.
 *
 *****************************************************************************/

/**  @} (end addtogroup efm8_usb_config Library Configuration) */

// Set default USB library configurations if the value is not configured in
// usbconfig.h

#ifndef SLAB_USB_BUS_POWERED
#define SLAB_USB_BUS_POWERED              1
#endif

#ifndef SLAB_USB_FULL_SPEED
#define SLAB_USB_FULL_SPEED               1
#endif

#ifndef SLAB_USB_CLOCK_RECOVERY_ENABLED
#define SLAB_USB_CLOCK_RECOVERY_ENABLED   1
#endif

#ifndef SLAB_USB_REMOTE_WAKEUP_ENABLED
#define SLAB_USB_REMOTE_WAKEUP_ENABLED    0
#endif

#ifndef SLAB_USB_NUM_INTERFACES
#define SLAB_USB_NUM_INTERFACES           1
#endif

#ifndef SLAB_USB_SUPPORT_ALT_INTERFACES
#define SLAB_USB_SUPPORT_ALT_INTERFACES   0
#endif

#ifndef SLAB_USB_EP1IN_USED
#define SLAB_USB_EP1IN_USED               0
#endif

#ifndef SLAB_USB_EP1OUT_USED
#define SLAB_USB_EP1OUT_USED              0
#endif

#ifndef SLAB_USB_EP2IN_USED
#define SLAB_USB_EP2IN_USED               0
#endif

#ifndef SLAB_USB_EP2OUT_USED
#define SLAB_USB_EP2OUT_USED              0
#endif

#ifndef SLAB_USB_EP3IN_USED
#define SLAB_USB_EP3IN_USED               0
#endif

#ifndef SLAB_USB_EP3OUT_USED
#define SLAB_USB_EP3OUT_USED              0
#endif

#ifndef SLAB_USB_EP1IN_MAX_PACKET_SIZE
#define SLAB_USB_EP1IN_MAX_PACKET_SIZE    64
#endif

#ifndef SLAB_USB_EP1OUT_MAX_PACKET_SIZE
#define SLAB_USB_EP1OUT_MAX_PACKET_SIZE   64
#endif

#ifndef SLAB_USB_EP2IN_MAX_PACKET_SIZE
#define SLAB_USB_EP2IN_MAX_PACKET_SIZE    64
#endif

#ifndef SLAB_USB_EP2OUT_MAX_PACKET_SIZE
#define SLAB_USB_EP2OUT_MAX_PACKET_SIZE   64
#endif

#ifndef SLAB_USB_EP3IN_MAX_PACKET_SIZE
#define SLAB_USB_EP3IN_MAX_PACKET_SIZE    64
#endif

#ifndef SLAB_USB_EP3OUT_MAX_PACKET_SIZE
#define SLAB_USB_EP3OUT_MAX_PACKET_SIZE   64
#endif

#ifndef SLAB_USB_EP1IN_TRANSFER_TYPE
#define SLAB_USB_EP1IN_TRANSFER_TYPE      USB_EPTYPE_INTR
#endif

#ifndef SLAB_USB_EP1OUT_TRANSFER_TYPE
#define SLAB_USB_EP1OUT_TRANSFER_TYPE     USB_EPTYPE_INTR
#endif

#ifndef SLAB_USB_EP2IN_TRANSFER_TYPE
#define SLAB_USB_EP2IN_TRANSFER_TYPE      USB_EPTYPE_INTR
#endif

#ifndef SLAB_USB_EP2OUT_TRANSFER_TYPE
#define SLAB_USB_EP2OUT_TRANSFER_TYPE     USB_EPTYPE_INTR
#endif

#ifndef SLAB_USB_EP3IN_TRANSFER_TYPE
#define SLAB_USB_EP3IN_TRANSFER_TYPE      USB_EPTYPE_INTR
#endif

#ifndef SLAB_USB_EP3OUT_TRANSFER_TYPE
#define SLAB_USB_EP3OUT_TRANSFER_TYPE     USB_EPTYPE_INTR
#endif

#ifndef SLAB_USB_RESET_CB
#define SLAB_USB_RESET_CB                 0
#endif

#ifndef SLAB_USB_SOF_CB
#define SLAB_USB_SOF_CB                   0
#endif

#ifndef SLAB_USB_STATE_CHANGE_CB
#define SLAB_USB_STATE_CHANGE_CB          0
#endif

#ifndef SLAB_USB_IS_SELF_POWERED_CB
#define SLAB_USB_IS_SELF_POWERED_CB       0
#endif

#ifndef SLAB_USB_SETUP_CMD_CB
#define SLAB_USB_SETUP_CMD_CB             0
#endif

#ifndef SLAB_USB_HANDLER_CB
#define SLAB_USB_HANDLER_CB               0
#endif

#ifndef SLAB_USB_NUM_LANGUAGES
#define SLAB_USB_NUM_LANGUAGES            1
#endif

#ifndef SLAB_USB_LANGUAGE
#define SLAB_USB_LANGUAGE                 USB_LANGID_ENUS
#endif

#ifndef SLAB_USB_PWRSAVE_MODE
#define SLAB_USB_PWRSAVE_MODE             USB_PWRSAVE_MODE_ONSUSPEND
#endif

#ifndef SLAB_USB_POLLED_MODE
#define SLAB_USB_POLLED_MODE  0
#endif

#if SLAB_USB_POLLED_MODE
void usbIrqHandler(void);
#endif

/***************************************************************************//**
 *  @addtogroup efm8_api API Functions
 *  @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Abort all pending transfers.
 *
 * @details
 *   Aborts transfers for all endpoints currently in use. Pending
 *   transfers on the default endpoint (EP0) are not aborted.
 ******************************************************************************/
void USBD_AbortAllTransfers(void);

/***************************************************************************//**
 * @brief
 *   Abort a pending transfer on a specific endpoint.
 *
 * @param epAddr
 *   The address of the endpoint to abort.
 * @return
 *   @ref USB_STATUS_OK is the transfer aborted, @ref USB_STATUS_ILLEGAL
 *   otherwise
 ******************************************************************************/
int8_t USBD_AbortTransfer(uint8_t epAddr);

/***************************************************************************//**
 * @brief
 *   Start USB device operation.
 *
 * @details
 *   Device operation is started by connecting a pull-up resistor on the
 *   appropriate USB data line.
 ******************************************************************************/
void USBD_Connect(void);

/***************************************************************************//**
 * @brief
 *   Stop USB device operation.
 *
 * @details
 *   Device operation is stopped by disconnecting the pull-up resistor from the
 *   appropriate USB data line. Often referred to as a "soft" disconnect.
 ******************************************************************************/
void USBD_Disconnect(void);

/***************************************************************************//**
 * @brief
 *   Check if an endpoint is busy doing a transfer.
 *
 * @param epAddr
 *   The address of the endpoint to check.
 *
 * @return
 *   True if endpoint is busy, false otherwise.
 ******************************************************************************/
bool USBD_EpIsBusy(uint8_t epAddr);

/***************************************************************************//**
 * @brief
 *   Get current USB device state.
 *
 * @return
 *   Device USB state. See @ref USBD_State_TypeDef.
 ******************************************************************************/
USBD_State_TypeDef USBD_GetUsbState(void);

/***************************************************************************//**
 * @brief
 *   Initializes USB device hardware and internal protocol stack data structures,
 *   then connects the data-line (D+ or D-) pullup resistor to signal host that
 *   enumeration can begin.
 *
 * @note
 *   You may later use @ref USBD_Disconnect() and @ref USBD_Connect() to force
 *   reenumeration.
 *
 * @param p
 *   Pointer to device initialization struct. See @ref USBD_Init_TypeDef.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int8_t USBD_Init(const USBD_Init_TypeDef *p);

/***************************************************************************//**
 * @brief
 *   Start a read (OUT) transfer on an endpoint.
 *
 * @note
 *   If it is possible that the host will send more data than your device
 *   expects, round buffer size up to the next multiple of maxpacket size.
 *
 * @param epAddr
 *   Endpoint address.
 *
 * @param dat
 *   Pointer to transfer data buffer.
 *
 * @param byteCount
 *   Transfer length.
 *
 * @param callback
 *   Boolean to determine if USB_XferCompleteCb should be called for this
 *   transfer.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int8_t USBD_Read(uint8_t epAddr,
                 uint8_t *dat,
                 uint16_t byteCount,
                 bool callback);

/***************************************************************************//**
 * @brief
 *   Perform a remote wakeup signaling sequence.
 *
 * @note
 *   It is the responsibility of the application to ensure that remote wakeup
 *   is not attempted before the device has been suspended for at least 5
 *   miliseconds. This function should not be called from within an interrupt
 *   handler.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int8_t USBD_RemoteWakeup(void);

/***************************************************************************//**
 * @brief
 *  Processes USB events when the library is configured for polled mode
 *
 * @details
 *  The USB library can be configured for interrupt (SLAB_USB_POLLED_MODE == 0)
 *  or polled (SLAB_USB_POLLED_MODE == 1) mode.
 *
 *  When in interrupt mode, the USB interrupt handler will trigger
 *  when a USB event occurs. Callback functions will be called as needed.
 *
 *  When in polled mode, the application must call USBD_Run() periodically to
 *  check for and process USB events. This may be useful in complex systems or
 *  when using an RTOS to perform all USB processing in the main loop instead
 *  of in the interrupt context.
 *
 ******************************************************************************/
void USBD_Run(void);

/***************************************************************************//**
 * @brief
 *   Set an endpoint in the stalled (halted) state.
 *
 * @param epAddr
 *   The address of the endpoint to stall.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int8_t USBD_StallEp(int8_t epAddr);

/***************************************************************************//**
 * @brief
 *   Stop USB device stack operation.
 *
 * @details
 *   The data-line pullup resistor is turned off, USB interrupts are disabled,
 *   and finally the USB pins are disabled.
 ******************************************************************************/
void USBD_Stop(void);

/***************************************************************************//**
 * @brief
 *   Enters USB suspend mode
 *
 * @details
 *   Disables USB transceiver, VDD Monitor, and prefetch engine. Suspends the
 *   internal regulator and internal oscillator.
 *   This function will not exit until the device recognizes resume signaling,
 *   VBUS attachment/removal, or a remote wakeup source interrupt.
 *   Before exiting, restores the states of the USB transceiver,
 *   VDD Monitor, prefetch engine, and internal regulator.
 ******************************************************************************/
void USBD_Suspend(void);

/***************************************************************************//**
 * @brief
 *   Reset stall state on a stalled (halted) endpoint.
 *
 * @param epAddr
 *   The address of the endpoint to un-stall.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int8_t USBD_UnStallEp(uint8_t epAddr);

/***************************************************************************//**
 * @brief
 *   Start a write (IN) transfer on an endpoint.
 *
 * @param epAddr
 *   Endpoint address.
 *
 * @param dat
 *   Pointer to transfer data buffer. This buffer must be WORD (4 byte) aligned.
 *
 * @param byteCount
 *   Transfer length.
 *
 * @param callback
 *   Boolean to determine if USB_XferCompleteCb should be called for this
 *   transfer.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int8_t USBD_Write(uint8_t epAddr,
                  uint8_t *dat,
                  uint16_t byteCount,
                  bool callback);

/**  @} (end addtogroup efm8_api API Functions) */

/***************************************************************************//**
 *  @addtogroup efm8_callbacks Callback Functions
 *  @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *  USB Handler Entry callback function.
 * @details
 *  Some systems may wish to be in a low-power state when between USB events.
 *  This low-power state may configure the system clock to a very low
 *  frequency. In order to reduce the execution time of the USB handler, this
 *  function is called before the handler executes to allow the system to switch
 *  to a higher clock source. When all USB processing is complete,
 *  @ref USBD_ExitHandler() will be called to allow the system to return
 *  to the low-power state.
 *  This callback function is optionally enabled by setting
 *  @ref SLAB_USB_HANDLER_CB to 1.
 ******************************************************************************/
void USBD_EnterHandler(void);

/***************************************************************************//**
 * @brief
 *  USB Handler Exit callback function.
 * @details
 *  Some systems may wish to be in a low-power state when between USB events.
 *  This low-power state may configure the system clock to a very low
 *  frequency. This function is called after all USB processing is finished
 *  to allow  a system that was previously configured by
 *  @ref USBD_EnterHandler() for high power to return to a low power state.
 *  This callback function is optionally enabled by setting
 *  @ref SLAB_USB_HANDLER_CB to 1.
 ******************************************************************************/
void USBD_ExitHandler(void);

/***************************************************************************//**
 * @brief
 *  USB Reset callback function.
 * @details
 *  Called whenever USB reset signaling is detected on the USB port.
 ******************************************************************************/
void USBD_ResetCb(void);

/***************************************************************************//**
 * @brief
 *  USB Start Of Frame (SOF) interrupt callback function.
 *
 * @details
 *  Called at each SOF interrupt (if enabled),
 *
 * @param sofNr
 *   Current frame number. The value rolls over to 0 after 16383 (0x3FFF).
 ******************************************************************************/
void USBD_SofCb(uint16_t sofNr);

/***************************************************************************//**
 * @brief
 *  USB State change callback function.
 *
 * @details
 *  Called whenever the USB state of the device changes
 *
 * @param oldState
 *   The device USB state just left. See @ref USBD_State_TypeDef.
 *
 * @param newState
 *   New (the current) USB device state. See @ref USBD_State_TypeDef.
 ******************************************************************************/
void USBD_DeviceStateChangeCb(USBD_State_TypeDef oldState,
                              USBD_State_TypeDef newState);

/***************************************************************************//**
 * @brief
 *  USB power mode callback function.
 *
 * @details
 *  Called whenever the device stack needs to know if the device is currently
 *  self- or bus-powered. Typically when host has issued a @ref GET_STATUS
 *  setup command.
 *
 * @return
 *  True if self-powered, false otherwise.
 ******************************************************************************/
bool USBD_IsSelfPoweredCb(void);

/***************************************************************************//**
 * @brief
 *  USB setup request callback function.
 *
 * @details
 *  Called on each setup request received from host. This gives the system a
 *  possibility to extend or override standard requests, and to implement class
 *  or vendor specific requests. Return @ref USB_STATUS_OK if the request is
 *  handled, return @ref USB_STATUS_REQ_ERR if it is an illegal request or
 *  return @ref USB_STATUS_REQ_UNHANDLED to pass the request on to the default
 *  request handler.
 *
 * @param setup
 *  Pointer to a USB setup packet. See @ref USB_Setup_TypeDef.
 *
 * @return
 *  An appropriate status/error code. See @ref USB_Status_TypeDef.
 ******************************************************************************/
USB_Status_TypeDef USBD_SetupCmdCb(SI_VARIABLE_SEGMENT_POINTER(setup,
                                                               USB_Setup_TypeDef,
                                                               MEM_MODEL_SEG));

/***************************************************************************//**
 * @brief
 *  USB set interface callback function.
 *
 * @details
 *  Called each time the SET_INTERFACE request is made.
 *
 * @param interface
 *  Number of the interface to set.
 *
 * @param altSetting
 *  Alternate setting for the interface
 *
 * @return
 *  @ref USB_STATUS_OK if the alternate interface is valid and can be set,
 *  @ref USB_STATUS_REQ_ERR otherwise
 ******************************************************************************/
USB_Status_TypeDef USBD_SetInterfaceCb(uint8_t interface, uint8_t altSetting);

/***************************************************************************//**
 * @brief
 *  Queries the application to see if a remote wakeup occurred.
 * @details
 *  If remote wakeup is enabled via @ref SLAB_USB_REMOTE_WAKEUP_ENABLED, the
 *  USB library will query the application after waking from suspend to see if
 *  the remote wakeup source was the reason for the wakeup. If this function
 *  returns True, the library will exit suspend mode and the application should
 *  call @ref USBD_RemoteWakeup() to wake up the host.
 * @return
 *  True if the remote wakeup source was the reason the device woke from
 *  suspend, false otherwise.
 *
 ******************************************************************************/
bool USBD_RemoteWakeupCb(void);

/***************************************************************************//**
 * @brief
 *  Delays 10 - 15 ms while resume signaling is active during a remote
 *  wakeup event
 *
 ******************************************************************************/
void USBD_RemoteWakeupDelay(void);

/***************************************************************************//**
 * @brief
 *  Processes USB events when the library is configured for polled mode
 *
 * @ details
 *  The USB library can be configured for interrupt
 *  (@ref SLAB_USB_POLLED_MODE == 0) or polled (@ref SLAB_USB_POLLED_MODE == 1)
 *  mode.
 *
 *  When in interrupt mode, the USB interrupt handler will trigger
 *  when a USB event occurs. Callback functions will be called as needed.
 *
 *  When in polled mode, the application must call USBD_Run() periodically to
 *  check for and process USB events. This may be useful in complex systems or
 *  when using an RTOS to perform all USB processing in the main loop instead
 *  of in the interrupt context.
 *
 ******************************************************************************/
void USBD_Run(void);

/***************************************************************************//**
 * @brief
 *  USB transfer complete callback function.
 *
 * @details
 *  Called each time a packet is sent on an IN endpoint or received on an
 *  OUT endpoint.
 *
 * @param epAddr
 *  Endpoint on which the transfer occurred
 *
 *  @param status
 *    Status of the endpoint
 *
 *  @param xferred
 *    For bulk, interrupt, and control transfers:
 *      Number of bytes transferred since the last USBD_Write() or USBD_Read()
 *      call.
 *    For isochronous IN transfers:
 *      This parameter is not used
 *    For isochronous OUT transfers:
 *      the number of bytes received in the last packet
 *
 *  @param remaining
 *    For bulk, interrupt, and control transfers:
 *      Number of bytes left to send or receive on the endpoint
 *    For isochronous transfers:
 *      The current index into the circular buffer holding isochronous data
 *
 * @return
 *    For bulk, interrupt, and control transfers:
 *      '0'
 *    For isochronous IN transfers:
 *      the number of bytes to transmit in the next packet
 *    For isochronous OUT ransfers:
 *      '0'
 ******************************************************************************/
uint16_t USBD_XferCompleteCb(uint8_t epAddr, \
                             USB_Status_TypeDef status, \
                             uint16_t xferred, \
                             uint16_t remaining);

/**  @} (end addtogroup efm8_callbacks Callback Functions) */

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
// -------------------- FIFO Access Functions  ---------------------------------
void USB_ReadFIFO(uint8_t fifoNum, uint8_t numBytes, uint8_t *dat);
void USB_WriteFIFO(uint8_t fifoNum, uint8_t numBytes, uint8_t *dat, bool txPacket);
/// @endcond DO_NOT_INCLUDE_WITH_DOXYGEN

// -------------------- Include Files ------------------------------------------

 // Error if peripheral driver not in use
 #include "usb_0.h"

/** @} (end addtogroup Efm8_usb) */

#endif // __SILICON_LABS_EFM8_USB_H__
