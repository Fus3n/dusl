#include <utility>

#include <dusl/Interpreter.hpp>

void dusl::Interpreter::setContext(dusl::Context _ctx) {
    this->ctx = std::move(_ctx);
}

dusl::FResult dusl::Interpreter::visit(ProgramNode &node) {
    FResult val = FResult::createResult(std::make_shared<NoneObject>(node.tok), node.tok);
    for (auto& statement: node.statements) {
        val = statement->accept(*this);
        if (val.isError() || val.result->isReturn())
            return val;
    }
    return val;
}

dusl::FResult dusl::Interpreter::visit(BlockNode &node) {
    FResult val = FResult::createResult(std::make_shared<NoneObject>(node.tok), node.tok);
    for (auto& statement: node.statements) {
        val = statement->accept(*this);
        if (val.isError() || val.result->isReturn() || val.result->isBreak())
            return val;
    }
    return val;
}

dusl::FResult dusl::Interpreter::visit(const std::shared_ptr<DataNode>& node) {
    return node->accept(*this);
}
