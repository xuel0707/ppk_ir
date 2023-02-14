# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2015-2020

dpdk_version = meson.project_version().split('-')[0].split('.')
dpdk_year = dpdk_version[0]
dpdk_month = dpdk_version[1]
version = dpdk_year + '.' + dpdk_month

drivers = [
	'txgbe',
	'ngbe',
]

txgbe_version_map = {
	'16.04' : '16.04',
	'16.07' : '16.04',
	'16.11' : '16.04',
	'17.02' : '16.04',
	'17.05' : '16.04',
	'17.08' : '16.04',
	'17.11' : '17.11',
	'18.02' : '17.11',
	'18.05' : '17.11',
	'18.08' : '17.11',
	'18.11' : '18.11',
	'19.02' : '18.11',
	'19.05' : '18.11',
	'19.08' : '18.11',
	'19.11' : '19.11',
	'20.02' : '20.02',
	'20.05' : '20.05',
	'20.08' : '20.08',
	'20.11' : '20.11',
}

ngbe_version_map = {
	'16.04' : '16.04',
	'16.07' : '16.07',
	'16.11' : '16.11',
	'17.02' : '17.02',
	'17.05' : '17.05',
	'17.08' : '17.08',
	'17.11' : '17.11',
	'18.02' : '18.02',
	'18.05' : '18.05',
	'18.08' : '18.08',
	'18.11' : '18.11',
	'19.02' : '19.02',
	'19.05' : '19.05',
	'19.08' : '19.08',
	'19.11' : '19.11',
	'20.02' : '20.02',
	'20.05' : '20.05',
	'20.08' : '20.08',
	'20.11' : '20.11',
}

versions = {
	'txgbe' : txgbe_version_map[version],
	'ngbe' : ngbe_version_map[version],
}

std_deps = ['ethdev', 'kvargs'] # 'ethdev' also pulls in mbuf, net, eal etc
std_deps += ['bus_pci']         # very many PMDs depend on PCI, so make std
std_deps += ['bus_vdev']        # same with vdev bus
config_flag_fmt = 'RTE_LIBRTE_@0@_PMD'
driver_name_fmt = 'rte_pmd_@0@'