#!/bin/bash
##############################################################

# 打印脚本命令.
set -x
# 在有错误输出时停止运行.
set -e

#获得cpu核数，与某些软件的编译选项相关
#最好不要超过4,否则编译可能出错
NUM_CORES=`grep -c ^processor /proc/cpuinfo`

# cd /home/zhaoxin/ppk_sw

# 安装Mininet
mininet_install() {
tar -zxvf grpc.tar.gz
# tar -zxvf ppk.tar.gz
tar -zxvf protobuf.tar.gz
tar -zxvf PI.tar.gz
# tar -zxvf P4Runtime_GRPCPP.tar.gz
}

########################以下代码块的安装顺序不可改变###################################


protobuf_install() {
cd protobuf
make clean
export CFLAGS="-Os"
export CXXFLAGS="-Os"
export LDFLAGS="-Wl,-s"
./autogen.sh
./configure --prefix=/usr
make -j4
sudo make install
sudo ldconfig
unset CFLAGS CXXFLAGS LDFLAGS
# force install python module
cd python
# sudo python setup.py install
cd ../..
}


grpc_install() {
cd grpc
make clean
export LDFLAGS="-Wl,-s"
make -j4
sudo make install
sudo ldconfig
unset LDFLAGS
cd ..
# Install gRPC Python Package
# python -m pip install grpcio
}


bmv2_deps_install() {
cd behavioral-model
# From bmv2's install_deps.sh, we can skip apt-get install.
# Nanomsg is required by p4runtime, p4runtime is needed by BMv2...
./install_deps.sh
cd ..
}


PI_install() {
cd PI
make clean
./autogen.sh
./configure --with-proto
make -j4
sudo make install
sudo ldconfig
cd ..
}


bmv2_install() {
cd behavioral-model
./autogen.sh
./configure --with-pi # --enable-debugger --disable-logging-macros
make -j2
sudo make install
sudo ldconfig
# Simple_switch_grpc target
cd targets/simple_switch_grpc
./autogen.sh
./configure --with-thrift
make -j2
sudo make install
sudo ldconfig
cd ..
cd ..
cd ..
}


p4c_install() {
cd ppk/p4c
rm -rf build
mkdir -p build
cd build
cmake ..
make -j2
sudo make install
sudo ldconfig
cd ..
cd ..
}

#--------------- END --------------------

# Call functions above
mininet_install
protobuf_install
grpc_install
#bmv2_deps_install
PI_install
#bmv2_install
p4c_install

