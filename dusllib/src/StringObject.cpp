#include <dusl/DObject.hpp>
#include <fmt/core.h>
#include <dusl/utils/dusl_core.hpp>
#include <cctype>

#include <functional>

std::string dusl::StringObject::toString() const {
    return value;
}

bool dusl::StringObject::isTrue() const {
    return !value.empty();
}

std::string dusl::StringObject::getTypeString() const {
    return "string";
}

dusl::FResult dusl::StringObject::getProperty(const std::string& name, const Token& token) {
    if (name == "size") {
        return FResult::createResult(std::make_shared<IntObject>(value.length(), tok), tok);
    }
    return Object::getProperty(name, tok);
}

dusl::FResult
dusl::StringObject::add_to(const std::shared_ptr<Object>& other, const Token &token) {
    if (auto stringObject = std::dynamic_pointer_cast<StringObject>(other)) {
        auto result = value + stringObject->value;
        return FResult::createResult(std::make_shared<StringObject>(result, tok), tok);
    }
    return Object::add_to(other, token);
}

dusl::FResult
dusl::StringObject::equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto strObj = std::dynamic_pointer_cast<StringObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value == strObj->value, tok), tok);
    }
    return FResult::createResult(std::make_shared<BooleanObject>(false, tok), tok);
}

dusl::FResult
dusl::StringObject::not_equal_to(const std::shared_ptr<Object> &other, const Token &token) {
    if (auto strObj = std::dynamic_pointer_cast<StringObject>(other)) {
        return FResult::createResult(std::make_shared<BooleanObject>(value != strObj->value, tok), tok);
    }
    return FResult::createResult(std::make_shared<BooleanObject>(true, tok), tok);
}

dusl::FResult dusl::StringObject::hash(const dusl::Token &token) const {
    return FResult::createResult(std::make_shared<IntObject>(std::hash<std::string>{}(value), token), tok);
}

dusl::FResult dusl::StringObject::index(std::shared_ptr<ListObject> idx_args) {
    if (idx_args->items.size() != 1) {
        return FResult::createError(
            IndexError,
            fmt::format("list index takes 1 argument but {} were given", idx_args->items.size()),
            tok
        );
    }

    auto first_arg = idx_args->items[0];

    if (auto intObj = dynamic_cast<IntObject*>(first_arg.get())) {
        if (intObj->value > value.size() - 1) {
            return FResult::createError(
                IndexError,
                fmt::format("index out of range {}", intObj->value),
                intObj->tok
            );
        }
        return FResult::createResult(std::make_shared<StringObject>(value[intObj->value], tok), tok);
    } else if (auto rangeObj = dynamic_cast<RangeObject*>(first_arg.get())) {
        if ((rangeObj->start  > value.size() - 1) || (rangeObj->end > value.size() - 1) || (rangeObj->start > rangeObj->end)) {
            return FResult::createError(
                IndexError,
                fmt::format("Index out of range {}", rangeObj->start),
                rangeObj->tok
            );
        }
        std::string result = value.substr(rangeObj->start, rangeObj->end - rangeObj->start + 1);
        return FResult::createResult(std::make_shared<StringObject>(result, tok), tok);
    }

    return Object::index(idx_args);
}

dusl::FResult dusl::StringObject::get(StringObject &str, dusl::Interpreter &visitor,
                                        const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 1, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }

    auto arg = fn_node->args_node.args[0]->accept(visitor);
    if (arg.isError()) return arg;

    if (auto intObj = std::dynamic_pointer_cast<IntObject>(arg.result)) {
        if (intObj->value > str.value.length()) {
            return FResult::createError(IndexError, fmt::format("{} index out of range {}", str.getTypeString(), intObj->value), fn_node->tok);
        }
        return FResult::createResult(std::make_shared<StringObject>(str.value[intObj->value], str.tok), fn_node->tok);
    }

    return FResult::createError(RunTimeError, "'get' expects an INT as an argument", fn_node->tok);
}

dusl::FResult dusl::StringObject::split(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 1, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError,
                                    res.value(),
                                    fn_node->tok
        );
    }
    auto first_arg = fn_node->args_node.args[0]->accept(visitor);
    if (first_arg.isError()) return first_arg;

    if (auto strObj = dynamic_cast<StringObject*>(first_arg.result.get())) {
        return FResult::createResult(ListObject::splitAtDelimiter(str.value, strObj->value, str.tok), fn_node->tok);
    }
}

dusl::FResult dusl::StringObject::to_int(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }

    try {
        auto str_value = std::stoi(str.value);
        return FResult::createResult(std::make_shared<IntObject>(str_value, str.tok), fn_node->tok);
    } catch (std::exception &e) {
        return FResult::createError(RunTimeError, fmt::format("Invalid integer {}", str.value), fn_node->tok);
    }
}

dusl::FResult dusl::StringObject::to_float(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    try {
        auto str_value = std::stold(str.value);
        return FResult::createResult(std::make_shared<FloatObject>(str_value, str.tok), fn_node->tok);
    } catch (std::exception &e) {
        return FResult::createError(RunTimeError, fmt::format("Invalid Float {}", str.value), fn_node->tok);
    }
}

void dusl::StringObject::init_functions() {
    functions["split"] = reinterpret_cast<PropertyFunction>(StringObject::split);
    functions["get"] = reinterpret_cast<PropertyFunction>(StringObject::get);
    functions["toInt"] = reinterpret_cast<PropertyFunction>(StringObject::to_int);
    functions["toFloat"] = reinterpret_cast<PropertyFunction>(StringObject::to_float);
    functions["toLower"] = reinterpret_cast<PropertyFunction>(StringObject::to_lower);
    functions["toUpper"] = reinterpret_cast<PropertyFunction>(StringObject::to_upper);
    functions["isDigit"] = reinterpret_cast<PropertyFunction>(StringObject::is_digit);
    functions["isAlpha"] = reinterpret_cast<PropertyFunction>(StringObject::is_alpha);
    functions["replace"] = reinterpret_cast<PropertyFunction>(StringObject::replace);
    functions["startsWith"] = reinterpret_cast<PropertyFunction>(StringObject::starts_with);
    functions["endsWith"] = reinterpret_cast<PropertyFunction>(StringObject::ends_with);
}
std::string toLowerStr(std::string& s) {
    for(char &c : s)
        c = std::tolower(c);
    return s;
}

std::string toUpperstr(std::string& s) {
    for(char &c : s)
        c = std::toupper(c);
    return s;
}
dusl::FResult dusl::StringObject::to_lower(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    return FResult::createResult(std::make_shared<StringObject>(toLowerStr(str.value), str.tok), fn_node->tok);
}

dusl::FResult dusl::StringObject::to_upper(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    return FResult::createResult(std::make_shared<StringObject>(toUpperstr(str.value), str.tok), fn_node->tok);
}

dusl::FResult dusl::StringObject::is_digit(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    for (auto c : str.value) {
        if (!std::isdigit(c)) {
            return FResult::createResult(std::make_shared<BooleanObject>(false, str.tok), fn_node->tok);
        }
    }
    return FResult::createResult(std::make_shared<BooleanObject>(true, str.tok), fn_node->tok);
}

dusl::FResult dusl::StringObject::is_alpha(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 0, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }
    for (auto c : str.value) {
        if (!std::isalpha(c)) {
            return FResult::createResult(std::make_shared<BooleanObject>(false, str.tok), fn_node->tok);
        }
    }
    return FResult::createResult(std::make_shared<BooleanObject>(true, str.tok), fn_node->tok);
}

dusl::FResult dusl::StringObject::replace(dusl::StringObject &str, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 2, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }

    auto first_arg = fn_node->args_node.args[0]->accept(visitor);
    if (first_arg.isError()) return first_arg;
    auto second_arg = fn_node->args_node.args[1]->accept(visitor);
    if (second_arg.isError()) return second_arg;

    if (auto to_replace = dynamic_cast<StringObject*>(first_arg.result.get())) {
        if (auto replace_with = dynamic_cast<StringObject*>(second_arg.result.get())) {
            std::string result = str.value;
            std::size_t found = result.find(to_replace->value);
            while (found != std::string::npos) {
                result.replace(found, to_replace->value.length(), replace_with->value);
                found = result.find(to_replace->value, found + replace_with->value.length());
            }
            return FResult::createResult(std::make_shared<StringObject>(result, str.tok), fn_node->tok);
        }
    }

    return FResult::createError(TypeError, "replace expects two strings", fn_node->tok);
}

dusl::FResult dusl::StringObject::starts_with(dusl::StringObject &str, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 1, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }

    auto first_arg = fn_node->args_node.args[0]->accept(visitor);
    if (first_arg.isError()) return first_arg;
    if (auto to_check = dynamic_cast<StringObject*>(first_arg.result.get())) {
        if (str.value.size() < to_check->value.size()) {
            return FResult::createResult(std::make_shared<BooleanObject>(false, str.tok), fn_node->tok);
        }
        auto subs = str.value.substr(0, to_check->value.size());
        if (subs != to_check->value) {
            return FResult::createResult(std::make_shared<BooleanObject>(false, str.tok), fn_node->tok);
        }

        return FResult::createResult(std::make_shared<BooleanObject>(true, str.tok), fn_node->tok);
    }

    return FResult::createError(TypeError, "startsWith expects a string", fn_node->tok);
}

dusl::FResult dusl::StringObject::ends_with(dusl::StringObject &str, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res = verifyArgsCount(fn_node->args_node.args.size(), 1, str.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }

    auto first_arg = fn_node->args_node.args[0]->accept(visitor);
    if (first_arg.isError()) return first_arg;
    if (auto to_check = dynamic_cast<StringObject*>(first_arg.result.get())) {
        if (str.value.size() < to_check->value.size()) {
            return FResult::createResult(std::make_shared<BooleanObject>(false, str.tok), fn_node->tok);
        }
        auto subs = str.value.substr(str.value.size() - to_check->value.size());
        if (subs != to_check->value) {
            return FResult::createResult(std::make_shared<BooleanObject>(false, str.tok), fn_node->tok);
        }

        return FResult::createResult(std::make_shared<BooleanObject>(true, str.tok), fn_node->tok);
    }

    return FResult::createError(TypeError, "endsWith expects a string", fn_node->tok);
}

