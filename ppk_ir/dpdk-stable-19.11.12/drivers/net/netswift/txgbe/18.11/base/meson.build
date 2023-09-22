# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017 Intel Corporation

sources = [
	'txgbe_hw.c',
	'txgbe_api.c',
	'txgbe_hw.c',
	'txgbe_dcb_hw.c',
	'txgbe_dcb.c',
	'txgbe_hv_vf.c',
	'txgbe_mbx.c',
	'txgbe_phy.c',
	'txgbe_vf.c',
]

error_cflags = ['-Wno-unused-value',
		'-Wno-unused-but-set-variable']
c_args = cflags
if allow_experimental_apis
	c_args += '-DALLOW_EXPERIMENTAL_API'
endif
foreach flag: error_cflags
	if cc.has_argument(flag)
		c_args += flag
	endif
endforeach

base_lib = static_library('txgbe_base', sources,
	dependencies: static_rte_eal,
	c_args: c_args)
base_objs = base_lib.extract_all_objects()
