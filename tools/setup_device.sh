#!/bin/bash


if [[ $# != "1" ]]
then

    echo "usage: $0 <ca-key>"
    exit 1

fi

export PATH=$PATH:gencert:hid_config:insert_key

# setup atecc
config.py pubkey.hex

# generate cert
gencert.sh "$1" "$(cat pubkey.hex)" attest.der > pubkey.c.txt

# add key to build
insert.py ../firmware/src/u2f-atecc.c pubkey.c.txt ../firmware/src/u2f-atecc.c

echo "now rebuild and program"
