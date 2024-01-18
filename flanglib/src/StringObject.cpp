#include "flang/FObject.hpp"
#include "flang/ErrorType.hpp"
#include <fmt/core.h>
#include "flang/utils/fcore.h"
#include <cctype>

#include <functional>

std::string flang::StringObject::toString() const {
    return value;
}

bool flang::StringObject::isTrue() const {
    return !value.empty();
}

std::string flang::StringObject::getTypeString() const {
    return "string";
}

flang::FResult flang::StringObject::getProperty(const std::string& name, const Token& token) {
    if (name == "size") {
        return FResult::createResult(std::make_shared<IntObject>(value.length(), tok), tok);
    }
    return Object::getProperty(name, tok);
}

flang::FResult
flang::StringObject::add_to(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto stringObject = std::dynamic_pointer_cast<StringObject>(other)) {
        auto result = value + stringObject->value;
        return FResult::createResult(std::make_shared<StringObject>(result, tok), tok);
    }
    return Object::add_to(other, token);
}

flang::FResult
flang::StringObject::equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto strObj = std::dynamic_pointer_cast<StringObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == strObj->value, tok), tok);
    }
    return FResult::createResult(std::make_shared<BooleanObject>(false, tok), tok);
}

flang::FResult
flang::StringObject::not_equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto strObj = std::dynamic_pointer_cast<StringObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != strObj->value, tok), tok);
    }
    return FResult::createResult(std::make_shared<BooleanObject>(true, tok), tok);
}

flang::FResult flang::StringObject::hash(const flang::Token &token) const {
    return FResult::createResult(std::make_shared<IntObject>(std::hash<std::string>{}(value), token), tok);
}

flang::FResult flang::StringObject::index(std::shared_ptr<ListObject> idx_args) {
    if (idx_args->items.size() != 1) {
        return FResult::createError(
            IndexError,
            fmt::format("list index takes 1 argument but {} were given", idx_args->items.size()),
            tok
        );
    }

    auto first_arg = idx_args->items[0];
    if (auto intObj = dynamic_cast<IntObject*>(first_arg.get())) {
        if (intObj->value > value.size() - 1) {
            return FResult::createError(
                    IndexError,
                    fmt::format("Index out of range {}", intObj->value),
                    tok
            );
        }
        return FResult::createResult(std::make_shared<StringObject>(value[intObj->value], tok), tok);
    }

    return Object::index(idx_args);
}

flang::FResult flang::StringObject::get(StringObject &str, flang::Interpreter &visitor,
                                        const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args.size(), 1, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }

    auto arg = fn_node->args[0]->accept(visitor);
    if (arg.isError()) return arg;

    if (auto intObj = std::dynamic_pointer_cast<IntObject>(arg.result)) {
        if (intObj->value > str.value.length()) {
            return FResult::createError(IndexError, fmt::format("{} index out of range {}", str.getTypeString(), intObj->value), fn_node->tok);
        }
        return FResult::createResult(std::make_shared<StringObject>(str.value[intObj->value], str.tok), fn_node->tok);
    }

    return FResult::createError(RunTimeError, "'get' expects an INT as an argument", fn_node->tok);
}

flang::FResult flang::StringObject::split(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args.size(), 1, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError,
                                    res.value(),
                                    fn_node->tok
        );
    }
    auto first_arg = fn_node->args[0]->accept(visitor);
    if (first_arg.isError()) return first_arg;

    if (auto strObj = dynamic_cast<StringObject*>(first_arg.result.get())) {
        return FResult::createResult(ListObject::splitAtDelimiter(str.value, strObj->value, str.tok), fn_node->tok);
    }
}

flang::FResult flang::StringObject::to_int(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }

    try {
        auto str_value = std::stoi(str.value);
        return FResult::createResult(std::make_shared<IntObject>(str_value, str.tok), fn_node->tok);
    } catch (std::exception &e) {
        return FResult::createError(RunTimeError, fmt::format("Invalid integer {}", str.value), fn_node->tok);
    }
}

flang::FResult flang::StringObject::to_float(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    try {
        auto str_value = std::stold(str.value);
        return FResult::createResult(std::make_shared<FloatObject>(str_value, str.tok), fn_node->tok);
    } catch (std::exception &e) {
        return FResult::createError(RunTimeError, fmt::format("Invalid Float {}", str.value), fn_node->tok);
    }
}

void flang::StringObject::init_functions() {
    functions["split"] = reinterpret_cast<PropertyFunction>(StringObject::split);
    functions["get"] = reinterpret_cast<PropertyFunction>(StringObject::get);
    functions["toInt"] = reinterpret_cast<PropertyFunction>(StringObject::to_int);
    functions["toFloat"] = reinterpret_cast<PropertyFunction>(StringObject::to_float);
    functions["toLower"] = reinterpret_cast<PropertyFunction>(StringObject::to_lower);
    functions["toUpper"] = reinterpret_cast<PropertyFunction>(StringObject::to_upper);
    functions["isDigit"] = reinterpret_cast<PropertyFunction>(StringObject::is_digit);
    functions["isAlpha"] = reinterpret_cast<PropertyFunction>(StringObject::is_alpha);
}
std::string toLowerStr(std::string& s) {
    for(char &c : s)
        c = std::tolower(c);
    return s;
}

std::string toUpperstr(std::string& s) {
    for(char &c : s)
        c = std::toupper(c);
    return s;
}
flang::FResult flang::StringObject::to_lower(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    return FResult::createResult(std::make_shared<StringObject>(toLowerStr(str.value), str.tok), fn_node->tok);
}

flang::FResult flang::StringObject::to_upper(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    return FResult::createResult(std::make_shared<StringObject>(toUpperstr(str.value), str.tok), fn_node->tok);
}

flang::FResult flang::StringObject::is_digit(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    for (auto c : str.value) {
        if (!std::isdigit(c)) {
            return FResult::createResult(std::make_shared<BooleanObject>(false, str.tok), fn_node->tok);
        }
    }
    return FResult::createResult(std::make_shared<BooleanObject>(true, str.tok), fn_node->tok);
}

flang::FResult flang::StringObject::is_alpha(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    for (auto c : str.value) {
        if (!std::isalpha(c)) {
            return FResult::createResult(std::make_shared<BooleanObject>(false, str.tok), fn_node->tok);
        }
    }
    return FResult::createResult(std::make_shared<BooleanObject>(true, str.tok), fn_node->tok);
}

