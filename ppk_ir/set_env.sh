#!/bin/bash
#get archtecture of the CPU 
os_bit=$(uname -m | awk '{print " | "$0}')
ppk_path=$(pwd)
echo $ppk_path
if [[ $os_bit =~ "x86_64" ]];then
   export DPDK_VSN=19.11.12
   export RTE_SDK=$ppk_path/dpdk-stable-19.11.12/
   export RTE_TARGET=x86_64-native-linuxapp-gcc
   export P4C=$ppk_path/p4c	
   export PPK=ppk
elif [[ $os_bit =~ "aarch64" ]] && [[ $USER =~ "pinet" ]];then
   export DPDK_VSN=Marvell-v8
   export RTE_SDK=$ppk_path/dpdk-stable-19.11.12/
   export RTE_TARGET=arm64-armv8a-linuxapp-gcc
   export P4C=$ppk_path/p4c
   export PPK=ppk
elif [[ $os_bit =~ "aarch64" ]];then
   export DPDK_VSN=Marvell-v8
   export RTE_SDK=$ppk_path/dpdk-Marvell-v8
   export RTE_TARGET=arm64-octeontx2-linuxapp-gcc
   export P4C=$ppk_path/p4c
   export PPK=ppk_arm64
else
   echo "unknown archtecture"
fi
