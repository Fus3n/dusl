cmake_minimum_required(VERSION 3.26)
project(dusllib)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("../fmt-10.1.1" build)

include_directories(include/dusl)
file(GLOB SOURCES src/*.cpp)
file(GLOB INCLUDES include/*.h)

add_library(dusllib STATIC
        ${SOURCES}
        ${INCLUDES}
)
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_include_directories(dusllib PUBLIC include)
target_link_libraries(dusllib PUBLIC fmt)
