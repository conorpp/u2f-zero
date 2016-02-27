#!/bin/bash

if [[ $# != "4" ]] ; 
then

    echo "usage: $0 <ca-cert> <ca-key> <cert-name> <key-name>"
    exit 1

fi

cacert=$1
cakey=$2
certname=$3
keyname=$4

openssl ecparam -genkey -name prime256v1 -out $keyname

openssl req -new -key $keyname -out "$keyname".csr 

openssl x509 -req -in "$keyname".csr  -CA $cacert -CAkey $cakey -CAcreateserial -outform DER \
    -out $certname -set_serial 1 -clrext -days $((365 * 6))
