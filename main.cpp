#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <dusl/utils/dusl_core.hpp>
#include <dusl/Context.hpp>
#include <dusl/SymbolTable.hpp>
#include <dusl/utils/dusl_standard.hpp>

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    // TODO: LATEST: fix json output not having expected ret
    // TODO: fix "not" operator not workig wiht memberacess

    if (argc < 1) {
        std::cerr << "Usage: dusl [file]" << std::endl;
        return 1;
    }

    std::string file_name = argv[1];
    dusl::Interpreter visitor;

    dusl::Context global_context;
    global_context.enterScope();
    visitor.setContext(global_context);
    dusl::loadSTL(visitor);

    bool success = dusl::runSingleFile(file_name, visitor);
//    dusl::runRepl(visitor);

    global_context.exitScope();

    return success;
}
