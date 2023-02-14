# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2015-2020

subdir('base')
objs = [base_objs]

sources = files(
	'txgbe_ethdev.c',
	'txgbe_ethdev_vf.c',
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

install_headers('rte_pmd_txgbe.h')
install_headers('rte_pmd_txgbe_compat.h')
