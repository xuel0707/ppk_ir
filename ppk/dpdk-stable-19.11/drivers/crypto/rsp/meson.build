# this does not build the RSP driver, instead that is done in the compression
# driver which comes later. Here we just add our sources files to the list
build = false
reason = '' # sentinal value to suppress printout
dep = dependency('libcrypto', required: false)
#rsp_deps = ['bus_pci']
rsp_includes += include_directories('.',"../../bus/pci/")
rsp_deps += ['cryptodev' 'bus_pci']
if dep.found()
	# Add our sources files to the list
	rsp_sources += files('rsp_sym_pmd.c',
			     'rsp_sym.c',
			     'rsp_sym_session.c',
			     'rsp_asym_pmd.c',
			     'rsp_asym.c')
	rsp_ext_deps += dep
	rsp_cflags += '-DBUILD_RSP_SYM'
	rsp_cflags += '-DBUILD_RSP_ASYM'
endif
