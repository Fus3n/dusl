#include "flang/utils/fcore.h"
#include "flang/utils/common.h"
#include <fmt/core.h>
#include <fstream>
#include <memory>
#include <utility>
#include <filesystem>


void flang::createFunction(flang::Interpreter &visitor, std::string name, flang::BuiltinFunctionObject::FunctionPointer func) {
    flang::Token tok;
    tok.value = std::move(name);
    visitor.ctx.currentSymbol().setValue(
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
    std::string code;

    auto path = std::filesystem::path(file_name);

    // read the file
    code = flang::read_file(file_name);

    // trim code
    flang::str_trim(code);

    if (code.empty()) {
        return false;
    }

    auto tokens = lexer.tokenize(code, file_name);
    auto ast = parser.parse(code, file_name, tokens);

    const auto jsn = ast.toJson().dump(2);
//    fmt::println("{}", jsn);
    flang::write_file("ast.json", jsn);
//    return 0;

    auto file_context_name = path.stem().string();
    visitor.ctx.setBaseCtxName(file_context_name);
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
    visitor.ctx.currentSymbol().setValue(name, std::make_shared<flang::IntObject>(*value));
}

void flang::addGlobalVariable(flang::Interpreter &visitor, const std::string &name, flang::StringObject *value) {
    visitor.ctx.currentSymbol().setValue(name, std::make_shared<flang::StringObject>(*value));
}
void flang::addGlobalVariable(flang::Interpreter &visitor, const std::string &name, flang::BooleanObject *value) {
    visitor.ctx.currentSymbol().setValue(name, std::make_shared<flang::BooleanObject>(*value));
}

[[maybe_unused]] flang::IntObject flang::createInt(int64_t value) {
    return flang::IntObject(value, flang::Token());
}
flang::IntObject flang::createInt(int64_t value, flang::Token tok) {
    return flang::IntObject(value, std::move(tok));
}

flang::StringObject flang::createString(std::string value) {
    return flang::StringObject(std::move(value), flang::Token());
}
flang::StringObject flang::createString(std::string value, flang::Token tok) {
    return flang::StringObject(std::move(value), std::move(tok));
}

flang::BooleanObject flang::createBool(bool value) {
    return {value, flang::Token()};
}

flang::BooleanObject flang::createBool(bool value, flang::Token tok) {
    return {value, std::move(tok)};
}

[[noreturn]] void flang::runRepl(flang::Interpreter &visitor) {
    fmt::println(
            "Flang 0.0.1 ({}, {}) on {}",
            __DATE__, __TIME__,
            flang::getOsName()
    );

    flang::Lexer lexer;
    flang::Parser parser;
    std::string code;

    // TODO: temporary, will be changed later
    while (true) {
        std::string line;
        fmt::print(">>> ");
        std::getline(std::cin, line);
        code += line + "\n";
        if (line.empty()) {
            continue;
        }

        // RUN
        const std::string file_name = "<REPL>";
        auto tokens = lexer.tokenize(code, file_name);
        auto ast = parser.parse(code, file_name, tokens);

        try {
            auto return_val = visitor.visit(ast);

            if (return_val.isError()) {
                fmt::println("{}", return_val.err->toString());
                continue;
            }

            if (return_val.result) {
                if (return_val.result->getTypeString() == "none" || return_val.result->isBreak()) {
                    continue;
                }
                fmt::println("{}", return_val.result->toString());
            }
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

    // pause console

}





