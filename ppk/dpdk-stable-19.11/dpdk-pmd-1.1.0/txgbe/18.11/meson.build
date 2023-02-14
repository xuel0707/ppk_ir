# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017 Intel Corporation

version = 2

cflags += ['-DRTE_LIBRTE_TXGBE_BYPASS']

allow_experimental_apis = true

subdir('base')
objs = [base_objs]

sources = files(
	'txgbe_ethdev.c',
	'txgbe_fdir.c',
	'txgbe_flow.c',
	'txgbe_ipsec.c',
	'txgbe_pf.c',
	'txgbe_rxtx.c',
	'txgbe_tm.c',
	'txgbe_vf_representor.c',
	'rte_pmd_txgbe.c'
)

deps += ['hash', 'security']

if arch_subdir == 'x86'
	dpdk_conf.set('RTE_TXGBE_INC_VECTOR', 1)
	sources += files('txgbe_rxtx_vec_sse.c')
elif arch_subdir == 'arm'
	dpdk_conf.set('RTE_TXGBE_INC_VECTOR', 1)
	sources += files('txgbe_rxtx_vec_neon.c')
endif

includes += include_directories('base')

install_headers('rte_pmd_txgbe.h')
