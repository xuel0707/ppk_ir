# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2010-2016 Intel Corporation

include $(RTE_SDK)/mk/rte.vars.mk

#
# library name
#
LIB = librte_pmd_ngbe.a

CFLAGS += -DALLOW_EXPERIMENTAL_API
CFLAGS += -O3
CFLAGS += $(WERROR_FLAGS)

EXPORT_MAP := rte_pmd_ngbe_version.map

LIBABIVER := 1

ifeq ($(CONFIG_RTE_TOOLCHAIN_ICC),y)
#
# CFLAGS for icc
#
CFLAGS_BASE_DRIVER  = -diag-disable 174 -diag-disable 593 -diag-disable 869
CFLAGS_BASE_DRIVER += -diag-disable 981 -diag-disable 2259

CFLAGS_ngbe_rxtx.o += -diag-disable 3656

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
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_phy.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_phy_mtd.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_phy_rtl.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_phy_mvl.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_phy_zte.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_phy_yt.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_eeprom.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_hw.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_hw_sp.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_hw_em.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_vf.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_hv_vf.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_mbx.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_rxtx.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_ethdev.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_ethdev_vf.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_pf.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_flow.c
ifeq ($(CONFIG_RTE_ARCH_ARM64),y)
SRCS-$(CONFIG_RTE_NGBE_INC_VECTOR) += ngbe_rxtx_vec_neon.c
else
SRCS-$(CONFIG_RTE_NGBE_INC_VECTOR) += ngbe_rxtx_vec_sse.c
endif

ifeq ($(CONFIG_RTE_LIBRTE_SECURITY),y)
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_ipsec.c
endif
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += rte_pmd_ngbe.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_tm.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_vf_representor.c

SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_ptypes.c
SRCS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe_mng.c

# install this header file
SYMLINK-$(CONFIG_RTE_LIBRTE_NGBE_PMD)-include := rte_pmd_ngbe.h

include $(RTE_SDK)/mk/rte.lib.mk
