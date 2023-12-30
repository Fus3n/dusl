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
std::shared_ptr<flang::Object> flang::IntObject::add_to(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value + intObject->value;
        return std::make_shared<IntObject>(result, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value + floatObject->value;
        return std::make_shared<FloatObject>(result, tok);
    }

    return Object::add_to(other, token);
}

std::shared_ptr<flang::Object> flang::IntObject::sub_by(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value - intObject->value;
        return std::make_shared<IntObject>(result, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value - floatObject->value;
        return std::make_shared<FloatObject>(result, tok);
    }
    return Object::sub_by(other, token);
}

std::shared_ptr<flang::Object> flang::IntObject::multiplied_by(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value * intObject->value;
        return std::make_shared<IntObject>(result, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value * floatObject->value;
        return std::make_shared<FloatObject>(result, tok);
    }
    return Object::multiplied_by(other, token);
}

std::shared_ptr<flang::Object> flang::IntObject::divided_by(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        if (intObject->value == 0) {
            NumberError::throwZeroDivisionError(token, other);
            return std::make_shared<NoneObject>(token);
        }

        long double result = value / (long double)intObject->value;
        return std::make_shared<FloatObject>(result, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        if (floatObject->value == 0) {
            NumberError::throwZeroDivisionError(token, other);
            return std::make_shared<NoneObject>(token);
        }
        auto result = value / floatObject->value;
        return std::make_shared<FloatObject>(result, tok);
    }

    return Object::divided_by(other, token);
}

std::shared_ptr<flang::Object>
flang::IntObject::modulo_by(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value % intObject->value;
        return std::make_shared<IntObject>(result, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = fmodl(value, floatObject->value);
        return std::make_shared<FloatObject>(result, tok);
    }
    return Object::modulo_by(other, token);
}



std::shared_ptr<flang::Object>
flang::IntObject::greater_than(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value > intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value > floatObject->value, tok);
    }

    return Object::greater_than(other, token);
}

std::shared_ptr<flang::Object>
flang::IntObject::less_than(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value < intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value < floatObject->value, tok);
    }

    return Object::less_than(other, token);
}

std::shared_ptr<flang::Object>
flang::IntObject::greater_or_equal(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value >= intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value >= floatObject->value, tok);
    }

    return Object::greater_or_equal(other, token);
}

std::shared_ptr<flang::Object>
flang::IntObject::less_or_equal(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value <= intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value <= floatObject->value, tok);
    }

    return Object::less_or_equal(other, token);
}

std::shared_ptr<flang::Object>
flang::IntObject::equal_to(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value == intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value == floatObject->value, tok);
    }

    return std::make_shared<BooleanObject>(false, tok);
}

std::shared_ptr<flang::Object>
flang::IntObject::not_equal_to(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value != intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value != floatObject->value, tok);
    }

    return std::make_shared<BooleanObject>(true, tok);
}

std::shared_ptr<flang::Object>
flang::IntObject::unary_plus(const flang::Token &token) {
    return std::make_shared<IntObject>(+value, token);
}

std::shared_ptr<flang::Object>
flang::IntObject::unary_minus(const flang::Token &token) {
    return std::make_shared<IntObject>(-value, token);
}

size_t flang::IntObject::hash(const flang::Token &token) const {
    return std::hash<int64_t>{}(value);
}

