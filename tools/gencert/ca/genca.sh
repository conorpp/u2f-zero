#!/bin/bash

keyname=key.pem
certname=cert.pem
smallcertname=cert.der
curve=prime256v1

# generate EC private key
openssl ecparam -genkey -name "$curve" -out "$keyname"
# generate a "signing request"
openssl req -new -key "$keyname" -out "$keyname".csr
# self sign the request
openssl x509 -req -days 18250  -in "$keyname".csr -signkey "$keyname" -out "$certname"

# convert to smaller size format DER
openssl  x509 -in $certname  -outform der -out $smallcertname

