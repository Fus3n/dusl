#include <sstream>
#include <utility>
#include "flang/FObject.hpp"
#include "flang/ErrorType.hpp"
#include "flang/utils/fcore.h"
#include "flang/DataNodes.hpp"

#include <fmt/core.h>


std::string flang::ListObject::toString() const {
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

bool flang::ListObject::isTrue() const {
    return !items.empty();
}

std::shared_ptr<flang::Object> flang::ListObject::splitAtDelimiter(const std::string &value, const std::string &delim, const Token& tok) {
    std::vector<std::shared_ptr<flang::Object>> items;
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

std::shared_ptr<flang::Object>
flang::ListObject::callProperty(flang::Interpreter &visitor, const std::shared_ptr<flang::FunctionCallNode> &fn_node) {
    auto func = functions.find(fn_node->tok.value);
    if (func !=  functions.end()) {
        return func->second(*this, visitor, fn_node);
    }
    std::cout << "sos \n";
    return Object::callProperty(visitor, fn_node);
}

std::shared_ptr<flang::Object> flang::ListObject::getProperty(const std::string &name) {
    if (name == "size") {
        return std::make_shared<IntObject>(items.size(), tok);
    }
    return Object::getProperty(name);
}

std::shared_ptr<flang::Object>
flang::ListObject::get(ListObject& list, flang::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {

    auto res= verifyArgsCount(fn_node->args.size(), 1, list.tok);
    if (res.has_value()) {
        FError(RunTimeError, res.value(), fn_node->tok.pos).Throw();
    }

    auto first_arg = fn_node->args[0]->accept(visitor);
    if (auto intObj = dynamic_cast<IntObject*>(first_arg.get())) {
        if (intObj->value > list.items.size() - 1) {
            FError(RunTimeError, fmt::format("Index out of range {}", intObj->value), first_arg->tok.pos).Throw();
        }
        return list.items[intObj->value];
    }
}

// TODO: Fix calling c++ function from language
std::shared_ptr<flang::Object>
flang::ListObject::push(ListObject& list, flang::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res= verifyArgsCount(fn_node->args.size(), 1, list.tok);
    if (res.has_value())
        FError(RunTimeError, res.value(), fn_node->tok.pos).Throw();
    auto val = fn_node->args[0]->accept(visitor);
    list.items.push_back(val); // getting segfault
    return std::make_shared<NoneObject>(list.tok);
}

std::shared_ptr<flang::Object>
flang::ListObject::set(ListObject& list, flang::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node) {
    auto res= verifyArgsCount(fn_node->args.size(), 2, list.tok);
    if (res.has_value())
        FError(RunTimeError, res.value(), fn_node->tok.pos).Throw();

    auto index = fn_node->args[0]->accept(visitor);
    auto val = fn_node->args[1]->accept(visitor);

    if (auto intObj = dynamic_cast<IntObject*>(index.get())) {
        if (intObj->value > list.items.size() - 1)
            FError(RunTimeError, fmt::format("Index out of range {}", intObj->value), index->tok.pos).Throw();

        list.items[intObj->value] = val;
    }

    return std::make_shared<NoneObject>(list.tok);
}

std::string flang::ListObject::getTypeString() const {
    return "list";
}

void flang::ListObject::preload_functions() {
    functions["set"] = ListObject::set;
    functions["push"] = ListObject::push;
    functions["get"] = ListObject::get;

}

flang::ListObject::ListObject(Token tok) : Object(std::move(tok)) {
    preload_functions();
}


