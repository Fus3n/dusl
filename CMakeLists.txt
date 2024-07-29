cmake_minimum_required(VERSION 3.26)
project(dusl)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(dusllib)

add_executable(dusl main.cpp)

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()


target_link_libraries(dusl PUBLIC dusllib)
target_link_libraries(dusl PUBLIC fmt)
