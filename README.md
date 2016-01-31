

U2F Zero
========





Overview
=======

U2F Zero is an affordable and physically secure two factor authentication token that implements
the [U2F protocol](https://fidoalliance.org/specifications/overview/).

Firmware
========

### Code overview

The HID layer for U2F is implemented in `u2f_hid.c`.  The USB
boilerplate is in `callback.c`.

Firmware generally works as follows:

* Main loop adds a request for USB to read a 64 byte packet
* `USBD_XferCompleteCb` gets called with packet when it comes
* USB HID layer gets called
* USB HID layer will call U2F layer when appropriate

[U2F HID layer spec](https://fidoalliance.org/specs/fido-u2f-v1.0-nfc-bt-amendment-20150514/fido-u2f-hid-protocol.html)

[U2F layer spec](https://fidoalliance.org/specs/fido-u2f-v1.0-nfc-bt-amendment-20150514/fido-u2f-raw-message-formats.html)

### Using Simplicity Studio

* Open Simplicity Studio
* Click File -> Import
* General -> Existing Projects into Workspace
* Select root directory and choose the `firmware/` directory
* Finish


### Using command line tools

* TODO
