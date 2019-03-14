# This project has been replaced by Solo security key

This project is superseded by the [open source FIDO2 token, Solo](https://github.com/SoloKeysSec/solo).  It is still
in development, but offers:

* FIDO2 + U2F + more secure
* Easier to build & customize
* Just as cheap as U2F Zero

U2F Zero is no longer maintained.

U2F Zero
========

U2F Zero is an open source U2F token for 2 factor authentication.  It is implemented securely.  It works with Google accounts, Github, Duo, and anything else supporting U2F.  The latest version uses key derivation and has no limit on registrations.
![](http://i.imgur.com/dQpo9wC.jpg "The door keys are not used")

You can easily [build your own](https://github.com/conorpp/u2f-zero/wiki/Building-a-U2F-Token).  You just need to order the 
[8 SMT parts](https://github.com/conorpp/u2f-zero/wiki/Parts-List#smt-parts), [$1.13-$3.5 per PCB](https://github.com/conorpp/u2f-zero/wiki/Parts-List#pcbs), and [programmer](http://www.digikey.com/product-detail/en/silicon-labs/DEBUGADPTR1-USB/336-1182-ND/807653).
It ends up being $35 for programmer and ~$5/board.  The token should be durable enough to survive on a key chain for years, even after going through the wash.

Check out [the wiki](https://github.com/conorpp/u2f-zero/wiki) for more on how to [build your own](https://github.com/conorpp/u2f-zero/wiki/Building-a-U2F-Token).

#### Redesign / FIDO 2

FIDO 2 protocol is replacing U2F.  It has more flexibility and support for password-less login.  A new open source FIDO 2 token is being planned (with support for USB, Bluetooth, and NFC).  Discussion for hardware design and usage [is happening here](https://github.com/conorpp/u2f-zero/issues/76), feel free to chip in.  

Security Overview
-----------------

The security level is about the same as a modern car key.  Any secret information cannot be read or duplicated.  A true random number generator is used to create unpredictable keys.  

However, side channel leakage is an unsolved problem in industry and academia.  So for well equipped adversaries that can make targetted attacks and get physical access, secret information leakage is possible.  Any other hardware token that claims it's "impenetrable" or otherwise totally secure is *still* vulnerable to physical side channels and it's important to acknowledge.  However, most people don't worry about targeted attacks from well equipped adversaries.

For more information about U2F Zero's secure implementation and the problem of side channels, check out [the wiki](https://github.com/conorpp/u2f-zero/wiki/Security-Overview).

Support
--------

Support this project by purchasing or sharing [U2F Zero on Amazon](https://www.amazon.com/U2F-Zero/dp/B01L9DUPK6/ref=lp_15603989011_1_1?srs=15603989011&ie=UTF8&qid=1501811252&sr=8-1).


License
-------

Everything is open source and licensed under the [Simplified BSD License](https://github.com/conorpp/u2f-zero/blob/master/LICENSE.txt).
