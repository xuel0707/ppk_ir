# Install script for directory: /home/aa/ppk_sw/ppk/p4c

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/p4c" TYPE DIRECTORY FILES "/home/aa/ppk_sw/ppk/p4c/p4include" FILES_MATCHING REGEX "/[^/]*\\.p4$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/aa/ppk_sw/ppk/p4c/build/tools/driver/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/lib/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/tools/ir-generator/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/ir/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/backends/dpdk/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/frontends/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/midend/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/control-plane/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/backends/bmv2/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/backends/ebpf/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/backends/ubpf/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/backends/p4test/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/backends/graphs/cmake_install.cmake")
  include("/home/aa/ppk_sw/ppk/p4c/build/test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/aa/ppk_sw/ppk/p4c/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
