#include <sstream>
#include <utility>
#include <dusl/DObject.hpp>
#include <dusl/ErrorType.hpp>
#include <dusl/utils/dusl_core.hpp>
#include <dusl/DataNodes.hpp>

#include <fmt/core.h>


std::string dusl::ListObject::toString() const {
    std::stringstream ss;
    ss << "[";
    for (int i = 0; i < items.size(); i++) {
        ss << items[i]->toString();
        if (i != items.size() - 1)
            ss << ", ";
    }
    ss << "]";
    return ss.str();
}

bool dusl::ListObject::isTrue() const {
    return !items.empty();
}

std::shared_ptr<dusl::Object> dusl::ListObject::splitAtDelimiter(const std::string &value, const std::string &delim, const Token& tok) {
    std::vector<std::shared_ptr<dusl::Object>> items;
    std::istringstream stream(value);
    std::string character;

    int i = 0;
    size_t start = 0, end = 0;
    while ((end = value.find_first_of(delim, start)) != std::string::npos) {
        items.push_back(std::move(std::make_shared<StringObject>(value.substr(start, end - start), tok)));
        start = end + 1; // Move past the delimiter
        i++;
    }
    items.push_back(std::move(std::make_shared<StringObject>(value.substr(start), tok)));
    ListObject list(tok);
    list.items = std::move(items);
    return std::make_shared<ListObject>(list);
}

dusl::FResult dusl::ListObject::getProperty(const std::string& name, const Token& token) {
    if (name == "size") {
        return FResult::createResult(std::make_shared<IntObject>(items.size(), tok), tok);
    }
    return Object::getProperty(name, tok);
}

dusl::FResult
dusl::ListObject::get(ListObject& list, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {

    auto res= verifyArgsCount(fn_node->args_node.args.size(), 1, list.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }

    auto first_arg = fn_node->args_node.args[0]->accept(visitor);
    if (first_arg.isError())
        return first_arg;

    if (auto intObj = dynamic_cast<IntObject*>(first_arg.result.get())) {
        if (intObj->value == -1) {
            return FResult::createResult(std::make_shared<IntObject>(list.items.size() - 1, first_arg.result->tok), fn_node->tok);
        }
        if (intObj->value > list.items.size() - 1) {
            return FResult::createError(RunTimeError, fmt::format("Index out of range {}", intObj->value), first_arg.result->tok);
        }
        return FResult::createResult(list.items[intObj->value], fn_node->tok);
    }
}

// TODO: Fix calling c++ function from language
dusl::FResult
dusl::ListObject::push(ListObject& list, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res= verifyArgsCount(fn_node->args_node.args.size(), 1, list.tok);
    if (res.has_value())
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);

    auto val = fn_node->args_node.args[0]->accept(visitor);
    if (val.isError()) return val;

    list.items.push_back(std::move(val.result)); // getting segfault
    return FResult::createResult(std::make_shared<NoneObject>(list.tok), fn_node->tok);
}

dusl::FResult
dusl::ListObject::set(ListObject& list, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res= verifyArgsCount(fn_node->args_node.args.size(), 2, list.tok);
    if (res.has_value())
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);

    auto index = fn_node->args_node.args[0]->accept(visitor);
    if (index.isError()) return index;
    auto val = fn_node->args_node.args[1]->accept(visitor);
    if (val.isError()) return val;

    if (auto intObj = dynamic_cast<IntObject*>(index.result.get())) {
        if (intObj->value > list.items.size() - 1)
            return FResult::createError(IndexError, fmt::format("Index out of range {}", intObj->value), index.result->tok);

        list.items[intObj->value] = std::move(val.result);
    }

    return FResult::createResult(std::make_shared<NoneObject>(list.tok), fn_node->tok);
}

std::string dusl::ListObject::getTypeString() const {
    return "list";
}

dusl::FResult dusl::ListObject::index(std::shared_ptr<ListObject> idx_args) {
    if (idx_args->items.size() != 1) {
        return FResult::createError(
            IndexError,
            fmt::format("list index takes 1 argument but {} were given", idx_args->items.size()),
            tok
        );
    }

    auto first_arg = idx_args->items[0];
    if (auto intObj = dynamic_cast<IntObject*>(first_arg.get())) {
        if (intObj->value > items.size() - 1) {
            return FResult::createError(
                    IndexError,
                    fmt::format("Index out of range {}", intObj->value),
                first_arg->tok
            );
        }
        return FResult::createResult(items[intObj->value], tok);
    }

    return Object::index(idx_args);
}

dusl::FResult
dusl::ListObject::index_assign(std::shared_ptr<Object> &right, std::shared_ptr<ListObject> idx_args) {
    if (idx_args->items.size() != 1) {
        return FResult::createError(
                IndexError,
                fmt::format("list index takes 1 argument but {} were given", idx_args->items.size()),
                tok
        );
    }

    auto first_arg = idx_args->items[0];
    if (auto intObj = dynamic_cast<IntObject*>(first_arg.get())) {
        if (intObj->value > items.size() - 1) {
            return FResult::createError(
                    IndexError,
                    fmt::format("Index out of range {}", intObj->value),
                    first_arg->tok
            );
        }
        items[intObj->value] = right;
        return FResult::createResult(right, tok);
    }

    return Object::index_assign(right, idx_args);
}

dusl::FResult dusl::ListObject::for_each(ListObject &list, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res= verifyArgsCount(fn_node->args_node.args.size(), 1, list.tok);
    if (res.has_value()) {
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);
    }

    auto callback_obj = fn_node->args_node.args[0]->accept(visitor);
    if (callback_obj.isError()) return callback_obj;

    FResult last;
    for (auto & item : list.items) {
        std::vector<std::shared_ptr<Object>> args { item };
        auto argObj = ArgumentObject(args, item->tok);

        last = callback_obj.result->call(visitor, argObj, item->tok);
        if (last.isError())
            return last;
        else if (last.result->isReturn())
            break;
    }

    return last;
}

dusl::FResult dusl::ListObject::pop_back(ListObject &list, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res= verifyArgsCount(fn_node->args_node.args.size(), 0, list.tok);
    if (res.has_value())
        return FResult::createError(RunTimeError, res.value(), fn_node->tok);

    if (!list.items.empty()) {
        auto r = list.items.back();
        list.items.pop_back();
        return FResult::createResult(r, fn_node->tok);
    }

    return FResult::createError(IndexError, "Cannot pop from empty list", fn_node->tok);
}

