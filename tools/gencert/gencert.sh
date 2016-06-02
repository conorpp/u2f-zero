

if [[ $# != "3" ]]
then

    echo "usage: $0 <ca-key> <public-key-hex> <out-cert>"
    exit 1

fi

export PATH=$PATH:.

key=$1
pub=$2
cert=$3

hex2pubkey $pub pubkey.pem
signcert $key pubkey.pem $cert
cbytes.py $cert

