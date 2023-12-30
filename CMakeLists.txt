cmake_minimum_required(VERSION 3.26)
project(flang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#add_subdirectory("../fmt-10.1.1" build)
add_subdirectory(flanglib)

add_executable(flang main.cpp)


include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()



target_link_libraries(flang PUBLIC flanglib)
target_link_libraries(flang PUBLIC fmt)
