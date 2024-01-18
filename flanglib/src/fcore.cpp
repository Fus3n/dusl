#include "flang/utils/fcore.h"
#include "flang/utils/common.h"
#include <fmt/core.h>
#include <fstream>
#include <memory>
#include <utility>


void flang::createFunction(flang::Interpreter &visitor, std::string name, flang::BuiltinFunctionObject::FunctionPointer func) {
    flang::Token tok;
    tok.value = std::move(name);
    visitor.ctx.currenSymbol().setValue(
            tok.value, std::make_shared<flang::BuiltinFunctionObject>(tok, func)
    );
}

std::optional<std::string> flang::verifyArgsCount(size_t current_count, size_t expected_count, const flang::Token& tok, bool fixed_args) {
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

    // trim code
    flang::str_trim(code);

    if (code.empty()) {
        return false;
    }

    auto tokens = lexer.tokenize(code, file_name);
    auto ast = parser.parse(code, file_name, tokens);

//    for (auto& stmt : ast.statements) {
//        // test
//        fmt::println("{}", stmt->toString());
//    }
//    return 1;
    auto return_val = visitor.visit(ast);

    if (return_val.isError()){
        fmt::println("{}", return_val.err->toString());
        return false;
    }

    if (return_val.result) {
        if (return_val.result->getTypeString() == "none" || return_val.result->isBreak()) {
            return false; // don't print anything if its none
        }
        fmt::println("{}", return_val.result->toString());
        return false;
    } else {
        fmt::println("Return value is null");
        return true;
    }

}

void flang::addGlobalVariable(flang::Interpreter &visitor, const std::string& name, flang::IntObject *value) {
    visitor.ctx.currenSymbol().setValue(name, std::make_shared<flang::IntObject>(*value));
}

void flang::addGlobalVariable(flang::Interpreter &visitor, const std::string &name, flang::StringObject *value) {
    visitor.ctx.currenSymbol().setValue(name, std::make_shared<flang::StringObject>(*value));
}
void flang::addGlobalVariable(flang::Interpreter &visitor, const std::string &name, flang::BooleanObject *value) {
    visitor.ctx.currenSymbol().setValue(name, std::make_shared<flang::BooleanObject>(*value));
}

[[maybe_unused]] flang::IntObject flang::createInt(int64_t value) {
    return flang::IntObject(value, flang::Token());
}

flang::StringObject flang::createString(std::string value) {
    return flang::StringObject(std::move(value), flang::Token());
}

flang::BooleanObject flang::createBool(bool value) {
    return {value, flang::Token()};
}


