#include <dusl/DObject.hpp>
#include <dusl/ErrorType.hpp>
#include <dusl/utils/dusl_core.hpp>
#include <fmt/core.h>
#include <cmath>

std::string dusl::IntObject::toString() const {
    return std::to_string(value);
}

bool dusl::IntObject::isTrue() const {
    return value != 0;
}
std::string dusl::IntObject::getTypeString() const {
    return "int";
}
dusl::FResult dusl::IntObject::add_to(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value + intObject->value;
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value + floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }

    return Object::add_to(other, token);
}

dusl::FResult dusl::IntObject::sub_by(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value - intObject->value;
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value - floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }
    return Object::sub_by(other, token);
}

dusl::FResult
dusl::IntObject::multiplied_by(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value * intObject->value;
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = value * floatObject->value;
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }
    return Object::multiplied_by(other, token);
}

dusl::FResult
dusl::IntObject::divided_by(const std::shared_ptr<Object>& other, const Token &token) {
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

dusl::FResult
dusl::IntObject::modulo_by(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        auto result = value % intObject->value;
        return FResult::createResult(std::make_shared<IntObject>(result, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        auto result = fmodl(value, floatObject->value);
        return FResult::createResult(std::make_shared<FloatObject>(result, tok), tok);
    }
    return Object::modulo_by(other, token);
}



dusl::FResult
dusl::IntObject::greater_than(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value > intObject->value, tok), tok);;
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value > floatObject->value, tok), tok);
    }

    return Object::greater_than(other, token);
}

dusl::FResult
dusl::IntObject::less_than(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value < intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value < floatObject->value, tok), tok);
    }

    return Object::less_than(other, token);
}

dusl::FResult
dusl::IntObject::greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value >= intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value >= floatObject->value, tok), tok);
    }

    return Object::greater_or_equal(other, token);
}

dusl::FResult
dusl::IntObject::less_or_equal(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value <= intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value <= floatObject->value, tok), tok);
    }

    return Object::less_or_equal(other, token);
}

dusl::FResult
dusl::IntObject::equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == floatObject->value, tok), tok);
    }

    return FResult::createResult(std::make_shared<BooleanObject>(false, tok), tok);
}

dusl::FResult
dusl::IntObject::not_equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto intObject = std::dynamic_pointer_cast<IntObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != intObject->value, tok), tok);
    } else if (auto floatObject = std::dynamic_pointer_cast<FloatObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != floatObject->value, tok), tok);
    }

    return FResult::createResult(std::make_shared<BooleanObject>(true, tok), tok);
}

dusl::FResult
dusl::IntObject::unary_plus(const Token &token) {
    return FResult::createResult(std::make_shared<IntObject>(+value, token), tok);
}

dusl::FResult
dusl::IntObject::unary_minus(const Token &token) {
    return FResult::createResult(std::make_shared<IntObject>(-value, token), tok);
}

dusl::FResult dusl::IntObject::hash(const dusl::Token &token) const {
    return FResult::createResult(std::make_shared<IntObject>(std::hash<int64_t>{}(value), token), tok);
}

dusl::FResult dusl::IntObject::to_string(IntObject &integer, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res_arg = verifyArgsCount(fn_node->args_node.args.size(), 0, integer.tok);
    if (res_arg.has_value())
        return FResult::createError(RunTimeError, res_arg.value(), fn_node->tok);

    auto res = std::make_shared<StringObject>(fmt::format("{}", integer.value), integer.tok);
    return FResult::createResult(res, fn_node->tok);
}

void dusl::IntObject::init_functions() {
    functions["toStr"] = reinterpret_cast<PropertyFunction>(IntObject::to_string);
}


