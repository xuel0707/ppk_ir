# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2015-2020

allow_experimental_apis = true

subdir('base')
objs = [base_objs]

sources = files(
	'ngbe_ethdev.c',
	'ngbe_ethdev_vf.c',
	'ngbe_flow.c',
	'ngbe_ipsec.c',
	'ngbe_pf.c',
	'ngbe_rxtx.c',
	'ngbe_tm.c',
	'ngbe_vf_representor.c',
	'rte_pmd_ngbe.c'
)

deps += ['hash', 'security']

includes += include_directories('base')

install_headers('rte_pmd_ngbe.h')
install_headers('rte_pmd_compat.h')
