#include <utility>

#include "flang/Interpreter.hpp"

void flang::Interpreter::setContext(flang::Context _ctx) {
    this->ctx = std::move(_ctx);
}

std::shared_ptr<flang::Object> flang::Interpreter::visit(flang::ProgramNode &node) {
    std::shared_ptr<Object> val;
    for (auto& statement: node.statements) {
        val = statement->accept(*this);
    }
    return val;
}

std::shared_ptr<flang::Object> flang::Interpreter::visit(flang::BlockNode &node) {
    std::shared_ptr<Object> val;
    for (auto& statement: node.statements) {
        val = statement->accept(*this);
    }
    return val;
}

std::shared_ptr<flang::Object> flang::Interpreter::visit(const std::shared_ptr<DataNode>& node) {
    return node->accept(*this);
}

