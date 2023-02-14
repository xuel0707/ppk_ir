#!/bin/bash

set -e

sudo apt-get update

sudo apt-get install build-essential -y

sudo apt-get install net-tools tcpdump curl git -y

sudo apt-get install libgc-dev libfl-dev  libboost-iostreams-dev \
    libboost-graph-dev llvm  python-scapy python-ipaddr python-ply \
    libreadline-dev -y

ubuntu_release=`lsb_release -s -r`
if [[ "${ubuntu_release}" = "18" ]]
then
    # This older package libssl1.0-dev enables compiling Thrift 0.9.2
    # on Ubuntu 18.04.  Package libssl-dev exists, but Thrift 0.9.2
    # fails to compile when it is installed.
    # TBD: whether using this package makes a difference for Ubuntu
    # 18.04 and Thrift 0.11.0.
    LIBSSL_DEV="libssl1.0-dev"
else
    LIBSSL_DEV="libssl-dev"
fi
sudo apt-get install -y \
    automake \
    cmake \
    libjudy-dev \
    libgmp-dev \
    libpcap-dev \
    libboost-dev \
    libboost-test-dev \
    libboost-program-options-dev \
    libboost-system-dev \
    libboost-filesystem-dev \
    libboost-thread-dev \
    libevent-dev \
    libtool \
    flex \
    bison \
    pkg-config \
    g++ \
    libssl-dev \
    libffi-dev \
    python-dev \
    python-pip \
    wget

python -m pip install psutil
python -m pip install crcmod

