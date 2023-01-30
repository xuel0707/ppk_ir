include $(RTE_SDK)/mk/rte.vars.mk

APP = rsp_benchmark

CFLAGS += $(WERROR_FLAGS)
CFLAGS += -Wno-sign-compare -Wno-deprecated-declarations -O3

# all source are stored in SRCS-y
SRCS-y := main.c

LDLIBS += -L$(RTE_SDK)/app/rsp_benchmark/ -l:libcrypto_api.a

#ifeq ($(CONFIG_RTE_LIBRTE_PMD_CRYPTO_SCHEDULER),y)
LDLIBS += -lrte_pmd_crypto_scheduler
#endif

include $(RTE_SDK)/mk/rte.app.mk
