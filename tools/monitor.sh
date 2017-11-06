#!/bin/bash


export PATH=$PATH:`pwd`/flashing:../../../u2f_zero_client:../../../gencert

export attest_priv=gencert/ca/key.pem
export attest_pub=gencert/ca/attest.der
adapters[0]=0
adapters[1]=COM3
adapters[2]=COM4
num_adapters=2
firmware=../firmware
export setup=setup_device.sh
export starting_SN=DAFE1E340AB70000
setup_SNs=(0 CAFEBABEFFFFFFF0 CAFEBABEFFFFFFF1 CAFEBABEFFFFFFF2)

if [[ -n "$1" ]] ; then
    starting_SN=$((0x$starting_SN + $1))
fi

function inc_hex {
    a=$((0x$1 + 1))
    a=$(printf "%x\n" $a)
    echo ${a^^}
}

function is_running {

    sn=$1
    if [[ -f workers/$sn/worker/.finished ]] ; then
        pwd
        echo "$sn finished"
        return 1
    fi
    
    pwd
    echo "$sn still running"
    return 0

}

function remove_lock {

    sn=$1
    touch workers/$sn/worker/.finished 

}

function start_programming {

    sn=$1
    setupf=$2
    setup_sn=$3
    rm -f workers/$sn/worker/.finished
    cd workers/$sn/worker && ./$setup ../../../$attest_priv ../../../$attest_pub $sn $starting_SN $setupf $setup_sn \
        && touch .finished && cd ../../.. &
    export starting_SN=$(inc_hex $starting_SN)

}


#for i in `seq 1 100` ; do

    #adapters[$i]=$(FlashUtilCL.exe DeviceSN $i)

    #if [[ ${adapters[$i]} = *"out of range"* ]]
    #then
        #break
    #fi

    #num_adapters=$(($num_adapters + 1))
#done

export num_adapters=$num_adapters
export adapters=$adapters

rm -rf workers
mkdir workers

for i in `seq 1 $num_adapters` ; do

    echo "$i :" ${adapters[$i]}
    mkdir workers/${adapters[$i]}
    mkdir workers/${adapters[$i]}/worker
    cp -rf $firmware workers/${adapters[$i]}
    cp  $setup workers/${adapters[$i]}/worker
    touch workers/${adapters[$i]}/worker/.finished

    # fix path in meta file to point to new location for files that are "dynamic"
    sed -i "s/firmware.*src.*cert.c/tools\/workers\/${adapters[$i]}\/firmware\/src\/cert.c/g"  \
        workers/${adapters[$i]}/firmware/release/src/cert.__i 
    sed -i "s/firmware.*src.*descriptors.c/tools\/workers\/${adapters[$i]}\/firmware\/src\/descriptors.c/g"  \
        workers/${adapters[$i]}/firmware/release/src/descriptors.__i

done

#cd workers/${adapters[1]}/worker && ./$setup ../../../$key ${adapters[1]} CAFEBABE00000001

starting_SN=$(inc_hex $starting_SN)
echo $starting_SN

#cd workers/${adapters[1]}/worker && ./$setup ../../../$key ${adapters[1]} CAFEBABE00000001 &
#is_running ${adapters[1]}
#echo $?

#exit 0

c_stack=(0 0 0 0)

for (( ; ; ))
do

    read -n 1 j

    if [[ $j -gt $num_adapters ]] ; then

        echo "$j is too big"
        continue

    fi
    
    # 3 element queue
    c_stack+=($j)
    c_stack=("${c_stack[@]:1}")

    if [[ "${c_stack[1]}" -eq "${c_stack[2]}" ]] ; then

        if [[ "${c_stack[2]}" -eq "${c_stack[3]}" ]] ; then

            # if entered 3 times in a row, remove lock
            remove_lock ${adapters[$j]}
            c_stack=(0 0 0 0)
            echo
            echo
            echo "restarting $j: ${adapters[$j]}"
            echo
            echo

        fi

    fi

    jobs



    is_running ${adapters[$j]}

    if [[ $? -eq 1 ]] ; then
        echo "starting $j ${adapters[$j]}"
        start_programming ${adapters[$j]} ../firmware/SETUP_"${setup_SNs[$j]}".hex ${setup_SNs[$j]}
    else
        echo "$j ${adapters[$j]} is already running"
    fi

done

