#include <iostream>
#include <string>
#include <fstream>
#include <dusl/utils/dusl_core.hpp>
#include <dusl/Context.hpp>
#include <dusl/utils/dusl_standard.hpp>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: dusl [file]" << std::endl;
        return 1;
    }

    std::string file_name = argv[1];
    dusl::Interpreter visitor;

    dusl::Context global_context;
    global_context.enterScope();
    visitor.setContext(global_context);
    dusl::loadSTL(visitor);
    auto items = dusl::loadObjects(visitor); // keeping list of object so they dont die and get deleted

    bool success = dusl::runSingleFile(file_name, visitor);
//    dusl::runRepl(visitor);

    global_context.exitScope();

    return success;
}
