#include "flang/FObject.hpp"
#include "flang/ErrorType.hpp"
#include <fmt/core.h>
#include <cmath>

std::string flang::IntObject::toString() const {
    return std::to_string(value);
}

bool flang::IntObject::isTrue() const {
    return value != 0;
}
std::string flang::IntObject::getTypeString() const {
    return "int";
}
flang::FResult flang::IntObject::add_to(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value + intObject->value;
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value + floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::add_to(other, token);
}

flang::FResult flang::IntObject::sub_by(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value - intObject->value;
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value - floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }
    return Object::sub_by(other, token);
}

flang::FResult
flang::IntObject::multiplied_by(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value * intObject->value;
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value * floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }
    return Object::multiplied_by(other, token);
}

flang::FResult
flang::IntObject::divided_by(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        if (intObject->value == 0) {
            return NumberError::throwZeroDivisionError(token, other);
        }

        long double result = value / (long double)intObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        if (floatObject->value == 0) {
            return NumberError::throwZeroDivisionError(token, other);
        }
        auto result = value / floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::divided_by(other, token);
}

flang::FResult
flang::IntObject::modulo_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value % intObject->value;
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = fmodl(value, floatObject->value);
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }
    return Object::modulo_by(other, token);
}



flang::FResult
flang::IntObject::greater_than(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value > intObject->value, tok), tok);;
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value > floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

flang::FResult
flang::IntObject::less_than(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value < intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value < floatObject->value, tok), tok);
    }

    return Object::less_than(other, token);
}

flang::FResult
flang::IntObject::greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value >= intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value >= floatObject->value, tok), tok);
    }

    return Object::greater_or_equal(other, token);
}

flang::FResult
flang::IntObject::less_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value <= intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value <= floatObject->value, tok), tok);
    }

    return Object::less_or_equal(other, token);
}

flang::FResult
flang::IntObject::equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == floatObject->value, tok), tok);
    }

    return FResult::createResult(std::make_shared<BooleanObject>(false, tok), tok);
}

flang::FResult
flang::IntObject::not_equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != floatObject->value, tok), tok);
    }

    return FResult::createResult(std::make_shared<BooleanObject>(true, tok), tok);
}

flang::FResult
flang::IntObject::unary_plus(const Token &token) {
    return FResult::createResult(std::make_shared<IntObject>(+value, token), tok);
}

flang::FResult
flang::IntObject::unary_minus(const Token &token) {
    return FResult::createResult(std::make_shared<IntObject>(-value, token), tok);
}

flang::FResult flang::IntObject::hash(const flang::Token &token) const {
    return FResult::createResult(std::make_shared<IntObject>(std::hash<int64_t>{}(value), token), tok);
}

flang::FResult flang::IntObject::to_string(IntObject &integer, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = std::make_shared<StringObject>(fmt::format("{}", integer.value), integer.tok);
    return FResult::createResult(res, fn_node->tok);
}

void flang::IntObject::init_functions() {
    functions["toStr"] = reinterpret_cast<PropertyFunction>(IntObject::to_string);
}


