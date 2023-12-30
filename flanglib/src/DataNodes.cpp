#include "flang/DataNodes.hpp"
#include <fmt/core.h>
#include <sstream>
#include "flang/Interpreter.hpp" // Include the full definition of Interpreter
#include "flang/ErrorType.hpp"
#include "FObject.hpp"

std::string flang::DataNode::toString() const {
    return {};
}

std::string flang::StringNode::toString() const {
    return fmt::format("StringNode(\"{}\")", value);
}

std::string flang::IntNode::toString() const {
    return fmt::format("IntNode({})", value);
}


std::string flang::FloatNode::toString() const {
    return fmt::format("FloatNode({})", value);
}

std::string flang::ListNode::toString() const {
    std::stringstream ss;
    ss << "[";
    for (auto& item: items) {
        ss << item->toString() << ", ";
    }
    ss << "]";
    return fmt::format("ListNode({})", ss.str());
}

std::string flang::VarAccessNode::toString() const {
    return fmt::format("VarAccessNode({})", tok.value);
}

std::string flang::AssignmentNode::toString() const {
    return fmt::format("AssignmentNode(\"{}\" -> {})", tok.value, expr->toString());
}

std::string flang::FunctionDefNode::toString() const {
    std::string arg_str = "[";
    for (auto& arg: args) {
        arg_str += "\t" + arg->toString() + ", ";
    }
    arg_str += "]";

    return fmt::format("FunctionDefNode({}, \nargs={}\nblock={})", tok.value ,arg_str, block->toString());
}

std::string flang::FunctionCallNode::toString() const {
    std::string arg_str = "[";
    for (auto& arg: args) {
        arg_str += "\t" + arg->toString() + ", ";
    }
    arg_str += "]";
    return fmt::format("FunctionCallNode({}, args={})", tok.value, arg_str);
}

std::string flang::BlockNode::toString() const {
    std::ostringstream oss;
    oss << "BlockNode(\n";
    for (const auto& statement: this->statements) {
        oss << "\t" + statement->toString() << "\n";
    }
    oss << ")";
    return oss.str();
}


std::string flang::BinOpNode::toString() const {
    std::string out;

    auto left_str = left_node->toString();
    auto right_str = right_node->toString();

    return fmt::format("BinOpNode({} {} {})", left_str, BinOpNode::OpToString(op), right_str);
}

std::string flang::BinOpNode::OpToString(flang::BinOpNode::Operations op) {
    switch (op) {
        case OP_PLUS:
            return "+";
        case OP_MINUS:
            return "-";
        case OP_MUL:
            return "*";
        case OP_DIV:
            return "/";
        case OP_MODULO:
            return "%";
        case OP_LESS_THAN:
            return "<";
        case OP_GREATER_THAN:
            return ">";
        case OP_LESS_OR_EQUAL:
            return "<=";
        case OP_GREATER_OR_EQUAL:
            return ">=";
        case OP_EQUAL_TO:
            return "==";
        case OP_NOT_EQUAL_TO:
            return "!=";
        default:
            return "unknown_operation";
    }
}

std::string flang::NoneNode::toString() const {
    return "none";
}

std::string flang::StructDefNode::toString() const {
    std::string values_str = "[\n";
    for (const auto& value: this->values) {

        values_str += "\t" + value->toString() + "\n";
    }
    values_str += "]";

    return fmt::format("StructDefNode({})", values_str);
}

std::string flang::MemberAccessNode::toString() const {
    return fmt::format("MemberAccessNode({} dotted by {})", left_node->toString(), right_node->toString());
}

std::shared_ptr<flang::Object> flang::ProgramNode::accept(Interpreter &visitor) {
    std::shared_ptr<Object> val;
    for (auto& statement: statements) {
        val = statement->accept(visitor);
    }
    return val;
}

std::shared_ptr<flang::Object> flang::BlockNode::accept(Interpreter &visitor) {
    std::shared_ptr<Object> val;
    for (auto& statement: statements) {
        val = statement->accept(visitor);
        if (val->isReturn()) {
            return val;
        }
    }
    return val;
}

std::shared_ptr<flang::Object> flang::StringNode::accept(Interpreter &visitor) {
    return std::make_shared<StringObject>(value, tok);
}

std::shared_ptr<flang::Object> flang::IntNode::accept(Interpreter &visitor) {
    return std::make_shared<IntObject>(value, tok);
}

std::shared_ptr<flang::Object> flang::FloatNode::accept(Interpreter &visitor) {
    return std::make_shared<FloatObject>(value, tok);
}

std::shared_ptr<flang::Object> flang::VarAccessNode::accept(Interpreter &visitor) {
    if (!visitor.ctx.currenSymbol().hasKey(tok.value)) {
        FError(NameError, fmt::format("variable '{}' is not defined", tok.value), tok.pos)
            .Throw();
    }
    return visitor.ctx.currenSymbol().getValue(tok.value);
}

std::shared_ptr<flang::Object> flang::AssignmentNode::accept(Interpreter &visitor) {
    // TODO: evaluate and check if error returned after implementing error nodes
    auto xp = expr->accept(visitor);
    visitor.ctx.currenSymbol().setValue(tok.value, xp);
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object> flang::FunctionDefNode::accept(Interpreter &visitor) {
    auto func_obj = std::make_shared<FunctionObject>(tok.value, std::move(args), std::move(block), tok);
    visitor.ctx.currenSymbol().setValue(tok.value, func_obj);
    return func_obj;
}

std::shared_ptr<flang::Object> flang::FunctionCallNode::accept(Interpreter &visitor) {
    if (!visitor.ctx.currenSymbol().hasKey(tok.value)) {
        FError(NameError, fmt::format("variable '{}' is not defined", tok.value), tok.pos)
                .Throw();
    }

    auto function = visitor.ctx.currenSymbol().getValue(tok.value);

    std::vector<std::shared_ptr<Object>> evaluated_args = {};
    evaluated_args.reserve(args.size());

    for (const auto& arg : args) {
            evaluated_args.push_back(std::move(arg->accept(visitor)));
        }

    return function->call(visitor, evaluated_args, tok);
}


std::shared_ptr<flang::Object> flang::BinOpNode::accept(Interpreter &visitor) {
    auto left = left_node->accept(visitor);
    auto right = right_node->accept(visitor);

    switch (op) {
        case OP_PLUS:
            return left->add_to(right, tok);
        case OP_MINUS:
            return left->sub_by(right, tok);
        case OP_MUL:
            return left->multiplied_by(right, tok);
        case OP_DIV:
            return left->divided_by(right, tok);
        case OP_MODULO:
            return left->modulo_by(right, tok);
        case OP_GREATER_THAN:
            return left->greater_than(right, tok);
        case OP_LESS_THAN:
            return left->less_than(right, tok);
        case OP_GREATER_OR_EQUAL:
            return left->greater_or_equal(right, tok);
        case OP_LESS_OR_EQUAL:
            return left->less_or_equal(right, tok);
        case OP_EQUAL_TO:
            return left->equal_to(right, tok);
        case OP_NOT_EQUAL_TO:
            return left->not_equal_to(right, tok);
        default:
            return std::make_shared<NoneObject>(tok);
    }
}

flang::BinOpNode::Operations flang::BinOpNode::TokToOperation(const flang::Token& token) {
    switch (token.tok) {
        case flang::TokenType::Plus:
            return OP_PLUS;
        case flang::TokenType::Minus:
            return OP_MINUS;
        case flang::TokenType::Multiply:
            return OP_MUL;
        case flang::TokenType::Divide:
            return OP_DIV;
        case flang::TokenType::Modulo:
            return OP_MODULO;
        case flang::TokenType::LessThan:
            return OP_LESS_THAN;
        case flang::TokenType::GreaterThan:
            return OP_GREATER_THAN;
        case flang::TokenType::GreaterOrEqual:
            return OP_GREATER_OR_EQUAL;
        case flang::TokenType::LessOrEqual:
            return OP_LESS_OR_EQUAL;
        case flang::TokenType::EqualTo:
            return OP_EQUAL_TO;
        case flang::TokenType::NotEqualTo:
            return OP_NOT_EQUAL_TO;
        default:
            FError(RunTimeError,
                   fmt::format("Invalid binary operation '{}'", tokToString(token.tok)),
                   token.pos
            ).Throw();
            return OP_GREATER_OR_EQUAL;
    }

}

std::shared_ptr<flang::Object> flang::NoneNode::accept(Interpreter &visitor) {
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object> flang::StructDefNode::accept(Interpreter &visitor) {
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object> flang::MemberAccessNode::accept(flang::Interpreter &visitor) {
    auto left_obj = left_node->accept(visitor);

    if (auto prop = dynamic_cast<VarAccessNode*>(right_node.get())) {
        return left_obj->getProperty(prop->tok.value);
    } else if (auto func = dynamic_cast<FunctionCallNode*>(right_node.get())) {
        return left_obj->callProperty(visitor, std::make_shared<FunctionCallNode>(*func));
    }

    FError(RunTimeError,
           fmt::format("Invalid property '{}'", right_node->toString()),
           tok.pos
    ).Throw();
    return std::make_shared<NoneObject>(tok);
}

std::shared_ptr<flang::Object> flang::ListNode::accept(flang::Interpreter &visitor) {
    std::vector<std::shared_ptr<Object>> evaluated_items;
    evaluated_items.resize(items.size());
    for (int i = 0; i < items.size(); i++) {
        evaluated_items[i] = std::move(items[i]->accept(visitor));
    }
    ListObject list(tok);
    list.items = std::move(evaluated_items);
    return std::make_shared<ListObject>(list);
}

std::string flang::IFNode::toString() const {
    std::string else_node_str = "[\n";
    for (const auto& value: this->else_ifs) {
        else_node_str += "\t" + value.toString() + "\n";
    }
    else_node_str += "]";
    return fmt::format("IFNode(\n\t\tcondition: {}\n\t\telse_ifs: {}\n\t\telse_node: {})", cond_node.toString(), else_node_str, else_node ? else_node->toString() : "");
}

std::shared_ptr<flang::Object> flang::IFNode::accept(flang::Interpreter &visitor) {
    auto cond_obj = cond_node.condition_node->accept(visitor);

    if (cond_obj->isTrue()) {
        return cond_node.body_node->accept(visitor);
    }

    for (auto& elseif: else_ifs) {
        auto elseif_cond = elseif.condition_node->accept(visitor);
        if (elseif_cond->isTrue()) {
            return elseif.body_node->accept(visitor);
        }
    }

    if (else_node) {
        return else_node->accept(visitor);
    }

    // TODO: redundant
    return std::make_shared<NoneObject>(tok);
}

std::string flang::ConditionNode::toString() const {
    return fmt::format("ConditionNode(cond: {}, body: {})", condition_node->toString(), body_node->toString());
}

std::shared_ptr<flang::Object> flang::ConditionNode::accept(flang::Interpreter &visitor) {
    return {};
}

std::string flang::LogicalOpNode::toString() const {
    return fmt::format("LogicalOpNode(left: {}, op: {}, right: {})", left_node->toString(), OpToString(op),
                       right_node->toString());
}
std::string flang::LogicalOpNode::OpToString(flang::LogicalOpNode::Operations op) {
    switch (op) {
        case OP_AND:
            return "and";
        case OP_OR:
            return "or";
        default:
            return "UnknownLogicalOP";
    }
}

std::shared_ptr<flang:: Object> flang::LogicalOpNode::accept(flang::Interpreter &visitor) {
    auto left = left_node->accept(visitor);
    auto right = right_node->accept(visitor);

    if (op == Operations::OP_AND) {
        if (left->isTrue() && right->isTrue()) {
            return std::make_shared<BooleanObject>(true, tok);
        }
        return std::make_shared<BooleanObject>(false, tok);
    } else {
        if (left->isTrue() || right->isTrue()) {
            return std::make_shared<BooleanObject>(true, tok);
        }
        return std::make_shared<BooleanObject>(false, tok);
    }
}


std::string flang::ReturnNode::toString() const {
    return DataNode::toString();
}

std::shared_ptr<flang::Object> flang::ReturnNode::accept(flang::Interpreter &visitor) {
    return std::make_shared<ReturnObject>(return_node->accept(visitor), tok);
}

std::string flang::UnaryOpNode::toString() const {
    return fmt::format("UnaryOpNode(op: {}, expr: {})", OpToString(op), right_node->toString());
}

std::string flang::UnaryOpNode::OpToString(flang::UnaryOpNode::Operations op) {
    switch (op) {
        case OP_PLUS:
            return "+";
        case OP_MINUS:
            return "-";
        case OP_NOT:
            return "not";
        default:
            return "UnknownUnaryOp";
    }
}

flang::UnaryOpNode::Operations flang::UnaryOpNode::TokToOperation(const flang::Token &token) {
    switch (token.tok) {
        case Plus:
            return OP_PLUS;
        case Minus:
            return OP_MINUS;
        case Keyword: // keyword only needs to be "not"
            return OP_NOT;
        default:
            FError(RunTimeError,
                   fmt::format("Invalid unary operation '{}'", tokToString(token.tok)),
                   token.pos
            ).Throw();
            return OP_PLUS;
    }
}

std::shared_ptr<flang::Object> flang::UnaryOpNode::accept(flang::Interpreter &visitor) {
    auto right_obj = right_node->accept(visitor);

    switch (op) {
        case OP_PLUS:
            return right_obj->unary_plus(tok);
        case OP_MINUS:
            return right_obj->unary_minus(tok);
        case OP_NOT:
            return right_obj->unary_not(tok);
    }
}

std::string flang::WhileLoopNode::toString() const {
    return fmt::format("WhileLoopNode(cond: {})", cond_node.toString());
}

std::shared_ptr<flang::Object> flang::WhileLoopNode::accept(flang::Interpreter &visitor) {
    std::shared_ptr<Object> last_resp = std::make_shared<NoneObject>(tok);
    while (cond_node.condition_node->accept(visitor)->isTrue()) {
        last_resp = cond_node.body_node->accept(visitor);
    }
    return last_resp;
}

std::string flang::DictionaryNode::toString() const {
    // TODO: Finish this
    return "DictionaryNode";
}

std::shared_ptr<flang::Object> flang::DictionaryNode::accept(flang::Interpreter &visitor) {
    std::unordered_map<size_t, std::tuple<std::shared_ptr<Object>, std::shared_ptr<Object>>> obj_items;

    for (const auto& [key, value]: items) {
        auto left = key->accept(visitor);
        auto right = value->accept(visitor);

        size_t left_hash = left->hash(key->tok);
        obj_items[left_hash] = std::make_tuple(left, right);
    }

    return std::make_shared<DictionaryObject>(obj_items, tok);
}
