#include <utility>

#include "flang/Interpreter.hpp"

void flang::Interpreter::setContext(flang::Context _ctx) {
    this->ctx = std::move(_ctx);
}

flang::FResult flang::Interpreter::visit(ProgramNode &node) {
    FResult val;
    for (auto& statement: node.statements) {
        val = statement->accept(*this);
        if (val.isError() || val.result->isReturn())
            return val;
    }
    return val;
}

flang::FResult flang::Interpreter::visit(BlockNode &node) {
    FResult val;
    for (auto& statement: node.statements) {
        val = statement->accept(*this);
        if (val.isError() || val.result->isReturn() || val.result->isBreak())
            return val;
    }
    return val;
}

flang::FResult flang::Interpreter::visit(const std::shared_ptr<DataNode>& node) {
    return node->accept(*this);
}
