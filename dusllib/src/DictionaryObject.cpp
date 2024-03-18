#include <DObject.hpp>
#include <sstream>
#include <utils/dusl_core.hpp>

std::string dusl::DictionaryObject::toString() const {
    std::stringstream ss;
    ss << "{";
    for (auto& item: items) {
        ss << std::get<0>(item.second)->toString();
        ss << ": ";
        ss << std::get<1>(item.second)->toString();
        ss << ", ";
    }
    ss << "}";
    return ss.str();
}

std::string dusl::DictionaryObject::getTypeString() const {
    return "dict";
}

dusl::FResult dusl::DictionaryObject::index(std::shared_ptr<ListObject> idx_args, const std::optional<Token> token) {
    if (idx_args->items.size() != 1) {
        return FResult::createError(
            IndexError,
            fmt::format("dict index takes 1 argument but {} were given", idx_args->items.size()),
            tok);
    }

    auto first_arg = idx_args->items[0];
    auto hashed = first_arg->hash(tok);
    if (hashed.isError())
        return hashed;

    const auto key_object = dynamic_cast<IntObject*>(hashed.result.get()); // TODO: might cause issue?

    auto it = items.find(key_object->value);
    if (it == items.end()) {
        return FResult::createError(
                NameError,
                fmt::format("key {} not found in dict", first_arg->toString()),
                first_arg->tok);
    }

    return FResult::createResult(std::get<1>(it->second), tok);
}

dusl::FResult
dusl::DictionaryObject::index_assign(std::shared_ptr<Object> &right, std::shared_ptr<ListObject> idx_args) {
    if (idx_args->items.size() != 1) {
        return FResult::createError(
            IndexError,
            fmt::format("dict index takes 1 argument but {} were given", idx_args->items.size()),
            tok);
    }
    auto first_arg = idx_args->items[0];
    auto hashed = first_arg->hash(tok);
    if (hashed.isError())
        return hashed;

    const auto key_object = dynamic_cast<IntObject*>(hashed.result.get()); // TODO: might cause issue?
    items[key_object->value] = std::make_tuple(first_arg, right);
    return FResult::createResult(right, tok);
}

dusl::FResult dusl::DictionaryObject::getProperty(const std::string &name, const Token &tok) {
    if (name == "size") {
        return FResult::createResult(std::make_shared<IntObject>(items.size(), tok), tok);
    }
    return Object::getProperty(name, tok);
}

dusl::FResult dusl::DictionaryObject::exists(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    if (const auto res = verifyArgsCount(fn_node->args_node.args.size(), 1, dict.tok); res.has_value()) {
        return FResult::createError(
            RunTimeError,
            res.value(),
            fn_node->tok);
    }

    auto first_arg = fn_node->args_node.args[0]->accept(visitor);
    if (first_arg.isError()) return first_arg;

    auto hashed = first_arg.result->hash(dict.tok);
    if (hashed.isError())
        return hashed;

    const auto key_object = dynamic_cast<IntObject*>(hashed.result.get());
    if (dict.items.find(key_object->value) == dict.items.end()) {
        return FResult::createResult(std::make_shared<BooleanObject>(false, dict.tok), dict.tok);
    }

    return FResult::createResult(std::make_shared<BooleanObject>(true, dict.tok), dict.tok);
}

dusl::FResult dusl::DictionaryObject::get(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    if (const auto res = verifyArgsCount(fn_node->args_node.args.size(), 1, dict.tok); res.has_value()) {
        return FResult::createError(
            RunTimeError,
            res.value(),
            fn_node->tok);
    }

    auto first_arg = fn_node->args_node.args[0]->accept(visitor);
    if (first_arg.isError()) return first_arg;

    auto hashed = first_arg.result->hash(dict.tok);
    if (hashed.isError())
        return hashed;

    const auto intObject = dynamic_cast<IntObject*>(hashed.result.get());

    const auto it = dict.items.find(intObject->value);
    if (it == dict.items.end()) {
        return FResult::createResult(std::make_shared<NoneObject>(dict.tok), dict.tok); // TODO: return a proper "None" type
    }

    return FResult::createResult(std::get<1>(it->second), dict.tok);
}

dusl::FResult dusl::DictionaryObject::keys(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    if (const auto res = verifyArgsCount(fn_node->args_node.args.size(), 0, dict.tok); res.has_value()) {
        return FResult::createError(
            RunTimeError,
            res.value(),
            fn_node->tok);
    }

    const auto list = std::make_shared<ListObject>(dict.tok);
    list->items.reserve(dict.items.size());
    for (auto& item: dict.items) {
        list->items.push_back(std::get<0>(item.second));
    }
    return FResult::createResult(list, dict.tok);
}

dusl::FResult dusl::DictionaryObject::values(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    if (const auto res = verifyArgsCount(fn_node->args_node.args.size(), 0, dict.tok); res.has_value()) {
        return FResult::createError(
                RunTimeError,
                res.value(),
                fn_node->tok);
    }

    const auto list = std::make_shared<ListObject>(dict.tok);
    list->items.reserve(dict.items.size());
    for (auto& item: dict.items) {
        list->items.push_back(std::get<1>(item.second));
    }

    return FResult::createResult(list, dict.tok);
}
