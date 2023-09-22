include $(RTE_SDK)/mk/rte.vars.mk
# build directories
RSP_CRYPTO_DIR := $(RTE_SDK)/drivers/crypto/rsp
VPATH=$(RSP_CRYPTO_DIR)

#
# add for include rte_bus_pci.h
#
RSP_BUS_PCI_DIR = $(RTE_SDK)/drivers/bus/pci

# external library include paths
CFLAGS += -I$(SRCDIR)
CFLAGS += -I$(RSP_CRYPTO_DIR) -I$(RSP_BUS_PCI_DIR)
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-but-set-variable
# library symmetric crypto source files
ifeq ($(CONFIG_RTE_LIBRTE_CRYPTODEV),y)
ifeq ($(CONFIG_RTE_LIBRTE_PMD_RSP_ASYM),y)
	LDLIBS += -lrte_cryptodev
# 	LDLIBS += -lcrypto
	CFLAGS += -DBUILD_RSP_ASYM
	SRCS-y += rsp_asym.c
	SRCS-y += rsp_asym_sess.c
	SRCS-y += rsp_asym_pmd.c
	build_rsp = yes
endif
ifeq ($(CONFIG_RTE_LIBRTE_PMD_RSP_SYM),y)
	LDLIBS += -lrte_cryptodev
# 	LDLIBS += -lcrypto
	CFLAGS += -DBUILD_RSP_SYM
#    CFLAGS += -g
	SRCS-y += rsp_sym.c
	SRCS-y += rsp_session.c
	SRCS-y += rsp_sym_pmd.c
	SRCS-y += rsp_bulk.c
	SRCS-y += rsp_bulk_pmd.c
	SRCS-y += rpu_fw_data.c

	build_rsp = yes
endif
endif

ifdef build_rsp

	# library name
	LIB = librte_pmd_rsp.a
	LIBABIVER := 1
	# build flags
	CFLAGS += $(WERROR_FLAGS)
	CFLAGS += -O3
#    CFLAGS += -g
	# library common source files
	SRCS-y += rsp_device.c
	SRCS-y += rsp_common.c
	SRCS-y += rsp_logs.c
	SRCS-y += rsp_qp.c

	LDLIBS += -lrte_eal -lrte_mbuf -lrte_mempool
	LDLIBS += -lrte_pci -lrte_bus_pci
endif

include $(RTE_SDK)/mk/rte.lib.mk
