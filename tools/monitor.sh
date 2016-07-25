#!/bin/bash


export PATH=$PATH:`pwd`/flashing:../../../u2f_zero_client:../../../gencert

key=gencert/ca/key.pem
adapters[0]=0
num_adapters=0
firmware=../firmware
setup=setup_device.sh

for i in `seq 1 100` ; do

	adapters[$i]=$(FlashUtilCL.exe DeviceSN $i)

	if [[ ${adapters[$i]} = *"out of range"* ]]
	then
		break
	fi

	num_adapters=$(($num_adapters + 1))
done

rm -rf workers
mkdir workers

for i in `seq 1 $num_adapters` ; do
	echo ${adapters[$i]}
	mkdir workers/${adapters[$i]}
	mkdir workers/${adapters[$i]}/worker
	cp -rf $firmware workers/${adapters[$i]}
	cp  $setup workers/${adapters[$i]}/worker
done

cd workers/${adapters[1]}/worker && ./$setup ../../../$key ${adapters[1]} CAFEBABE00000001




