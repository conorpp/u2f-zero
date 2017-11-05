#!/bin/bash


ret=$(curl --request POST http://127.0.0.1:4040/ --data "port=$2" --data "firmware=$(cat "$1")")

if [[ $ret != *"Success"* ]]
then
    exit 1
fi

exit 0
#export FW=$2

#PORT=$1 python - <<END

#import requests, sys, os


#url = 'http://127.0.0.1:4040/'

#payload = {'port': os.environ['PORT'],  'firmware': open(os.environ['FW'], 'r').read()}

#print requests.post(url, data = payload)

#END
