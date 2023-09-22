# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2010-2016 Intel Corporation

include $(RTE_SDK)/mk/rte.vars.mk

#
# library name
#
LIB = librte_pmd_txgbe.a

CFLAGS += -DALLOW_EXPERIMENTAL_API
CFLAGS += -O3
CFLAGS += $(WERROR_FLAGS)

EXPORT_MAP := rte_pmd_txgbe_version.map

ifeq ($(CONFIG_RTE_TOOLCHAIN_ICC),y)
#
# CFLAGS for icc
#
CFLAGS_BASE_DRIVER  = -diag-disable 174 -diag-disable 593 -diag-disable 869
CFLAGS_BASE_DRIVER += -diag-disable 981 -diag-disable 2259

CFLAGS_txgbe_rxtx.o += -diag-disable 3656

else ifeq ($(CONFIG_RTE_TOOLCHAIN_CLANG),y)
#
# CFLAGS for clang
#
CFLAGS_BASE_DRIVER = -Wno-unused-parameter -Wno-unused-value
CFLAGS_BASE_DRIVER += -Wno-strict-aliasing -Wno-format-extra-args

else
#
# CFLAGS for gcc
#
ifeq ($(shell test $(GCC_VERSION) -ge 44 && echo 1), 1)
CFLAGS     += -Wno-deprecated
endif
CFLAGS_BASE_DRIVER = -Wno-unused-parameter -Wno-unused-value
CFLAGS_BASE_DRIVER += -Wno-strict-aliasing -Wno-format-extra-args

ifeq ($(shell test $(GCC_VERSION) -ge 50 && echo 1), 1)
ifeq ($(shell test $(GCC_VERSION) -ge 70 && echo 1), 1)
CFLAGS_BASE_DRIVER += -Wno-implicit-fallthrough
endif
endif

endif
LDLIBS += -lrte_eal -lrte_mbuf -lrte_mempool -lrte_ring
LDLIBS += -lrte_ethdev -lrte_net -lrte_kvargs -lrte_hash
LDLIBS += -lrte_bus_pci
LDLIBS += -lpthread

#
# Add extra flags for base driver files (also known as shared code)
# to disable warnings in them
#
BASE_DRIVER_OBJS=$(sort $(patsubst %.c,%.o,$(notdir $(wildcard $(SRCDIR)/base/*.c))))
$(foreach obj, $(BASE_DRIVER_OBJS), $(eval CFLAGS_$(obj)+=$(CFLAGS_BASE_DRIVER)))

VPATH += $(SRCDIR)/base

#
# all source are stored in SRCS-y
#
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_phy.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_eeprom.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_hw.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_vf.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_hv_vf.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_dcb.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_dcb_hw.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_mbx.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_rxtx.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_ethdev.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_ethdev_vf.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_fdir.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_pf.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_flow.c

ifeq ($(CONFIG_RTE_LIBRTE_SECURITY),y)
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_ipsec.c
endif
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += rte_pmd_txgbe.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_tm.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_vf_representor.c

SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_ptypes.c
SRCS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe_mng.c

# install this header file
SYMLINK-$(CONFIG_RTE_LIBRTE_TXGBE_PMD)-include := rte_pmd_txgbe.h

include $(RTE_SDK)/mk/rte.lib.mk
