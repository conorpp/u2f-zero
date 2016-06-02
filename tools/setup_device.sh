#!/bin/bash

SETUP_HEX=../firmware/release/SETUP.hex
FINAL_HEX=../firmware/release/u2f-firmware.hex

if [[ $# != "1" ]]
then

    echo "usage: $0 <ca-key>"
    exit 1

fi

export PATH=$PATH:gencert:u2f_zero_client:flashing

# setup atecc
date +"%T"
echo "erasing..."
erase.sh

if [[ "$?" -ne "0" ]] ; then
    erase.sh
fi

[[ "$?" -ne "0" ]] && exit 1

date +"%T"
echo "programming setup..."
program.sh $SETUP_HEX

[[ "$?" -ne "0" ]] && exit 1

date +"%T"
echo "configuring..."
client.py configure pubkey.hex >/dev/null

if [[ "$?" -ne "0" ]] ; then
    sleep .2
    client.py configure pubkey.hex >/dev/null
fi

if [[ "$?" -ne "0" ]] ; then
    sleep .2
    client.py configure pubkey.hex
fi

[[ "$?" -ne "0" ]] && exit 1

date +"%T"
echo "generate attestation certificate..."
gencert.sh "$1" "$(cat pubkey.hex)" attest.der > ../firmware/src/cert.c

[[ "$?" -ne "0" ]] && exit 1

date +"%T"
echo "building..."

PATH1=$PATH
cd ../firmware/release && make all && cd ../../tools
export PATH=$PATH1


[[ "$?" -ne "0" ]] && exit 1

date +"%T"
echo "programming final build..."
cp $FINAL_HEX prog.hex
program.sh prog.hex
#rm prog.hex

[[ "$?" -ne "0" ]] && exit 1

date +"%T"
echo "done."

