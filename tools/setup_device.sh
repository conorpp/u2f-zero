#!/bin/bash

SETUP_HEX=../firmware/SETUP.hex
FINAL_HEX=../firmware/release/u2f-firmware.hex
FLASH_TOOLS=1
SN=
SN_build=

if [[ $# != "1" ]] && [[ $# != "2" ]] && [[ $# != "3" ]]
then

    echo "usage: $0 <ca-key> [debugger-SN] [new-SN-for-U2F-token]"
    exit 1

fi


if [[ $# != "1" ]] ; then
	SN=$2
    if [[ $# = "3" ]] ; then
        SN_build=$3
    fi
fi

export PATH=$PATH:gencert:u2f_zero_client:flashing

if [[ $FLASH_TOOLS = 1 ]] 
then

    # setup atecc
    echo "erasing..."
    erase.sh $SN

    while [[ "$?" -ne "0" ]] ; do
        echo "$SN is retrying erase ... "
        sleep 0.2
        erase.sh $SN
    done

    echo "programming setup..."
    program.sh $SETUP_HEX $SN

    [[ "$?" -ne "0" ]] && exit 1

fi

echo "configuring..."

if [[ -n $SN_build ]] ; then
    client.py configure pubkey.hex -s $SN_build >/dev/null
else
    client.py configure pubkey.hex >/dev/null
fi

while [[ "$?" -ne "0" ]] ; do
    sleep .2
    client.py configure pubkey.hex
done


echo "generate attestation certificate..."
gencert.sh "$1" "$(cat pubkey.hex)" attest.der > ../firmware/src/cert.c

[[ "$?" -ne "0" ]] && exit 1

if [[ -n $SN_build ]] ; then
    sed -i "/#define SER_STRING.*/c\#define SER_STRING \"$SN_build\""  ../firmware/src/descriptors.c
    rm ../firmware/release/u2f-firmware.omf
fi

echo "done."
echo "building..."

if [[ $FLASH_TOOLS != 1 ]] 
then

    echo "Open Simplicity Studio and rebuild final program."
    echo "Then you can erase and reprogram U2F Token."
    exit 1

fi

PATH1=$PATH
cur=`pwd`
cd ../firmware/release && make all && cd $cur

[[ "$?" -ne "0" ]] && exit 1

export PATH=$PATH1

echo "programming final build..."
cp $FINAL_HEX prog.hex
program.sh prog.hex $SN
#rm prog.hex

while [[ "$?" -ne "0" ]] ; do
    sleep .2
    program.sh prog.hex $SN
done

[[ "$?" -ne "0" ]] && exit 1

echo "done."

