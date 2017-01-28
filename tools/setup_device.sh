#!/bin/bash

SETUP_HEX=../firmware/SETUP.hex
FINAL_HEX=../firmware/release/u2f-firmware.hex
FLASH_TOOLS=0
SN=
SN_build=
SN_setup=

if [[ $# != "1" ]] && [[ $# != "5" ]]
then

    echo "usage: $0 <ca-key> [debugger-SN new-SN-for-U2F-token setup-hex-file setup-SN]"
    exit 1

fi


if [[ $# != "1" ]] ; then
    FLASH_TOOLS=1
    SN=$2
    SN_build=$3
    SETUP_HEX=$4
    SN_setup=$5
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

if [[ -n $SN_setup ]] ; then
    client.py configure pubkey.hex -s $SN_setup >/dev/null
else
    client.py configure pubkey.hex >/dev/null
fi

while [[ "$?" -ne "0" ]] ; do
    sleep .2

    if [[ -n $SN_setup ]] ; then
        client.py configure pubkey.hex -s $SN_setup
    else
        client.py configure pubkey.hex
    fi

done


echo "generate attestation certificate..."
gencert.sh "$1" "$(cat pubkey.hex|head -n 1)" attest.der > ../firmware/src/cert.c
[[ "$?" -ne "0" ]] && exit 1

wkey=$(cbytes.py "$(cat pubkey.hex|head -n 2|tail -n1)" -s)
[[ "$?" -ne "0" ]] && exit 1

rkey=$(cbytes.py "$(cat pubkey.hex|tail -n 1)" -s)
[[ "$?" -ne "0" ]] && exit 1


echo "" >> ../firmware/src/cert.c
echo "code uint8_t WMASK[] = $wkey;" >> ../firmware/src/cert.c
echo "code uint8_t RMASK[] = $rkey;" >> ../firmware/src/cert.c


if [[ -n $SN_build ]] ; then
    echo "setting SN to $SN_build"
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

while [[ "$?" -ne "0" ]] ; do
    sleep .2
    program.sh prog.hex $SN
done

[[ "$?" -ne "0" ]] && exit 1

echo "waiting to unplug"
sleep 0.2

while [[ "$?" -eq 0 ]] ; do

    sleep 0.5
    client.py wink -s "$SN_build"

done

echo "done."
