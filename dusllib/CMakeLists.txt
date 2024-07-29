cmake_minimum_required(VERSION 3.26)
project(dusllib)

set(CMAKE_CXX_STANDARD 17)

include_directories(include/dusl)

add_library(dusllib STATIC
        src/BaseObject.cpp
        src/common.cpp
        src/Context.cpp
        src/DataNodes.cpp
        src/DictionaryObject.cpp
        src/DObject.cpp
        src/dusl_core.cpp
        src/dusl_standard.cpp
        src/ErrorObject.cpp
        src/ErrorType.cpp
        src/FloatObject.cpp
        src/Interpreter.cpp
        src/IntObject.cpp
        src/LanguageManager.cpp
        src/Lexer.cpp
        src/ListObject.cpp
        src/Parser.cpp
        src/Position.cpp
        src/StringObject.cpp
        src/StructObject.cpp
        src/StructProxyObject.cpp
        src/SymbolTable.cpp

        include/dusl/utils/common.h
        include/dusl/utils/dusl_core.hpp
        include/dusl/utils/dusl_standard.hpp

        include/dusl/Context.hpp
        include/dusl/DataNodes.hpp
        include/dusl/DObject.hpp
        include/dusl/ErrorType.hpp
        include/dusl/Interpreter.hpp
        include/dusl/LanguageManager.hpp
        include/dusl/Lexer.hpp
        include/dusl/Parser.hpp
        include/dusl/Position.hpp
        include/dusl/Serializable.hpp
        include/dusl/SymbolTable.hpp
)

# might need to add nlohmen json include files

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

add_subdirectory("../external/fmt-10.1.1" build)

target_include_directories(dusllib PUBLIC include)
target_link_libraries(dusllib PUBLIC fmt)
