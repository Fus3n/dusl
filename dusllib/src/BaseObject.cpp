#include <dusl/DObject.hpp>
#include <dusl/ErrorType.hpp>
#include "fmt/core.h"

std::string dusl::Object::getTypeString() const {
    return "object";
}

bool dusl::Object::isTrue() const {
    return true;
}

bool dusl::Object::isReturn() const {
    return false;
}

bool dusl::Object::isBreak() const {
    return false;
}

dusl::FResult dusl::Object::add_to(const std::shared_ptr<Object>& other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("addition is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}
dusl::FResult dusl::Object::sub_by(const std::shared_ptr<Object>& other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("subtraction is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}
dusl::FResult dusl::Object::multiplied_by(const std::shared_ptr<Object>& other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("multiplication is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}
dusl::FResult dusl::Object::divided_by(const std::shared_ptr<Object>& other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("division is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::modulo_by(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("modulo is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

dusl::FResult dusl::Object::getProperty(const std::string& name, const Token& tok) {
    return FResult::createError(
            RunTimeError,
            fmt::format("{} does not contain property {}", getTypeString(), name),
            tok
    );
}

dusl::FResult dusl::Object::callProperty(Interpreter &visitor, const std::shared_ptr<dusl::FunctionCallNode>& fn_node) {
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

dusl::FResult
dusl::Object::greater_than(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'>' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::less_than(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'<' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'>=' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::less_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'<=' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'==' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::not_equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'!=' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::call(Interpreter &visitor, ArgumentObject &args_node, const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("'{}' is not callable", tokToString(this->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::unary_plus(const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("unary '+' is not supported by {}", tokToString(this->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::unary_minus(const Token &token) {
    return FResult::createError(
            RunTimeError,
            fmt::format("unary '-' is not supported by", tokToString(this->tok.tok)),
            tok
    );
}

dusl::FResult
dusl::Object::unary_not(const Token &token) {
    return FResult::createResult(std::make_shared<BooleanObject>(!isTrue(), token), token);
}

dusl::FResult dusl::Object::hash(const dusl::Token &token) const {
    return FResult::createError(
            RunTimeError,
            fmt::format("{} is unhashable", getTypeString()),
            tok
    );
}

dusl::FResult dusl::Object::index(std::shared_ptr<ListObject> idx_args) {
    return FResult::createError(
            RunTimeError,
            fmt::format("{} does not support indexing", getTypeString()),
            tok
    );
}

dusl::FResult
dusl::Object::index_assign(std::shared_ptr<Object> &right, std::shared_ptr<ListObject> idx_args) {
    return FResult::createError(
            RunTimeError,
            fmt::format("{} does not support index assign", getTypeString()),
            tok
    );
}



