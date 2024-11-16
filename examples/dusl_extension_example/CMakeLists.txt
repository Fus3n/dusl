cmake_minimum_required(VERSION 3.26)
project(dusl_example_extension)

set(CMAKE_CXX_STANDARD 17)
#add_subdirectory(dusllib)

add_subdirectory(${CMAKE_SOURCE_DIR}/../../dusllib dusl)

add_library(dusl_example_extension SHARED
    example_extension.cpp
)

target_include_directories(dusl_example_extension PRIVATE
    ${CMAKE_SOURCE_DIR}/dusllib/include
)

target_link_libraries(dusl_example_extension PRIVATE
        dusl
)

set_target_properties(dusl_example_extension PROPERTIES
    PREFIX ""
    SUFFIX ".dusll"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/extensions"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/extensions"
)

message(STATUS "Library will be built in: ${CMAKE_BINARY_DIR}/extensions")