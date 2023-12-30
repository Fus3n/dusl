#pragma once
#include "flang/Lexer.hpp"
#include "flang/Parser.hpp"
#include "flang/LanguageManager.hpp"
#include "flang/Interpreter.hpp"
#include "flang/FObject.hpp"
#include <optional>

/// Add a function to the interpreter
namespace flang {
    void createFunction(flang::Interpreter &visitor, std::string name, flang::BuiltinFunctionObject::FunctionPointer func);
    [[maybe_unused]] std::optional<std::string> verifyArgsCount(int current_count, int expected_count, const flang::Token& tok, bool fixed_args=true);
    [[maybe_unused]] bool runSingleFile(const std::string& file_name, flang::Interpreter &visitor);
    [[maybe_unused]] bool loadSTD(flang::Interpreter &visitor);
}
