file(REMOVE_RECURSE
  "CMakeFiles/p4c_driver"
  "../../p4c_src/main.py"
  "../../p4c_src/driver.py"
  "../../p4c_src/util.py"
  "../../p4c_src/config.py"
  "../../p4c_src/__init__.py"
  "../../p4c_src/p4c.bmv2.cfg"
  "../../p4c_src/p4c.ebpf.cfg"
  "../../p4c_src/p4c.dpdk.cfg"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/p4c_driver.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
