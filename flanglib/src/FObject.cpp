#include "flang/FObject.hpp"
#include "flang/ErrorType.hpp"
#include "flang/Interpreter.hpp"
#include "utils/fcore.h"
#include <fmt/core.h>
#include <sstream>

std::string flang::NoneObject::toString() const {
    return "none";
}

bool flang::NoneObject::isTrue() const {
    return false;
}

std::string flang::NoneObject::getTypeString() const {
    return "none";
}

void flang::NumberError::throwZeroDivisionError(const Token &leftToken, const std::shared_ptr<Object> &other) {
    FError(ZeroDivisionError,
           fmt::format("division by zero {} with {}", tokToString(leftToken.tok), tokToString(other->tok.tok)),
           leftToken.pos
    ).Throw();
}

std::string flang::BuiltinFunctionObject::toString() const {
    const void * address = static_cast<const void*>(this);
    return fmt::format("BuiltinFunction<{}, {}>", tok.value, address);
}

std::shared_ptr<flang::Object>
flang::BuiltinFunctionObject::call(flang::Interpreter &visitor, std::vector<std::shared_ptr<Object>> &arguments,
                                   const flang::Token &token) {
    return body_func(visitor, arguments, token);
}

size_t flang::BuiltinFunctionObject::hash(const flang::Token &token) const {
    const void * address = static_cast<const void*>(this);
    return std::hash<const void*>{}(address);
}

std::string flang::FunctionObject::toString() const {
    const void * address = static_cast<const void*>(this);
    return fmt::format("Function<{}, {}>", tok.value, address);
}

std::shared_ptr<flang::Object>
flang::FunctionObject::call(flang::Interpreter &visitor, std::vector<std::shared_ptr<Object>> &arguments,
                            const flang::Token &token) {

    if (args.size() < arguments.size() || args.size() > arguments.size()) {
        FError(NameError, fmt::format("{} takes {} arguments but {} were given", tok.value, args.size(), args.size()), tok.pos)
                .Throw();
    }

    // preserve global symbols
    auto current_ctx_name = visitor.ctx.getName();
    auto new_name = current_ctx_name + "." + tok.value;
    visitor.ctx.setName(new_name);
    visitor.ctx.enterScope();

    for (int i = 0; i < args.size(); i++) {
        // funcDef Node's all argument are just VarAccess node,
        // so we just get the tok.value which should be the name of the variable
        // and set the value of the variable to the value of the argument
        visitor.ctx.currenSymbol().setValue(args[i]->tok.value, arguments[i]);
    }

    auto block_res = block->accept(visitor);
    visitor.ctx.exitScope();
    visitor.ctx.setName(current_ctx_name);

    if (block_res->isReturn()) {
        // unwrap the return wrapper
        auto ret = dynamic_cast<ReturnObject*>(block_res.get());
        return ret->return_obj;
    }
    return block_res;

}

size_t flang::FunctionObject::hash(const flang::Token &token) const {
    const void * address = static_cast<const void*>(this);
    return std::hash<const void*>{}(address);
}

std::string flang::BooleanObject::toString() const {
    return value ? "true": "false";
}

bool flang::BooleanObject::isTrue() const {
    return value;
}

size_t flang::BooleanObject::hash(const flang::Token &token) const {
    return std::hash<bool>{}(value);
}

std::string flang::ReturnObject::toString() const {
    return return_obj->toString();
}

bool flang::ReturnObject::isTrue() const {
    return return_obj->isTrue();
}

std::string flang::ReturnObject::getTypeString() const {
    return fmt::format("Return({})", return_obj->getTypeString());
}

bool flang::ReturnObject::isReturn() const {
    return true;
}

std::string flang::DictionaryObject::toString() const {
    std::stringstream ss;
    ss << "{";
    for (auto& item: items) {
        ss << std::get<0>(item.second)->toString();
        ss << ": ";
        ss << std::get<1>(item.second)->toString();
        ss << ", ";
    }
    ss << "}";
    return ss.str();
}

std::string flang::DictionaryObject::getTypeString() const {
    return "dict";
}

std::shared_ptr<flang::Object> flang::DictionaryObject::callProperty(flang::Interpreter &visitor, const std::shared_ptr<flang::FunctionCallNode> &fn_node) {
    if (fn_node->tok.value == "get") {
        auto res = verifyArgsCount(fn_node->args.size(), 1, tok);
        if (res.has_value()) {
            FError(RunTimeError,
                   res.value(),
                   fn_node->tok.pos
            ).Throw();
        }
        auto first_arg = fn_node->args[0]->accept(visitor);
        auto hashed = first_arg->hash(tok);
        auto it = items.find(hashed);
        if (it == items.end()) {
            FError(NameError,
                   fmt::format("key {} not found in dict", first_arg->toString()),
                   fn_node->tok.pos
                   ).Throw();

        }
        return std::get<1>(it->second);
    } else if (fn_node->tok.value == "keys") {
        auto res = verifyArgsCount(fn_node->args.size(), 0, tok);
        if (res.has_value()) {
            FError(RunTimeError,
                   res.value(),
                   fn_node->tok.pos
            ).Throw();
        }

        auto list = std::make_shared<ListObject>(tok);
        list->items.reserve(items.size());
        for (auto& item: items) {
            list->items.push_back(std::get<0>(item.second));
        }

        return list;
    } else if (fn_node->tok.value == "values") {
        auto res = verifyArgsCount(fn_node->args.size(), 0, tok);
        if (res.has_value()) {
            FError(RunTimeError,
                   res.value(),
                   fn_node->tok.pos
            ).Throw();
        }

        auto list = std::make_shared<ListObject>(tok);
        list->items.reserve(items.size());
        for (auto& item: items) {
            list->items.push_back(std::get<1>(item.second));
        }

        return list;
    }

    return Object::callProperty(visitor, fn_node);
}

std::shared_ptr<flang::Object> flang::DictionaryObject::getProperty(const std::string &name) {
    auto hashed = std::hash<std::string>{}(name);
    auto it = items.find(hashed);
    if (it == items.end()) {
        FError(NameError,
               fmt::format("key {} not found in dict", name),
               tok.pos
        ).Throw();
    }

    return std::get<1>(it->second);
}
