cmd_rte_eth_bond_flow.o = gcc -Wp,-MD,./.rte_eth_bond_flow.o.d.tmp  -m64 -pthread -I/home/zhaoxin/ppk_sw/dpdk-stable-19.11/lib/librte_eal/linux/eal/include  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_RDSEED -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C  -I/home/zhaoxin/ppk_sw/dpdk-stable-19.11/x86_64-native-linux-gcc/include -I/home/zhaoxin/ppk_sw/dpdk-stable-19.11/eqt/gmssl/include  -DRTE_USE_FUNCTION_VERSIONING -include /home/zhaoxin/ppk_sw/dpdk-stable-19.11/x86_64-native-linux-gcc/include/rte_config.h -D_GNU_SOURCE -DALLOW_EXPERIMENTAL_API -O3 -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -Wdeprecated -O3 -Wno-address-of-packed-member -fPIC    -o rte_eth_bond_flow.o -c /home/zhaoxin/ppk_sw/dpdk-stable-19.11/drivers/net/bonding/rte_eth_bond_flow.c 
