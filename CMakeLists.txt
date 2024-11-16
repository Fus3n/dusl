cmake_minimum_required(VERSION 3.26)
project(dusl-exec)

set(CMAKE_CXX_STANDARD 17)

# Set install prefix early
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/bin" CACHE PATH "Installation directory" FORCE)

# Add dusllib first
add_subdirectory(dusllib)

# Main executable
add_executable(dusl-exec main.cpp)

# Link libraries
target_link_libraries(dusl-exec PUBLIC dusl fmt)

# Enable IPO if supported
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# Installation rules
install(TARGETS dusl-exec
        RUNTIME DESTINATION .
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
