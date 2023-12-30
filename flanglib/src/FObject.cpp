#include "flang/FObject.hpp"
#include "flang/ErrorType.hpp"
#include "flang/Interpreter.hpp"
#include <fmt/core.h>
#include <sstream>
#include "flang/utils/fcore.h"

std::string flang::NoneObject::ToString() const {
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

std::string flang::ListObject::ToString() const {
    std::stringstream ss;
    ss << "[";
    for (int i = 0; i < items.size(); i++) {
        ss << items[i]->ToString();
        if (i != items.size() - 1)
            ss << ", ";
    }
    ss << "]";
    return ss.str();
}

bool flang::ListObject::isTrue() const {
    return !items.empty();
}

std::shared_ptr<flang::Object> flang::ListObject::splitAtDelimiter(const std::string &value, const std::string &delim, const Token& tok) {
    std::vector<std::shared_ptr<flang::Object>> items;
    std::istringstream stream(value);
    std::string character;

    int i = 0;
    size_t start = 0, end = 0;
    while ((end = value.find_first_of(delim, start)) != std::string::npos) {
        items.push_back(std::move(std::make_shared<StringObject>(value.substr(start, end - start), tok)));
        start = end + 1; // Move past the delimiter
        i++;
    }
    items.push_back(std::move(std::make_shared<StringObject>(value.substr(start), tok)));
    return std::make_shared<ListObject>(items, tok);
}

std::shared_ptr<flang::Object>
flang::ListObject::callProperty(flang::Interpreter &visitor, const std::shared_ptr<flang::FunctionCallNode> &fn_node) {
    if (fn_node->tok.value == "get") {
        auto res= verifyArgsCount(fn_node->args.size(), 1, tok);
        if (res.has_value()) {
            FError(RunTimeError, res.value(), tok.pos).Throw();
        }

        auto first_arg = fn_node->args[0]->accept(visitor);
        if (auto intObj = dynamic_cast<IntObject*>(first_arg.get())) {
            if (intObj->value > items.size() - 1) {
                FError(RunTimeError, fmt::format("Index out of range {}", intObj->value), first_arg->tok.pos).Throw();
            }

            return items[intObj->value];
        }
    } else if (fn_node->tok.value == "push") {
        auto res= verifyArgsCount(fn_node->args.size(), 1, tok);
        if (res.has_value())
            FError(RunTimeError, res.value(), tok.pos).Throw();
        auto val = fn_node->args[0]->accept(visitor);
        items.push_back(val);
        return std::make_shared<NoneObject>(tok);
    }

    return Object::callProperty(visitor, fn_node);
}

std::shared_ptr<flang::Object> flang::ListObject::getProperty(const std::string &name) {
    if (name == "size") {
        return std::make_shared<IntObject>(items.size(), tok);
    }
    return Object::getProperty(name);
}

std::string flang::FunctionObject::ToString() const {
    return fmt::format("Function<{}>", func_name);
}

bool flang::FunctionObject::isTrue() const {
    return true;
}

std::string flang::BuiltinFunctionObject::ToString() const {
    return fmt::format("BuiltinFunctionObject<{}>", tok.value);
}

bool flang::BuiltinFunctionObject::isTrue() const {
    return true;
}

std::shared_ptr<flang::Object>
flang::BuiltinFunctionObject::call(flang::Interpreter &visitor, std::vector<std::shared_ptr<Object>> &arguments,
                                   const flang::Token &token) {
    return body_func(visitor, arguments, token);
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
    visitor.ctx.setName(current_ctx_name + "." + tok.value);
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

std::string flang::BooleanObject::ToString() const {
    return value ? "true": "false";
}

bool flang::BooleanObject::isTrue() const {
    return value;
}

std::string flang::ReturnObject::ToString() const {
    return return_obj->ToString();
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
