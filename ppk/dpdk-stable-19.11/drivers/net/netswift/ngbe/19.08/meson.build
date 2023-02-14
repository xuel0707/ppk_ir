# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017 Intel Corporation

version = 2

cflags += ['-DRTE_LIBRTE_NGBE_BYPASS']

allow_experimental_apis = true

subdir('base')
objs = [base_objs]

sources = files(
	'ngbe_ethdev.c',
	'ngbe_fdir.c',
	'ngbe_flow.c',
	'ngbe_ipsec.c',
	'ngbe_pf.c',
	'ngbe_rxtx.c',
	'ngbe_tm.c',
	'ngbe_vf_representor.c',
	'rte_pmd_ngbe.c'
)

deps += ['hash', 'security']

if arch_subdir == 'x86'
	dpdk_conf.set('RTE_NGBE_INC_VECTOR', 1)
	sources += files('ngbe_rxtx_vec_sse.c')
endif

includes += include_directories('base')

install_headers('rte_pmd_ngbe.h')
