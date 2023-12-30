cmake_minimum_required(VERSION 3.26)
project(flanglib)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("../fmt-10.1.1" build)

set(iS_DEBUG, FALSE)

if(TRUE)
    set(CMAKE_CXX_FLAGS_RELEASE "-O0")
    set(CMAKE_C_FLAGS_RELEASE "-O0")
    set(CMAKE_C_FLAGS_DEBUG "-O0")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
    set(CMAKE_C_FLAGS_DEBUG "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-O3")
endif()


include_directories(include/flang)
file(GLOB SOURCES src/*.cpp)
file(GLOB INCLUDES include/*.h)

add_library(flanglib STATIC
        ${SOURCES}
        ${INCLUDES}
)
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_include_directories(flanglib PUBLIC include)
target_link_libraries(flanglib PUBLIC fmt)
