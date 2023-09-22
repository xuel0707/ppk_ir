# This does not build a driver, but instead holds common files for
# the crypto and compression drivers.
build = true
reason = '' # sentinal value to suppress printout
rsp_includes += include_directories('.',"../../bus/pci/")
rsp_deps = ['bus_pci']
rsp_sources = files('rsp_common.c',
		'rsp_qp.c',
		'rsp_device.c',
		'rsp_logs.c')
rsp_includes = [include_directories('.')]
rsp_ext_deps = []
rsp_cflags = []
