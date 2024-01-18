#include "flang/FObject.hpp"
#include "flang/ErrorType.hpp"
#include <fmt/core.h>
#include <cmath>

std::string flang::FloatObject::toString() const {
    return fmt::format("{}", value);
//    return std::to_string(value);
}

bool flang::FloatObject::isTrue() const {
    return value != 0;
}
std::string flang::FloatObject::getTypeString() const {
    return "float";
}

flang::FResult
flang::FloatObject::add_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value + intObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value + floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::add_to(other, token);
}

flang::FResult
flang::FloatObject::sub_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value - intObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value - floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::sub_by(other, token);
}

flang::FResult
flang::FloatObject::multiplied_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value * intObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value * floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::multiplied_by(other, token);
}

flang::FResult
flang::FloatObject::divided_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        if (intObject->value == 0) {
            return NumberError::throwZeroDivisionError(token, other);
        }

        long double result = value / (double)intObject->value;
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
flang::FloatObject::modulo_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = fmodl(value, intObject->value);
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = fmodl(value, floatObject->value);
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }
    return Object::modulo_by(other, token);
}


flang::FResult
flang::FloatObject::greater_than(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value > intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value > floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

flang::FResult
flang::FloatObject::less_than(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value < intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value < floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

flang::FResult
flang::FloatObject::greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value >= intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value >= floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

flang::FResult
flang::FloatObject::less_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value <= intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value <= floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

flang::FResult
flang::FloatObject::equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == floatObject->value, tok), tok);
    }

    return FResult::createResult(std::make_shared<BooleanObject>(false, tok), tok);
}

flang::FResult
flang::FloatObject::not_equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != floatObject->value, tok), tok);
    }

    return FResult::createResult(std::make_shared<BooleanObject>(true, tok), tok);
}

flang::FResult flang::FloatObject::unary_plus(const Token &token) {
    return FResult::createResult(std::make_shared<FloatObject>(+value, token), tok);
}

flang::FResult flang::FloatObject::unary_minus(const Token &token) {
    return FResult::createResult(std::make_shared<FloatObject>(-value, token), tok);
}

flang::FResult flang::FloatObject::hash(const flang::Token &token) const {
    return FResult::createResult(std::make_shared<IntObject>(std::hash<long double>{}(value), token), tok);
}

flang::FResult flang::FloatObject::to_string(FloatObject &floating, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = std::make_shared<StringObject>(fmt::format("{}", floating.value), floating.tok);
    return FResult::createResult(res, fn_node->tok);
}

void flang::FloatObject::init_functions() {
    functions["toStr"] = reinterpret_cast<PropertyFunction>(FloatObject::to_string);
}

