cmd_failsafe_args.o = gcc -Wp,-MD,./.failsafe_args.o.d.tmp  -m64 -pthread -I/home/aa/ppk_sw/ppk/dpdk-stable-19.11/lib/librte_eal/linux/eal/include  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_RDSEED -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I/home/aa/ppk_sw/ppk/dpdk-stable-19.11/x86_64-native-linuxapp-gcc/include -I/include  -DRTE_USE_FUNCTION_VERSIONING -include /home/aa/ppk_sw/ppk/dpdk-stable-19.11/x86_64-native-linuxapp-gcc/include/rte_config.h -D_GNU_SOURCE -DLINUX -DALLOW_EXPERIMENTAL_API -std=gnu99 -Wextra -O3 -I. -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700 -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -Wdeprecated -O3 -Wimplicit-fallthrough=2 -Wno-format-truncation -Wno-address-of-packed-member -Wno-strict-prototypes -pedantic -fPIC    -o failsafe_args.o -c /home/aa/ppk_sw/ppk/dpdk-stable-19.11/drivers/net/failsafe/failsafe_args.c 
