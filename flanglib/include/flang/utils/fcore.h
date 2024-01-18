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
    [[maybe_unused]] std::optional<std::string> verifyArgsCount(size_t current_count, size_t expected_count, const flang::Token& tok, bool fixed_args= true);
    [[maybe_unused]] bool runSingleFile(const std::string& file_name, flang::Interpreter &visitor);
    [[maybe_unused]] void addGlobalVariable(flang::Interpreter &visitor, const std::string& name, flang::IntObject* value);
    [[maybe_unused]] void addGlobalVariable(flang::Interpreter &visitor, const std::string& name, flang::StringObject* value);
    [[maybe_unused]] void addGlobalVariable(flang::Interpreter &visitor, const std::string& name, flang::BooleanObject* value);
    [[maybe_unused]] IntObject createInt(int64_t value);
    [[maybe_unused]] StringObject createString(std::string value);
    [[maybe_unused]] BooleanObject createBool(bool value);

}


