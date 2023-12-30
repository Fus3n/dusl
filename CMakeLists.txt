cmake_minimum_required(VERSION 3.26)
project(flang)

set(CMAKE_CXX_STANDARD 17)
# Enable optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
#add_subdirectory("../fmt-10.1.1" build)
add_subdirectory(flanglib)

add_executable(flang main.cpp)

target_link_libraries(flang PUBLIC flanglib)
target_link_libraries(flang PUBLIC fmt)
