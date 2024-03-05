cmake_minimum_required(VERSION 3.26)
project(flang)

set(CMAKE_CXX_STANDARD 17)

#set(iS_DEBUG, TRUE)
#
#if(iS_DEBUG)
#    set(CMAKE_CXX_FLAGS_RELEASE "-O0")
#    set(CMAKE_C_FLAGS_RELEASE "-O0")
#    set(CMAKE_C_FLAGS_DEBUG "-O0")
#    set(CMAKE_CXX_FLAGS_DEBUG "-O0")
#else()
#    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#    set(CMAKE_C_FLAGS_RELEASE "-O3")
#    set(CMAKE_C_FLAGS_DEBUG "-O3")
#    set(CMAKE_CXX_FLAGS_DEBUG "-O3")
#endif()

add_subdirectory(flanglib)

add_executable(flang main.cpp)

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_link_libraries(flang PUBLIC flanglib)
target_link_libraries(flang PUBLIC fmt)
