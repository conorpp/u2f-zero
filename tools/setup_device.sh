#!/bin/bash

SETUP_HEX=../firmware/SETUP.hex
FINAL_HEX=../firmware/release/u2f-firmware.hex
FLASH_TOOLS=0

if [[ $# != "1" ]]
then

    echo "usage: $0 <ca-key>"
    exit 1

fi

export PATH=$PATH:gencert:u2f_zero_client:flashing

if [[ $FLASH_TOOLS = 1 ]] 
then

    # setup atecc
    echo "erasing..."
    erase.sh

    while [[ "$?" -ne "0" ]] ; do
        sleep .1
        erase.sh
    done

    echo "programming setup..."
    program.sh $SETUP_HEX

    [[ "$?" -ne "0" ]] && exit 1

fi

echo "configuring..."
client.py configure pubkey.hex >/dev/null

while [[ "$?" -ne "0" ]] ; do
    sleep .2
    client.py configure pubkey.hex >/dev/null
done


echo "generate attestation certificate..."
gencert.sh "$1" "$(cat pubkey.hex)" attest.der > ../firmware/src/cert.c

[[ "$?" -ne "0" ]] && exit 1

echo "done."
echo "building..."

if [[ $FLASH_TOOLS = 1 ]] 
then

    echo "Building from command line only works on Windows"
    echo "Open Simplicity Studio and rebuild final program."
    echo "Then you can erase and reprogram U2F Token."
    exit 1

fi

PATH1=$PATH
cd ../firmware/release && make all && cd ../../tools
export PATH=$PATH1


[[ "$?" -ne "0" ]] && exit 1

echo "programming final build..."
cp $FINAL_HEX prog.hex
program.sh prog.hex
#rm prog.hex

while [[ "$?" -ne "0" ]] ; do
    sleep .2
    program.sh prog.hex
done

[[ "$?" -ne "0" ]] && exit 1

echo "done."

