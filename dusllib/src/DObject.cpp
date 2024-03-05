#include <dusl/DObject.hpp>
#include <dusl/ErrorType.hpp>
#include <dusl/Interpreter.hpp>
#include <dusl/utils/dusl_core.hpp>
#include <fmt/core.h>
#include <sstream>
#include <algorithm>

std::string dusl::NoneObject::toString() const {
    return "none";
}

bool dusl::NoneObject::isTrue() const {
    return false;
}

std::string dusl::NoneObject::getTypeString() const {
    return "none";
}

dusl::FResult dusl::NumberError::throwZeroDivisionError(const Token &leftToken, const std::shared_ptr<Object> &other) {
    return FResult::createError(
        ZeroDivisionError,
        fmt::format("division by zero {} with {}", tokToString(leftToken.tok), tokToString(other->tok.tok)),
        leftToken
    );
}

std::string dusl::BuiltinFunctionObject::toString() const {
    const void * address = static_cast<const void*>(this);
    return fmt::format("BuiltinFunction<{}, {}>", tok.value, address);
}

dusl::FResult
dusl::BuiltinFunctionObject::call(Interpreter &visitor, ArgumentObject &args_node,
                                  const Token &token) {

    return body_func(visitor, args_node, token);
}

dusl::FResult dusl::BuiltinFunctionObject::hash(const dusl::Token &token) const {
    const void * address = static_cast<const void*>(this);
    return FResult::createResult(
        std::make_shared<IntObject>(std::hash<const void*>{}(address), token),
        token
    );
}

std::string dusl::FunctionObject::toString() const {
    const void * address = static_cast<const void*>(this);
    return fmt::format("Function<{}, {}>", func_name, address);
}

dusl::FResult
dusl::FunctionObject::call(Interpreter &visitor, ArgumentObject &arguments, const Token &token) {
    auto arg_total = args.size();
    auto given_total = arguments.args.size() + arguments.default_args.size();
    auto max_expected_args = std::max(arg_total, given_total);


    if (given_total > max_expected_args) {
        return FResult::createError(NameError, fmt::format("{} takes at most {} arguments but {} were given", func_name, max_expected_args, given_total), tok);
    } else if (given_total < arg_total) {
        return FResult::createError(NameError, fmt::format("{} takes at least {} arguments but {} were given", func_name, arg_total, given_total), tok);
    } else if (given_total > arg_total && arguments.default_args.empty()) {
        return FResult::createError(NameError, fmt::format("{} takes at most {} arguments but {} were given", func_name, arg_total, given_total), tok);
    }

    // preserve global symbols
    auto current_ctx_name = visitor.ctx.getName();
    auto new_name = current_ctx_name + "." + func_name;
    visitor.ctx.setName(new_name);
    visitor.ctx.enterScope();

    for (int i = 0; i < arguments.args.size(); i++) {
        visitor.ctx.currentSymbol().setValue(args[i], arguments.args[i]);

        if (arguments.default_args.find(args[0]) != arguments.default_args.end()) {
            visitor.ctx.currentSymbol().setValue(args[0], arguments.default_args[args[0]]);
        }
    }

    for (auto& [name, value]: default_args) {
        // prioritize values from arguments as it's given by user
        if (arguments.default_args.find(name) != arguments.default_args.end()) {
            // the value for this name is provided
            visitor.ctx.currentSymbol().setValue(name, arguments.default_args[name]);
        } else {
            // the value is not provided, use default
            visitor.ctx.currentSymbol().setValue(name, value);
        }
    }

    for (auto& name: args) {
        if (arguments.default_args.find(name) != arguments.default_args.end()) {
            visitor.ctx.currentSymbol().setValue(name, arguments.default_args[name]);
        }
    }

    auto block_res = block->accept(visitor);
    visitor.ctx.exitScope();
    visitor.ctx.setName(current_ctx_name);

    if (block_res.isError())
        return block_res;

    if (block_res.result->isReturn()) {
        // unwrap the return wrapper
        auto ret = dynamic_cast<ReturnObject*>(block_res.result.get());
        return FResult::createResult(ret->return_obj, tok);
    }

    // TODO: decide weather to return none by default or last expression
    return FResult::createResult(block_res.result, tok);
}

dusl::FResult dusl::FunctionObject::hash(const dusl::Token &token) const {
    const void * address = static_cast<const void*>(this);
    return FResult::createResult(
        std::make_shared<IntObject>(std::hash<const void*>{}(address), token),
        token
    );
}

std::string dusl::BooleanObject::toString() const {
    return value ? "true": "false";
}

bool dusl::BooleanObject::isTrue() const {
    return value;
}

dusl::FResult dusl::BooleanObject::hash(const dusl::Token &token) const {
    return FResult::createResult(
        std::make_shared<IntObject>(std::hash<bool>{}(value), token),
        token
    );
}

std::string dusl::ReturnObject::toString() const {
    return return_obj->toString();
}

bool dusl::ReturnObject::isTrue() const {
    return return_obj->isTrue();
}

std::string dusl::ReturnObject::getTypeString() const {
    return fmt::format("Return({})", return_obj->getTypeString());
}

bool dusl::ReturnObject::isReturn() const {
    return true;
}

std::string dusl::FResult::toString() const {
    const auto &result_str = result != nullptr ? result->toString(): "none";
    const auto &err_str = err != nullptr ? err->toString(): "none";
    return fmt::format("Result(is_error: {}, result: {}, error: {})", is_error,  result_str, err_str);
}

dusl::FResult
dusl::FResult::createError(ErrorType err_type, std::string msg, const Token& tok) {
    auto error = std::make_shared<ErrorObject>(err_type, msg, tok.pos, tok);
    return FResult(error);
}

dusl::FResult
dusl::FResult::createResult(const std::shared_ptr<Object>& _result, const Token &tok) {
    return FResult(_result);
}

bool dusl::FResult::isError() const {
    return is_error;
}

std::string dusl::BreakObject::toString() const {
    return fmt::format("Break({})", tokToString(tok.tok));
}

std::string dusl::BreakObject::getTypeString() const {
    return "break";
}

bool dusl::BreakObject::isBreak() const {
    return true;
}

std::string dusl::RangeObject::toString() const {
    return fmt::format("Rng({}:{})",  start, end);
}

dusl::FResult dusl::RangeObject::to_list(RangeObject &range, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    std::vector<std::shared_ptr<Object>> items;
    items.resize(range.end);
    for (int64_t i = range.start; i < range.end; i++) {
        items[i] = std::make_shared<IntObject>(createInt(i, range.tok));
    }
    const auto list = std::make_shared<ListObject>(range.tok);
    list->items = std::move(items);

    return FResult::createResult(list, range.tok);
}

std::string dusl::RangeObject::getTypeString() const {
    return "range";
}

std::string dusl::ArgumentObject::toString() const {
    return std::string();
}
