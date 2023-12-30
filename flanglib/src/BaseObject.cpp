#include <FObject.hpp>
#include "flang/ErrorType.hpp"
#include <fmt/core.h>

std::string flang::Object::getTypeString() const {
    return "object";
}

std::shared_ptr<flang::Object> flang::Object::add_to(const std::shared_ptr<Object>& other, const Token &token) {
    FError(RunTimeError,
           fmt::format("addition is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}
std::shared_ptr<flang::Object> flang::Object::sub_by(const std::shared_ptr<Object>& other, const Token &token) {
    FError(RunTimeError,
           fmt::format("subtraction is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}
std::shared_ptr<flang::Object> flang::Object::multiplied_by(const std::shared_ptr<Object>& other, const Token &token) {
    FError(RunTimeError,
           fmt::format("multiplication is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}
std::shared_ptr<flang::Object> flang::Object::divided_by(const std::shared_ptr<Object>& other, const Token &token) {
    FError(RunTimeError,
           fmt::format("division is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object>
flang::Object::modulo_by(const std::shared_ptr<Object> &other, const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("modulo is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}



std::shared_ptr<flang::Object> flang::Object::getProperty(const std::string& name) {
    FError(RunTimeError,
           fmt::format("{} does not contain property {}", tokToString(this->tok.tok), name),
           tok.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object> flang::Object::callProperty(flang::Interpreter& visitor, const std::shared_ptr<flang::FunctionCallNode>& fn_node) {
    FError(RunTimeError,
           fmt::format("{} does not contain property function {}", tokToString(this->tok.tok), fn_node->tok.value),
           tok.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object>
flang::Object::greater_than(const std::shared_ptr<Object> &other, const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("'>' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object>
flang::Object::less_than(const std::shared_ptr<Object> &other, const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("'<' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object>
flang::Object::greater_or_equal(const std::shared_ptr<Object> &other, const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("'>=' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object>
flang::Object::less_or_equal(const std::shared_ptr<Object> &other, const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("'<=' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object>
flang::Object::equal_to(const std::shared_ptr<Object> &other, const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("'==' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object>
flang::Object::not_equal_to(const std::shared_ptr<Object> &other, const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("'!=' is not supported by {} with {}", tokToString(this->tok.tok), tokToString(other->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

bool flang::Object::isReturn() const {
    return false;
}

std::shared_ptr<flang::Object>
flang::Object::call(flang::Interpreter &visitor, std::vector<std::shared_ptr<Object>> &arguments, const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("'{}' is not callable", tokToString(this->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);

}

std::shared_ptr<flang::Object>
flang::Object::unary_plus(const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("unary '+' is not supported by {}", tokToString(this->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object>
flang::Object::unary_minus(const flang::Token &token) {
    FError(RunTimeError,
           fmt::format("unary '-' is not supported by", tokToString(this->tok.tok)),
           token.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object>
flang::Object::unary_not(const flang::Token &token) {
    return std::make_shared<BooleanObject>(!isTrue(), token);
}

