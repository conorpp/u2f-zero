

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
[[ "$?" -ne "0" ]] && exit 1
signcert $key pubkey.pem $cert
[[ "$?" -ne "0" ]] && exit  2
cbytes.py $cert
[[ "$?" -ne "0" ]] && exit 3

exit 0
