#include "flang/FObject.hpp"
#include "flang/ErrorType.hpp"
#include <fmt/core.h>
#include <cmath>

std::string flang::FloatObject::ToString() const {
    return fmt::format("{}", value);
//    return std::to_string(value);
}

bool flang::FloatObject::isTrue() const {
    return value != 0;
}
std::string flang::FloatObject::getTypeString() const {
    return "float";
}

std::shared_ptr<flang::Object>
flang::FloatObject::add_to(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value + intObject->value;
        return std::make_shared<FloatObject>(result, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value + floatObject->value;
        return std::make_shared<FloatObject>(result, tok);
    }

    return Object::add_to(other, token);
}

std::shared_ptr<flang::Object>
flang::FloatObject::sub_by(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value - intObject->value;
        return std::make_shared<FloatObject>(result, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value - floatObject->value;
        return std::make_shared<FloatObject>(result, tok);
    }

    return Object::sub_by(other, token);
}

std::shared_ptr<flang::Object>
flang::FloatObject::multiplied_by(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value * intObject->value;
        return std::make_shared<FloatObject>(result, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value * floatObject->value;
        return std::make_shared<FloatObject>(result, tok);
    }

    return Object::multiplied_by(other, token);
}

std::shared_ptr<flang::Object>
flang::FloatObject::divided_by(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        if (intObject->value == 0) {
            NumberError::throwZeroDivisionError(token, other);
            return std::make_shared<NoneObject>(token);
        }

        long double result = value / (double)intObject->value;
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
flang::FloatObject::modulo_by(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = fmodl(value, intObject->value);
        return std::make_shared<IntObject>(result, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = fmodl(value, floatObject->value);
        return std::make_shared<FloatObject>(result, tok);
    }
    return Object::modulo_by(other, token);
}


std::shared_ptr<flang::Object>
flang::FloatObject::greater_than(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value > intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value > floatObject->value, tok);
    }

    return Object::greater_than(other, token);
}

std::shared_ptr<flang::Object>
flang::FloatObject::less_than(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value < intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value < floatObject->value, tok);
    }

    return Object::greater_than(other, token);
}

std::shared_ptr<flang::Object>
flang::FloatObject::greater_or_equal(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value >= intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value >= floatObject->value, tok);
    }

    return Object::greater_than(other, token);
}

std::shared_ptr<flang::Object>
flang::FloatObject::less_or_equal(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value <= intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value <= floatObject->value, tok);
    }

    return Object::greater_than(other, token);
}

std::shared_ptr<flang::Object>
flang::FloatObject::equal_to(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value == intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value == floatObject->value, tok);
    }

    return std::make_shared<BooleanObject>(false, tok);
}

std::shared_ptr<flang::Object>
flang::FloatObject::not_equal_to(const std::shared_ptr<Object> &other, const flang::Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return std::make_shared<BooleanObject>(value != intObject->value, tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return std::make_shared<BooleanObject>(value != floatObject->value, tok);
    }

    return std::make_shared<BooleanObject>(true, tok);
}

std::shared_ptr<flang::Object> flang::FloatObject::unary_plus(const flang::Token &token) {
    return std::make_shared<FloatObject>(+value, token);
}

std::shared_ptr<flang::Object> flang::FloatObject::unary_minus(const flang::Token &token) {
    return std::make_shared<FloatObject>(-value, token);
}

