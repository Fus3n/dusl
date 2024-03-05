#include <dusl/utils/dusl_core.hpp>
#include <dusl/utils/common.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fstream>
#include <memory>
#include <utility>
#include <filesystem>


void dusl::createFunction(dusl::Interpreter &visitor, std::string name, dusl::BuiltinFunctionObject::FunctionPointer func) {
    dusl::Token tok;
    tok.value = std::move(name);
    visitor.ctx.currentSymbol().setValue(
            tok.value, std::make_shared<dusl::BuiltinFunctionObject>(tok, func)
    );
}

std::optional<std::string> dusl::verifyArgsCount(size_t current_count, size_t expected_count, const dusl::Token& tok, bool fixed_args) {
    if (fixed_args) {
        if (current_count != expected_count)
            return fmt::format("{} expects exactly {} arguments but received {}", tok.value, expected_count, current_count);
        return {};
    }
    if (current_count < expected_count)
        return fmt::format("{} expects at least {} arguments but received {}", tok.value, expected_count, current_count);
    return {};
}

bool dusl::runSingleFile(const std::string& file_name, dusl::Interpreter &visitor) {
    dusl::Lexer lexer;
    dusl::Parser parser;
    std::string code;

    auto path = std::filesystem::path(file_name);

    if (!exists(path)) {
        fmt::print(fmt::fg(fmt::color::red), "DUSL Error: {} Doesn't exist\n", path.string());
        return false;
    }
    // read the file
    code = dusl::read_file(file_name);

    // trim code
    dusl::str_trim(code);

    if (code.empty()) {
        return false;
    }

    auto tokens = lexer.tokenize(code, file_name);
    auto ast = parser.parse(code, file_name, tokens);

    const auto jsn = ast.toJson().dump(2);
//    fmt::println("{}", jsn);
    dusl::write_file("ast.json", jsn);
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

void dusl::addGlobalVariable(dusl::Interpreter &visitor, const std::string& name, dusl::IntObject *value) {
    visitor.ctx.currentSymbol().setValue(name, std::make_shared<dusl::IntObject>(*value));
}

void dusl::addGlobalVariable(dusl::Interpreter &visitor, const std::string &name, dusl::StringObject *value) {
    visitor.ctx.currentSymbol().setValue(name, std::make_shared<dusl::StringObject>(*value));
}
void dusl::addGlobalVariable(dusl::Interpreter &visitor, const std::string &name, dusl::BooleanObject *value) {
    visitor.ctx.currentSymbol().setValue(name, std::make_shared<dusl::BooleanObject>(*value));
}

[[maybe_unused]] dusl::IntObject dusl::createInt(int64_t value) {
    return dusl::IntObject(value, dusl::Token());
}
dusl::IntObject dusl::createInt(int64_t value, dusl::Token tok) {
    return dusl::IntObject(value, std::move(tok));
}

dusl::StringObject dusl::createString(std::string value) {
    return dusl::StringObject(std::move(value), dusl::Token());
}
dusl::StringObject dusl::createString(std::string value, dusl::Token tok) {
    return dusl::StringObject(std::move(value), std::move(tok));
}

dusl::BooleanObject dusl::createBool(bool value) {
    return {value, dusl::Token()};
}

dusl::BooleanObject dusl::createBool(bool value, dusl::Token tok) {
    return {value, std::move(tok)};
}

[[noreturn]] void dusl::runRepl(dusl::Interpreter &visitor) {
    fmt::println(
            "dusl 0.0.1 ({}, {}) on {}",
            __DATE__, __TIME__,
            dusl::getOsName()
    );

    dusl::Lexer lexer;
    dusl::Parser parser;
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





