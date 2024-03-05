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
    // TODO: LATEST: fix json output not having expected ret
    // TODO: fix "not" operator not workig wiht memberacess

    if (argc < 1) {
        std::cerr << "Usage: flang [file]" << std::endl;
        return 1;
    }

    std::string file_name = argv[1];
    flang::Interpreter visitor;

    flang::Context global_context;
    global_context.enterScope();
    visitor.setContext(global_context);
    flang::loadSTL(visitor);

    bool success = flang::runSingleFile(file_name, visitor);
//    flang::runRepl(visitor);

    global_context.exitScope();

    return success;
}
