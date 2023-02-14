# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2015-2020

sources = [
	'ngbe_eeprom.c',
	'ngbe_hw.c',
	'ngbe_hw_em.c',
	'ngbe_hw_sp.c',
	'ngbe_hv_vf.c',
	'ngbe_mbx.c',
	'ngbe_mng.c',
	'ngbe_phy.c',
	'ngbe_phy_rtl.c',
	'ngbe_phy_mvl.c',
	'ngbe_phy_yt.c',
	'ngbe_phy_zte.c',
	'ngbe_ptypes.c',
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
	dependencies: [static_rte_eal, static_rte_ethdev, static_rte_bus_pci],
	c_args: c_args)
base_objs = base_lib.extract_all_objects()
