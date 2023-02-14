# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017 Intel Corporation

sources = [
	'ngbe_hw.c',
	'ngbe_api.c',
	'ngbe_hw.c',
	'ngbe_hv_vf.c',
	'ngbe_mbx.c',
	'ngbe_phy.c',
	'ngbe_vf.c',
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

base_lib = static_library('ngbe_base', sources,
	dependencies: static_rte_eal,
	c_args: c_args)
base_objs = base_lib.extract_all_objects()
