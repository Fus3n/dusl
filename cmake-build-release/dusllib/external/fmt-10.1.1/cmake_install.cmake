# Install script for directory: F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "F:/RandomScript/CPPStuff/dusl/bin")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "D:/Clion/CLion 2023.2.2/bin/mingw/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/RandomScript/CPPStuff/dusl/cmake-build-release/dusllib/external/fmt-10.1.1/libfmt.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/fmt" TYPE FILE FILES
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/args.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/chrono.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/color.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/compile.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/core.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/format.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/format-inl.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/os.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/ostream.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/printf.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/ranges.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/std.h"
    "F:/RandomScript/CPPStuff/dusl/dusllib/external/fmt-10.1.1/include/fmt/xchar.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES
    "F:/RandomScript/CPPStuff/dusl/cmake-build-release/dusllib/external/fmt-10.1.1/fmt-config.cmake"
    "F:/RandomScript/CPPStuff/dusl/cmake-build-release/dusllib/external/fmt-10.1.1/fmt-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake"
         "F:/RandomScript/CPPStuff/dusl/cmake-build-release/dusllib/external/fmt-10.1.1/CMakeFiles/Export/b834597d9b1628ff12ae4314c3a2e4b8/fmt-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "F:/RandomScript/CPPStuff/dusl/cmake-build-release/dusllib/external/fmt-10.1.1/CMakeFiles/Export/b834597d9b1628ff12ae4314c3a2e4b8/fmt-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "F:/RandomScript/CPPStuff/dusl/cmake-build-release/dusllib/external/fmt-10.1.1/CMakeFiles/Export/b834597d9b1628ff12ae4314c3a2e4b8/fmt-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "F:/RandomScript/CPPStuff/dusl/cmake-build-release/dusllib/external/fmt-10.1.1/fmt.pc")
endif()

