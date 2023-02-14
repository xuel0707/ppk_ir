# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2010-2015 Intel Corporation

include $(RTE_SDK)/mk/rte.vars.mk

DIRS-$(CONFIG_RTE_LIBRTE_TXGBE_PMD) += txgbe
DIRS-$(CONFIG_RTE_LIBRTE_NGBE_PMD) += ngbe

ifeq ($(CONFIG_RTE_VER_YEAR),)
RTE_VERSION_H=$(RTE_SRCDIR)/lib/librte_eal/common/include/rte_version.h
export CONFIG_RTE_VER_YEAR=$(shell sed -n '/\#define.*VER_YEAR/s;.*\s\([0-9]\+\).*;\1;p' $(RTE_VERSION_H))
export CONFIG_RTE_VER_MONTH=$(shell sed -n '/\#define.*VER_MONTH/s;.*\s\([0-9]\+\).*;\1;p' $(RTE_VERSION_H))
endif

include $(RTE_SDK)/mk/rte.subdir.mk
