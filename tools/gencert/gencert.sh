

if [[ $# != "3" ]]
then

    echo "usage: $0 <ca-key> <public-key-hex> <out-cert>"
    exit 1

fi

key=$1
pub=$2
cert=$3

./hex2pubkey $pub pubkey.pem
./signcert $key pubkey.pem $cert
cat $cert | python cbytes.py

