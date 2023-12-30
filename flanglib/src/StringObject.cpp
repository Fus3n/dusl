#include "flang/FObject.hpp"
#include "flang/ErrorType.hpp"
#include <fmt/core.h>
#include "flang/utils/fcore.h"

#include <functional>

std::string flang::StringObject::toString() const {
    return fmt::format("\"{}\"", value);
}

bool flang::StringObject::isTrue() const {
    return !value.empty();
}

std::string flang::StringObject::getTypeString() const {
    return "string";
}

std::shared_ptr<flang::Object> flang::StringObject::getProperty(const std::string &name) {
    if (name == "count") {
        return std::make_shared<IntObject>(value.length(), tok);
    }
    return Object::getProperty(name);
}


std::shared_ptr<flang::Object>
flang::StringObject::callProperty(flang::Interpreter& visitor, const std::shared_ptr<flang::FunctionCallNode> &fn_node) {
    if (fn_node->tok.value == "split") {
        auto res = verifyArgsCount(fn_node->args.size(), 1, tok);
        if (res.has_value()) {
            FError(RunTimeError,
                   res.value(),
                   fn_node->tok.pos
            ).Throw();
        }
        auto first_arg = fn_node->args[0]->accept(visitor);
        if (auto strObj = dynamic_cast<StringObject*>(first_arg.get())) {
            return ListObject::splitAtDelimiter(value, strObj->value, tok);
        }
    } else if (fn_node->tok.value == "get") {
        auto res = verifyArgsCount(fn_node->args.size(), 1, tok);
        if (res.has_value()) {
            FError(RunTimeError,
                   res.value(),
                   fn_node->tok.pos
            ).Throw();
        }

        auto arg = fn_node->args[0]->accept(visitor);
        if (auto intObj = std::dynamic_pointer_cast<IntObject>(arg)) {
            if (intObj->value > value.length()) {
                FError(IndexError,
                       fmt::format("{} index out of range {}", getTypeString(), intObj->value),
                       fn_node->tok.pos
                ).Throw();
            }
            return std::make_shared<StringObject>(value[intObj->value], tok);
        } else {
            FError(IndexError,
                   "'get' expects an INT as an argument",
                   fn_node->tok.pos
            ).Throw();
        }
    }

    return Object::callProperty(visitor, fn_node);
}

std::shared_ptr<flang::Object> flang::StringObject::add_to(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto stringObject = std::dynamic_pointer_cast<StringObject>(other)) {
        auto result = value + stringObject->value;
        return std::make_shared<StringObject>(result, tok);
    }
    return Object::add_to(other, token);
}

std::shared_ptr<flang::Object>
flang::StringObject::equal_to(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto strObj = std::dynamic_pointer_cast<StringObject>(other)) {
        return std::make_shared<BooleanObject>(value == strObj->value, tok);
    }
    return std::make_shared<BooleanObject>(false, tok);
}

std::shared_ptr<flang::Object>
flang::StringObject::not_equal_to(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto strObj = std::dynamic_pointer_cast<StringObject>(other)) {
        return std::make_shared<BooleanObject>(value != strObj->value, tok);
    }
    return std::make_shared<BooleanObject>(true, tok);
}

size_t flang::StringObject::hash(const flang::Token &token) const {
    return std::hash<std::string>{}(value);
}
