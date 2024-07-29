#pragma once
#include <dusl/Lexer.hpp>
#include <dusl/Parser.hpp>
#include <dusl/LanguageManager.hpp>
#include <dusl/Interpreter.hpp>
#include <dusl/DObject.hpp>
#include <optional>

/// Add a function to the interpreter
namespace dusl {

    void createFunction(dusl::Interpreter &visitor, std::string name, dusl::BuiltinFunctionObject::FunctionPointer func, const std::string& doc_str="");

    //template <typename T>
    std::shared_ptr<StructProxyObject> createStruct(
        dusl::Interpreter &visitor, 
        const std::string& cls_name,
        dusl::CreatorFunction& func,
        const std::string& doc_str=""
    );

    [[maybe_unused]] std::optional<std::string> verifyArgsCount(size_t current_count, size_t expected_count, const dusl::Token& tok, bool fixed_args= true);
    [[maybe_unused]] bool runSingleFile(const std::string& file_name, dusl::Interpreter &visitor);

    [[noreturn]] [[maybe_unused]] void runRepl(dusl::Interpreter &visitor);
    [[maybe_unused]] void addGlobalVariable(dusl::Interpreter &visitor, const std::string& name, dusl::IntObject* value);
    [[maybe_unused]] void addGlobalVariable(dusl::Interpreter &visitor, const std::string& name, dusl::StringObject* value);
    [[maybe_unused]] void addGlobalVariable(dusl::Interpreter &visitor, const std::string& name, dusl::BooleanObject* value);
    [[maybe_unused]] IntObject createInt(int64_t value);
    [[maybe_unused]] IntObject createInt(int64_t value, Token tok);
    [[maybe_unused]] StringObject createString(std::string value);
    [[maybe_unused]] StringObject createString(std::string value, Token tok);
    [[maybe_unused]] BooleanObject createBool(bool value);
    [[maybe_unused]] BooleanObject createBool(bool value, Token tok);

}


