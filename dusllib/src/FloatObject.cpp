#include <dusl/DObject.hpp>
#include <dusl/ErrorType.hpp>
#include <dusl/utils/dusl_core.hpp>
#include <fmt/core.h>
#include <cmath>

std::string dusl::FloatObject::toString() const {
    return fmt::format("{}", value);
//    return std::to_string(value);
}

bool dusl::FloatObject::isTrue() const {
    return value != 0;
}
std::string dusl::FloatObject::getTypeString() const {
    return "float";
}

dusl::FResult
dusl::FloatObject::add_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value + intObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value + floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::add_to(other, token);
}

dusl::FResult
dusl::FloatObject::sub_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value - intObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value - floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::sub_by(other, token);
}

dusl::FResult
dusl::FloatObject::multiplied_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value * intObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value * floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::multiplied_by(other, token);
}

dusl::FResult
dusl::FloatObject::divided_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (const auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        if (intObject->value == 0) {
            return NumberError::throwZeroDivisionError(token, other);
        }

        long double result = value / (double)intObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    } if (const auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        if (floatObject->value == 0) {
            return NumberError::throwZeroDivisionError(token, other);
        }
        auto result = value / floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::divided_by(other, token);
}

dusl::FResult
dusl::FloatObject::modulo_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = fmodl(value, intObject->value);
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = fmodl(value, floatObject->value);
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }
    return Object::modulo_by(other, token);
}


dusl::FResult
dusl::FloatObject::greater_than(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value > intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value > floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

dusl::FResult
dusl::FloatObject::less_than(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value < intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value < floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

dusl::FResult
dusl::FloatObject::greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value >= intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value >= floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

dusl::FResult
dusl::FloatObject::less_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value <= intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value <= floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

dusl::FResult
dusl::FloatObject::equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == floatObject->value, tok), tok);
    }

    return FResult::createResult(std::make_shared<BooleanObject>(false, tok), tok);
}

dusl::FResult
dusl::FloatObject::not_equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != floatObject->value, tok), tok);
    }

    return FResult::createResult(std::make_shared<BooleanObject>(true, tok), tok);
}

dusl::FResult dusl::FloatObject::unary_plus(const Token &token) {
    return FResult::createResult(std::make_shared<FloatObject>(+value, token), tok);
}

dusl::FResult dusl::FloatObject::unary_minus(const Token &token) {
    return FResult::createResult(std::make_shared<FloatObject>(-value, token), tok);
}

dusl::FResult dusl::FloatObject::hash(const dusl::Token &token) const {
    return FResult::createResult(std::make_shared<IntObject>(std::hash<long double>{}(value), token), tok);
}

dusl::FResult dusl::FloatObject::to_string(FloatObject &floating, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res_arg = verifyArgsCount(fn_node->args_node.args.size(), 0, floating.tok);
    if (res_arg.has_value())
        return FResult::createError(RunTimeError, res_arg.value(), fn_node->tok);

    auto res = std::make_shared<StringObject>(fmt::format("{}", floating.value), floating.tok);
    return FResult::createResult(res, fn_node->tok);
}

void dusl::FloatObject::init_functions() {
    functions["toStr"] = reinterpret_cast<PropertyFunction>(FloatObject::to_string);
}

