cmake_minimum_required(VERSION 3.26)
project(flanglib)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("../fmt-10.1.1" build)
# Enable optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")

include_directories(include/flang)
file(GLOB SOURCES src/*.cpp)
file(GLOB INCLUDES include/*.h)

add_library(flanglib STATIC
        ${SOURCES}
        ${INCLUDES}
)

target_include_directories(flanglib PUBLIC include)
target_link_libraries(flanglib PUBLIC fmt)
