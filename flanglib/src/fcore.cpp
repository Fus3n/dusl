#include "flang/utils/fcore.h"
#include <fmt/core.h>
#include <fstream>

void flang::createFunction(flang::Interpreter &visitor, std::string name, flang::BuiltinFunctionObject::FunctionPointer func) {
    flang::Token tok;
    tok.value = std::move(name);
    visitor.ctx.currenSymbol().setValue(
            tok.value, std::make_shared<flang::BuiltinFunctionObject>(tok, func)
    );
}

std::optional<std::string> flang::verifyArgsCount(int current_count, int expected_count, const flang::Token& tok, bool fixed_args) {
    if (fixed_args) {
        if (current_count != expected_count)
            return fmt::format("{} expects exactly {} arguments but received {}", tok.value, expected_count, current_count);
        return {};
    }
    if (current_count < expected_count)
        return fmt::format("{} expects at least {} arguments but received {}", tok.value, expected_count, current_count);
    return {};
}

bool flang::runSingleFile(const std::string& file_name, flang::Interpreter &visitor) {
    flang::Lexer lexer;
    flang::Parser parser;

    // read the file
    std::string code;
    std::ifstream file(file_name);
    if (!file.is_open()) {
        fmt::print("Failed to open file: {}\n", file_name);
        return false;
    }
    while (!file.eof()) {
        std::string line;
        std::getline(file, line);
        code += line + "\n";
    }
    file.close();


    auto tokens = lexer.tokenize(code, file_name);
    auto ast = parser.parse(code, file_name, tokens);
    //    for (auto& statement : ast->statements) {
//        fmt::println("{}", statement->ToString());
//    }
//    return true;


    std::shared_ptr<flang::Object> return_val;
    for (auto& statement : ast->statements) {
        return_val = visitor.visit(statement);
    }

    bool success;
    if (return_val) {
        fmt::println("{}", return_val->ToString());
        success = true;
    } else {
        fmt::println("Return value is null");
        success = false;
    }


    return success;
}