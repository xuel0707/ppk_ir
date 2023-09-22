CDIR := $(dir $(lastword $(MAKEFILE_LIST)))
APP = polymorphic
include $(CDIR)/../makefiles/dpdk_backend_pre.mk
include $(CDIR)/../makefiles/common.mk
include $(CDIR)/../makefiles/hw_independent.mk
VPATH += ./ir
EXTRA_CFLAGS += 
LDFLAGS += 
SRCS-y += dpdk_nicon.c
SRCS-y += dpdk_model_v1model.c
SRCS-y += dpdkx_smem.c
SRCS-y += stateful_memory.c
CFLAGS += 
include $(CDIR)/../makefiles/dpdk_backend_post.mk
