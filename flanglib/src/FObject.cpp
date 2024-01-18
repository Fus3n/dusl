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

flang::FResult flang::NumberError::throwZeroDivisionError(const Token &leftToken, const std::shared_ptr<Object> &other) {
    return FResult::createError(
        ZeroDivisionError,
        fmt::format("division by zero {} with {}", tokToString(leftToken.tok), tokToString(other->tok.tok)),
        leftToken
    );
}

std::string flang::BuiltinFunctionObject::toString() const {
    const void * address = static_cast<const void*>(this);
    return fmt::format("BuiltinFunction<{}, {}>", tok.value, address);
}

flang::FResult
flang::BuiltinFunctionObject::call(Interpreter &visitor, std::vector<std::shared_ptr<Object>> &arguments,
                                   const Token &token) {

    return body_func(visitor, arguments, token);
}

flang::FResult flang::BuiltinFunctionObject::hash(const flang::Token &token) const {
    const void * address = static_cast<const void*>(this);
    return FResult::createResult(
        std::make_shared<IntObject>(std::hash<const void*>{}(address), token),
        token
    );
}

std::string flang::FunctionObject::toString() const {
    const void * address = static_cast<const void*>(this);
    return fmt::format("Function<{}, {}>", func_name, address);
}

flang::FResult
flang::FunctionObject::call(Interpreter &visitor, std::vector<std::shared_ptr<Object>> &arguments,
                            const Token &token) {

    if (args.size() < arguments.size() || args.size() > arguments.size()) {
        return FResult::createError(NameError, fmt::format("{} takes {} arguments but {} were given", tok.value, args.size(), arguments.size()), tok);
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
    if (block_res.isError())
        return block_res; // TODO: maybe exit scope before returning?

    visitor.ctx.exitScope();
    visitor.ctx.setName(current_ctx_name);

    if (block_res.result->isReturn()) {
        // unwrap the return wrapper
        auto ret = dynamic_cast<ReturnObject*>(block_res.result.get());
        return FResult::createResult(ret->return_obj, tok);
    }

    // TODO: decide weather to return none by default or last expression
    return FResult::createResult(block_res.result, tok);
}

flang::FResult flang::FunctionObject::hash(const flang::Token &token) const {
    const void * address = static_cast<const void*>(this);
    return FResult::createResult(
        std::make_shared<IntObject>(std::hash<const void*>{}(address), token),
        token
    );
}

std::string flang::BooleanObject::toString() const {
    return value ? "true": "false";
}

bool flang::BooleanObject::isTrue() const {
    return value;
}

flang::FResult flang::BooleanObject::hash(const flang::Token &token) const {
    return FResult::createResult(
        std::make_shared<IntObject>(std::hash<bool>{}(value), token),
        token
    );
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

flang::FResult flang::DictionaryObject::callProperty(Interpreter &visitor, const std::shared_ptr<flang::FunctionCallNode> &fn_node) {
    if (fn_node->tok.value == "get") {
        auto res = verifyArgsCount(fn_node->args.size(), 1, tok);
        if (res.has_value()) {
            return FResult::createError(
                RunTimeError,
                res.value(),
                fn_node->tok
            );
        }

        auto first_arg = fn_node->args[0]->accept(visitor);
        if (first_arg.isError()) return first_arg;

        auto hashed = first_arg.result->hash(tok);
        if (hashed.isError())
            return hashed;

        auto intObject = dynamic_cast<IntObject*>(hashed.result.get()); // TODO: might cause issue?

        auto it = items.find(intObject->value);
        if (it == items.end()) {
            return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
        }

        return FResult::createResult(std::get<1>(it->second), tok);
    } else if (fn_node->tok.value == "keys") {
        auto res = verifyArgsCount(fn_node->args.size(), 0, tok);
        if (res.has_value()) {
            return FResult::createError(
                RunTimeError,
                res.value(),
                fn_node->tok
            );
        }

        auto list = std::make_shared<ListObject>(tok);
        list->items.reserve(items.size());
        for (auto& item: items) {
            list->items.push_back(std::get<0>(item.second));
        }

        return FResult::createResult(list, tok);
    } else if (fn_node->tok.value == "values") {
        auto res = verifyArgsCount(fn_node->args.size(), 0, tok);
        if (res.has_value()) {
            return FResult::createError(
                    RunTimeError,
                    res.value(),
                    fn_node->tok
            );
        }

        auto list = std::make_shared<ListObject>(tok);
        list->items.reserve(items.size());
        for (auto& item: items) {
            list->items.push_back(std::get<1>(item.second));
        }

        return FResult::createResult(list, tok);
    }

    return Object::callProperty(visitor, fn_node);
}

flang::FResult flang::DictionaryObject::index(std::shared_ptr<ListObject> idx_args) {
    if (idx_args->items.size() != 1) {
        return FResult::createError(
            IndexError,
            fmt::format("dict index takes 1 argument but {} were given", idx_args->items.size()),
            tok
        );
    }

    auto first_arg = idx_args->items[0];
    auto hashed = first_arg->hash(tok);
    if (hashed.isError())
        return hashed;

    auto intObject = dynamic_cast<IntObject*>(hashed.result.get()); // TODO: might cause issue?

    auto it = items.find(intObject->value);
    if (it == items.end()) {
        return FResult::createError(
                NameError,
                fmt::format("key {} not found in dict", first_arg->toString()),
                first_arg->tok
        );
    }

    return FResult::createResult(std::get<1>(it->second), tok);
}

flang::FResult
flang::DictionaryObject::index_assign(std::shared_ptr<Object> &right, std::shared_ptr<ListObject> idx_args) {
    if (idx_args->items.size() != 1) {
        return FResult::createError(
            IndexError,
            fmt::format("dict index takes 1 argument but {} were given", idx_args->items.size()),
            tok
        );
    }
    auto first_arg = idx_args->items[0];
    auto hashed = first_arg->hash(tok);
    if (hashed.isError())
        return hashed;

    auto intObject = dynamic_cast<IntObject*>(hashed.result.get()); // TODO: might cause issue?
    items[intObject->value] = std::make_tuple(first_arg, right);
    return FResult::createResult(right, tok);
}

flang::FResult flang::DictionaryObject::getProperty(const std::string &name, const Token &tok) {
    if (name == "size") {
        return FResult::createResult(std::make_shared<IntObject>(items.size(), tok), tok);
    }
    return Object::getProperty(name, tok);
}

std::string flang::FResult::toString() const {
    const auto &result_str = result != nullptr ? result->toString(): "none";
    const auto &err_str = err != nullptr ? err->toString(): "none";
    return fmt::format("Result(is_error: {}, result: {}, error: {})", is_error,  result_str, err_str);
}

flang::FResult
flang::FResult::createError(ErrorType err_type, std::string_view msg, const Token& tok) {
    auto error = std::make_shared<ErrorObject>(err_type, msg, tok.pos, tok);
    return FResult(error);
}

flang::FResult
flang::FResult::createResult(const std::shared_ptr<Object>& _result, const Token &tok) {
    return FResult(_result);
}

bool flang::FResult::isError() const {
    return is_error;
}

std::string flang::BreakObject::toString() const {
    return fmt::format("Break({})", tokToString(tok.tok));
}

std::string flang::BreakObject::getTypeString() const {
    return "break";
}

bool flang::BreakObject::isBreak() const {
    return true;
}

std::string flang::RangeObject::toString() const {
    return fmt::format("Rng({}:{})",  start, end);
}

std::string flang::RangeObject::getTypeString() const {
    return "range";
}
