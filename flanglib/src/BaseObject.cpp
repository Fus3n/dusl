#include "FObject.hpp"
#include "flang/ErrorType.hpp"
#include "fmt/core.h"

std::string flang::Object::getTypeString() const {
    return "object";
}

bool flang::Object::isTrue() const {
    return true;
}

bool flang::Object::isReturn() const {
    return false;
}

bool flang::Object::isBreak() const {
    return false;
}

flang::FResult flang::Object::add_to(const std::shared_ptr<Object>& other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("addition is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}
flang::FResult flang::Object::sub_by(const std::shared_ptr<Object>& other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("subtraction is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}
flang::FResult flang::Object::multiplied_by(const std::shared_ptr<Object>& other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("multiplication is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}
flang::FResult flang::Object::divided_by(const std::shared_ptr<Object>& other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("division is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::modulo_by(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("modulo is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

flang::FResult flang::Object::getProperty(const std::string& name, const Token& tok) {
    return FResult::createError(
            RunTimeError,
            fmt::format("{} does not contain property {}", getTypeString(), name),
            tok
    );
}

flang::FResult flang::Object::callProperty(Interpreter &visitor, const std::shared_ptr<flang::FunctionCallNode>& fn_node) {
    auto func = functions.find(fn_node->tok.value);
    if (func !=  functions.end()) {
        return func->second(*this, visitor, fn_node);
    }

    return FResult::createError(
            RunTimeError,
            fmt::format("{} does not contain property function {}", getTypeString(), fn_node->tok.value),
            tok
    );
}

flang::FResult
flang::Object::greater_than(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'>' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::less_than(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'<' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'>=' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::less_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'<=' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'==' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::not_equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'!=' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::call(Interpreter &visitor, ArgumentObject &args_node, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'{}' is not callable", tokToString(this->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::unary_plus(const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("unary '+' is not supported by {}", tokToString(this->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::unary_minus(const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("unary '-' is not supported by", tokToString(this->tok.tok)),
            tok
    );
}

flang::FResult
flang::Object::unary_not(const Token &token) {
    return FResult::createResult(std::make_shared<BooleanObject>(!isTrue(), token), token);
}

flang::FResult flang::Object::hash(const flang::Token &token) const {
    return FResult::createError(
            RunTimeError,
            fmt::format("{} is unhashable", getTypeString()),
            tok
    );
}

flang::FResult flang::Object::index(std::shared_ptr<ListObject> idx_args) {
    return FResult::createError(
            RunTimeError,
            fmt::format("{} does not support indexing", getTypeString()),
            tok
    );
}

flang::FResult
flang::Object::index_assign(std::shared_ptr<Object> &right, std::shared_ptr<ListObject> idx_args) {
    return FResult::createError(
            RunTimeError,
            fmt::format("{} does not support index assign", getTypeString()),
            tok
    );
}



