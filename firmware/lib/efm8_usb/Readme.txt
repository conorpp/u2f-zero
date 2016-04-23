-------------------------------------------------------------------------------
 Readme.txt
-------------------------------------------------------------------------------

Copyright 2014 Silicon Laboratories, Inc.
http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt

Program Description:
-------------------

This is the generic EFM8 USB Firmware Library. Please see the EFM8 Libraries 
Documentation for more information (/doc/EFM8/software/Lib/index.html).

Known Issues and Limitations:
----------------------------

1) The library does not reset its Data Toggle after receiving a SET_INTERFACE 
   request.

Target and Tool Chain Information:
---------------------------------

Target:         EFM8UB1, EFM8UB2, C8051F320/1, C8051F326/7, C8051F34x, C8051F38x
Tool chain:     Keil

File List:
---------

/inc/efm8_usb.h
/src/efm8_usbd.c
/src/efm8_usbdch9.c
/src/efm8_usbdep.c
/src/efm8_usbdint.c

Release Information:
-------------------

Version 1.0.0
    - Initial release.

Version 1.0.1
    - Fixed bug in logic of remote wakeup feature where the device would 
      attempt to wake the host before enabling its USB transceiver.
    - Fixed bug where the device would stall the Data Phase instead of the 
      Setup Phase when sending a procedural stall on Endpoint 0.
    - Fixed bug where a bus-powered device would look at VBUS after a USB Reset
      to determine if it should enter the Default or Attached State. VBUS is 
      always present on a bus-powered device, so it should automatically enter 
      the Default State.
    - Removed code that generated a compiler warning when
      USB_PWRSAVE_MODE_FASTWAKE was enabled.
    - Improved documentation of USB_PWRSAVE_MODE_FASTWAKE feature.

-------------------------------------------------------------------------------
 End Of File
-------------------------------------------------------------------------------
