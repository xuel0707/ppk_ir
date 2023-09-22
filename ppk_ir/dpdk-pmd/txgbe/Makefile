# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2010-2015 Intel Corporation

include $(RTE_SDK)/mk/rte.vars.mk

ifeq ($(CONFIG_RTE_VER_YEAR), 16)
  NETSWIFT_VERSION="16.04"
else ifeq ($(CONFIG_RTE_VER_YEAR), 17)
  ifeq ($(CONFIG_RTE_VER_MONTH), 11)
    NETSWIFT_VERSION="17.11"
  else
    NETSWIFT_VERSION="16.04"
  endif
else ifeq ($(CONFIG_RTE_VER_YEAR), 18)
  ifeq ($(CONFIG_RTE_VER_MONTH), 11)
    NETSWIFT_VERSION="18.11"
  else
    NETSWIFT_VERSION="17.11"
  endif
else ifeq ($(CONFIG_RTE_VER_YEAR), 19)
  ifeq ($(CONFIG_RTE_VER_MONTH), 11)
    NETSWIFT_VERSION="19.11"
  else
    NETSWIFT_VERSION="18.11"
  endif
else
  NETSWIFT_VERSION=$(shell printf "%02d.%02d" $(CONFIG_RTE_VER_YEAR) $(CONFIG_RTE_VER_MONTH))
endif

DIRS-y += $(NETSWIFT_VERSION)

include $(RTE_SDK)/mk/rte.subdir.mk
