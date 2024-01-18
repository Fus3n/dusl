#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <flang/utils/fcore.h>
#include <flang/Context.hpp>
#include <flang/SymbolTable.hpp>
#include <flang/utils/fstandard.hpp>

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    // TODO: LATEST: fix some parser or syntax errors not printing
    if (argc < 1) {
        std::cerr << "Usage: flang [file]" << std::endl;
        return 1;
    }

    std::string file_name = argv[1];
    flang::Interpreter visitor;

    flang::Context global_context;
    global_context.enterScope();
    auto global_symbol = flang::SymbolTable();
    visitor.setContext(global_context);
    flang::loadSTL(visitor);

    bool success = runSingleFile(file_name, visitor);
    global_context.exitScope();

    return success;
}
