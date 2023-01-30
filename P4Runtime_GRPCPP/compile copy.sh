#!/bin/bash

DEBUG=""

if [ "$1" = "-D" ];then
	DEBUG="-g"
fi

GRPCDIR="../grpc-c/build"

T4P4SDIR="../t4p4s"

#LFLAGS="${GRPCDIR}/lib/libgrpc-c.la -lgrpc -lgpr -lprotobuf-c -lpthread"
LFLAGS="-lgrpc -lgpr -lprotobuf-c -lpthread"



#IFLAGS="-I. -I${GRPCDIR}/lib/h/ -I${GRPCDIR}/third_party/protobuf-c -I${GRPCDIR}/third_party/grpc/include -I./grpc-c-out/ -I./server/ -I../PI/proto/server -I${T4P4SDIR}/src/hardware_dep/shared/ctrl_plane"
IFLAGS="-I. -I./grpc-cpp-out/ -I./cpp-out/ -I./server/ -I./${T4P4SDIR}/src/hardware_dep/shared/ctrl_plane -I./../PI/proto/server"

#PROTO_SOURCES="grpc-c-out/p4/tmp/p4config.grpc-c.service.c grpc-c-out/p4/tmp/p4config.grpc-c.c grpc-c-out/p4/v1/p4data.grpc-c.service.c grpc-c-out/p4/v1/p4runtime.grpc-c.service.c grpc-c-out/p4/v1/p4data.grpc-c.c grpc-c-out/p4/v1/p4runtime.grpc-c.c grpc-c-out/p4/config/v1/p4types.grpc-c.c grpc-c-out/p4/config/v1/p4types.grpc-c.service.c grpc-c-out/p4/config/v1/p4info.grpc-c.service.c grpc-c-out/p4/config/v1/p4info.grpc-c.c grpc-c-out/google/rpc/status.grpc-c.service.c grpc-c-out/google/rpc/status.grpc-c.c grpc-c-out/google/rpc/code.grpc-c.c grpc-c-out/google/rpc/code.grpc-c.service.c grpc-c-out/gnmi/gnmi.grpc-c.c grpc-c-out/gnmi/gnmi.grpc-c.service.c grpc-c-out/google/protobuf/any.grpc-c.c grpc-c-out/google/protobuf/descriptor.grpc-c.c"

PROTO_PB_S="cpp-out/p4/v1/p4data.pb.cc cpp-out/p4/v1/p4runtime.pb.cc cpp-out/p4/config/v1/p4info.pb.cc cpp-out/p4/config/v1/p4types.pb.cc cpp-out/p4/tmp/p4config.pb.cc cpp-out/google/rpc/status.pb.cc cpp-out/google/rpc/code.pb.cc cpp-out/gnmi/gnmi.pb.cc"

PROTO_SOURCES="grpc-cpp-out/p4/v1/p4data.grpc.pb.cc grpc-cpp-out/p4/v1/p4runtime.grpc.pb.cc grpc-cpp-out/p4/config/v1/p4info.grpc.pb.cc grpc-cpp-out/p4/config/v1/p4types.grpc.pb.cc grpc-cpp-out/p4/tmp/p4config.grpc.pb.cc grpc-cpp-out/google/rpc/status.grpc.pb.cc grpc-cpp-out/google/rpc/code.grpc.pb.cc grpc-cpp-out/gnmi/gnmi.grpc.pb.cc"

OBJECTS=""

echo "Compiling device_mgr.c"
#gcc ${DEBUG} ${IFLAGS} -c -o obj/device_mgr.o server/device_mgr.c
g++ ${DEBUG} ${IFLAGS} -std=c++11 -fpermissive `pkg-config --cflags protobuf grpc` -c -o obj/device_mgr.o server/device_mgr.c

#gcc ${DEBUG} ${IFLAGS} -c -o obj/pi_server.o server/pi_server.c
echo "Compiling pi_server.cc"
g++ ${DEBUG} ${IFLAGS} -std=c++11 `pkg-config --cflags protobuf grpc` -c -o obj/pi_server.o server/pi_server.cc

echo "LINKD"
g++ ${DEBUG} ${IFLAGS} -o async_client ${OBJECTS} obj/map.o obj/uint128.o obj/device_mgr.o obj/pi_server.o obj/async_client.o -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc` -pthread -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl

echo "Creating static lib"
mkdir -p static_lib
ar rcs static_lib/libp4rt.a obj/*.o

echo "Static library is available in folder ./static_lib"
#echo "Usage example:"
#echo "gcc ${IFLAGS} server/test_server.c -L./static_lib -lp4rt -L${GRPCDIR}/lib/.libs -lgrpc-c ${LFLAGS} -o test_server"

