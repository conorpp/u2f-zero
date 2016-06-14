

U2F Zero
========

U2F Zero is an open source two factor authentication token that is implented securely.  It works with Google accounts, Github, Duo, OpenSSH, and anything else supporting U2F.

![](http://i.imgur.com/lL3VLz1.jpg)


You can easily [build your own](https://github.com/conorpp/u2f-zero/wiki/Building-a-U2F-Token).  You just need to order the 
[8 SMT parts](https://github.com/conorpp/u2f-zero/wiki/Parts-List#smt-parts), [$4.6 PCBs](https://oshpark.com/shared_projects/Rq9evtm7), and [programmer](http://www.digikey.com/product-detail/en/silicon-labs/DEBUGADPTR1-USB/336-1182-ND/807653).
It ends up being $35 for programmer and $4.5/board.  

Check out [the wiki](https://github.com/conorpp/u2f-zero/wiki) for more on how to [build your own](https://github.com/conorpp/u2f-zero/wiki/Building-a-U2F-Token).

![](http://i.imgur.com/s8VcUNT.jpg)

Security Overview
-----------------

The security level is about the same as a car key.  Any secret information cannot be read or duplicated.  A TRNG is used to generate unpredictable keys.  

However, side channel leakage is an unsolved problem in industry and academia.  So for well equipped adversaries that can make targetted attacks and get physical access, secret information leakage is possible.  Any other hardware token that claims it's "impenetrable" or otherwise totally secure are *still* vulnerable to physical side channels and it's important to acknowledge.  However, most people don't worry about targeted attacks from well equipped adversaries.

For more information about U2F Zero's secure implementation and the problem of side channels, check out [the wiki](https://github.com/conorpp/u2f-zero/wiki/Security-Overview).


