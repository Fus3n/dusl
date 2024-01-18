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

    return fmt::format("FunctionDefNode({}, \nargs={}\nblock={})", func_name, arg_str, block->toString());
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

flang::FResult flang::ProgramNode::accept(Interpreter &visitor) {
    flang::FResult val;
    for (auto& statement: statements) {
        val = statement->accept(visitor);
        if (val.isError())
            return val;
    }
    return val;
}

flang::FResult flang::BlockNode::accept(Interpreter &visitor) {
    flang::FResult val;
    for (auto& statement: statements) {
        val = statement->accept(visitor);

        if (val.isError() || val.result->isBreak() || val.result->isReturn())
            return val;
    }
    return val;
}

flang::FResult flang::StringNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<StringObject>(value, tok), tok);
}

flang::FResult flang::IntNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<IntObject>(value, tok), tok);
}

flang::FResult flang::FloatNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<FloatObject>(value, tok), tok);
}

flang::FResult flang::VarAccessNode::accept(Interpreter &visitor) {
    if (!visitor.ctx.currenSymbol().hasKey(tok.value)) {
        FError(NameError, fmt::format("variable '{}' is not defined", tok.value), tok.pos)
            .Throw();
    }
    return FResult::createResult(visitor.ctx.currenSymbol().getValue(tok.value), tok);
}

flang::FResult flang::AssignmentNode::accept(Interpreter &visitor) {
    auto xp = expr->accept(visitor);
    if (xp.isError())
        return xp;

    visitor.ctx.currenSymbol().setValue(tok.value, xp.result);
    return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
}

flang::FResult flang::FunctionDefNode::accept(Interpreter &visitor) {
    auto func_obj = std::make_shared<FunctionObject>(func_name, std::move(args), std::move(block), tok, is_anon);
    if (!is_anon) {
        // push to symbol table if not an anonymous function
        visitor.ctx.currenSymbol().setValue(func_name, func_obj);
    }

    return FResult::createResult(func_obj, tok);
}

flang::FResult flang::FunctionCallNode::accept(Interpreter &visitor) {
    if (!visitor.ctx.currenSymbol().hasKey(tok.value)) {
        FError(NameError, fmt::format("variable '{}' is not defined", tok.value), tok.pos)
                .Throw();
    }

    auto function = visitor.ctx.currenSymbol().getValue(tok.value);

    std::vector<std::shared_ptr<Object>> evaluated_args = {};
    evaluated_args.reserve(args.size());

    for (const auto& arg : args) {
        auto res = arg->accept(visitor);
        if (res.isError())
            return res;
        evaluated_args.push_back(std::move(res.result));
    }

    return function->call(visitor, evaluated_args, tok);
}

flang::FResult flang::BinOpNode::accept(Interpreter &visitor) {
    auto left = left_node->accept(visitor);
    if (left.isError())
        return left;

    auto right = right_node->accept(visitor);
    if (right.isError())
        return right;

    switch (op) {
        case OP_PLUS:
            return left.result->add_to(right.result, tok);
        case OP_MINUS:
            return left.result->sub_by(right.result, tok);
        case OP_MUL:
            return left.result->multiplied_by(right.result, tok);
        case OP_DIV:
            return left.result->divided_by(right.result, tok);
        case OP_MODULO:
            return left.result->modulo_by(right.result, tok);
        case OP_GREATER_THAN:
            return left.result->greater_than(right.result, tok);
        case OP_LESS_THAN:
            return left.result->less_than(right.result, tok);
        case OP_GREATER_OR_EQUAL:
            return left.result->greater_or_equal(right.result, tok);
        case OP_LESS_OR_EQUAL:
            return left.result->less_or_equal(right.result, tok);
        case OP_EQUAL_TO:
            return left.result->equal_to(right.result, tok);
        case OP_NOT_EQUAL_TO:
            return left.result->not_equal_to(right.result, tok);
        default:
            return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
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

flang::FResult flang::NoneNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
}

flang::FResult flang::StructDefNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
}

flang::FResult flang::MemberAccessNode::accept(Interpreter &visitor) {
    auto left_obj = left_node->accept(visitor);
    if (left_obj.isError())
        return left_obj;

    if (auto prop = dynamic_cast<VarAccessNode*>(right_node.get())) {
        return left_obj.result->getProperty(prop->tok.value, prop->tok);
    } else if (auto func = dynamic_cast<FunctionCallNode*>(right_node.get())) {
        return left_obj.result->callProperty(visitor, std::make_shared<FunctionCallNode>(*func));
    }

    return FResult::createError(
        RunTimeError,
        fmt::format("Invalid property '{}'", right_node->toString()),
        tok
    );
}

flang::FResult flang::ListNode::accept(Interpreter &visitor) {
    std::vector<std::shared_ptr<Object>> evaluated_items;
    evaluated_items.resize(items.size());
    for (int i = 0; i < items.size(); i++) {
        auto item = items[i]->accept(visitor);
        if (item.isError())
            return item;
        evaluated_items[i] = std::move(item.result);
    }
    ListObject list(tok);
    list.items = std::move(evaluated_items);

    return FResult::createResult(std::make_shared<ListObject>(list), tok);
}

std::string flang::IFNode::toString() const {
    std::string else_node_str = "[\n";
    for (const auto& value: this->else_ifs) {
        else_node_str += "\t" + value.toString() + "\n";
    }
    else_node_str += "]";
    return fmt::format("IFNode(\n\t\tcondition: {}\n\t\telse_ifs: {}\n\t\telse_node: {})", cond_node.toString(), else_node_str, else_node ? else_node->toString() : "");
}

flang::FResult flang::IFNode::accept(Interpreter &visitor) {
    auto cond_obj = cond_node.condition_node->accept(visitor);
    if (cond_obj.isError())
        return cond_obj;

    if (cond_obj.result->isTrue()) {
        return cond_node.body_node->accept(visitor);
    }

    for (auto& elseif: else_ifs) {
        auto elseif_cond = elseif.condition_node->accept(visitor);
        if (elseif_cond.isError()) return elseif_cond;

        if (elseif_cond.result->isTrue()) {
            return elseif.body_node->accept(visitor);
        }
    }

    if (else_node) {
        return else_node->accept(visitor);
    }

    return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
}

std::string flang::ConditionNode::toString() const {
    return fmt::format("ConditionNode(cond: {}, body: {})", condition_node->toString(), body_node->toString());
}

flang::FResult flang::ConditionNode::accept(Interpreter &visitor) {
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

flang::FResult flang::LogicalOpNode::accept(Interpreter &visitor) {
    auto left = left_node->accept(visitor);
    if (left.isError()) return left;
    auto right = right_node->accept(visitor);
    if (right.isError()) return right;

    if (op == Operations::OP_AND) {
        if (left.result->isTrue() && right.result->isTrue()) {
            return FResult::createResult(std::make_shared<BooleanObject>(true, tok), tok);
        }
        return FResult::createResult(std::make_shared<BooleanObject>(false, tok), tok);
    } else {
        if (left.result->isTrue() || right.result->isTrue()) {
            return FResult::createResult(std::make_shared<BooleanObject>(true, tok), tok);
        }
        return FResult::createResult(std::make_shared<BooleanObject>(false, tok), tok);
    }
}


std::string flang::ReturnNode::toString() const {
    return DataNode::toString();
}

flang::FResult flang::ReturnNode::accept(Interpreter &visitor) {
    auto ret = return_node->accept(visitor);
    if (ret.isError())
        return ret;

    return FResult::createResult(std::make_shared<ReturnObject>(ret.result, tok), tok);
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

flang::FResult flang::UnaryOpNode::accept(Interpreter &visitor) {
    auto right_obj = right_node->accept(visitor);
    if (right_obj.isError())
        return right_obj;

    switch (op) {
        case OP_PLUS:
            return right_obj.result->unary_plus(tok);
        case OP_MINUS:
            return right_obj.result->unary_minus(tok);
        case OP_NOT:
            return right_obj.result->unary_not(tok);
    }
}

std::string flang::WhileLoopNode::toString() const {
    return fmt::format("WhileLoopNode(cond: {})", cond_node.toString());
}

flang::FResult flang::WhileLoopNode::accept(Interpreter &visitor) {
    FResult last_result;

    auto cond = cond_node.condition_node->accept(visitor);
    if (cond.isError())
        return cond;

    while (cond.result->isTrue()) {
        last_result = cond_node.body_node->accept(visitor);

        if (last_result.isError() || last_result.result->isBreak() || last_result.result->isReturn())
            return last_result;

        cond = cond_node.condition_node->accept(visitor);
    }
    return last_result;
}

std::string flang::DictionaryNode::toString() const {
    // TODO: Finish this
    return "DictionaryNode";
}

flang::FResult flang::DictionaryNode::accept(Interpreter &visitor) {
    std::unordered_map<size_t, std::tuple<std::shared_ptr<Object>, std::shared_ptr<Object>>> obj_items;

    for (const auto& [key, value]: items) {
        auto left = key->accept(visitor);
        if (left.isError()) return left;
        auto right = value->accept(visitor);
        if (right.isError()) return right;

        auto left_hash = left.result->hash(key->tok);
        if (left_hash.isError())
            return left_hash;

        auto intObj = dynamic_cast<IntObject*>(left_hash.result.get());

        obj_items[intObj->value] = std::make_tuple(left.result, right.result);
    }

    return FResult::createResult(std::make_shared<DictionaryObject>(obj_items, tok), tok);
}

std::string flang::FunctionCallNodeEXPR::toString() const {
    // TODO: update this
    std::stringstream ss;
    ss << "[";
    for (int i = 0; i < args.size(); i++) {
        ss << args[i]->toString();
        if (i != args.size() - 1)
            ss << ", ";
    }
    ss << "]";
    return fmt::format("FunctionCallNodeEXPR(func: {}, args: {})", functionExpr->toString(), ss.str());
}

flang::FResult flang::FunctionCallNodeEXPR::accept(Interpreter &visitor) {
    auto functionObj = functionExpr->accept(visitor);
    if (functionObj.isError())
        return functionObj;

    std::vector<std::shared_ptr<Object>> evaluated_args = {};
    evaluated_args.reserve(args.size());

    for (const auto& arg : args) {
        auto arg_obj = arg->accept(visitor);
        if (arg_obj.isError()) return arg_obj;

        evaluated_args.push_back(std::move(arg_obj.result));
    }

    return functionObj.result->call(visitor, evaluated_args, tok);
}

flang::FResult flang::IndexNode::accept(Interpreter &visitor) {
    auto left_obj = expr->accept(visitor);
    if (left_obj.isError()) return left_obj;
    auto list_obj = index_args->accept(visitor);
    if (list_obj.isError()) return list_obj;

    if (auto list = dynamic_cast<ListObject*>(list_obj.result.get())) {
        return left_obj.result->index(std::make_shared<ListObject>(*list));
    }

    return FResult::createError(
        IndexError,
        fmt::format("Invalid index '{}'", index_args->toString()),
        tok
    );
}

std::string flang::IndexNode::toString() const {
    return fmt::format("IndexNode(expr: {}, index_args: {})", expr->toString(), index_args->toString());
}

std::string flang::IndexAssignNode::toString() const {
    return DataNode::toString();
}

flang::FResult flang::IndexAssignNode::accept(Interpreter &visitor) {
    auto left_obj = left_node->accept(visitor);
    if (left_obj.isError()) return left_obj;
    auto list_obj = index_args->accept(visitor);
    if (list_obj.isError()) return list_obj;
    auto right_obj = right_node->accept(visitor);
    if (right_obj.isError()) return left_obj;

    if (auto list = dynamic_cast<ListObject*>(list_obj.result.get())) {
        return left_obj.result->index_assign(right_obj.result, std::make_shared<ListObject>(*list));
    }

    return FResult::createError(
        IndexError,
        fmt::format("Invalid index '{}'", index_args->toString()),
        tok
    );
}

std::string flang::BreakNode::toString() const {
    return "BreakNode";
}

flang::FResult flang::BreakNode::accept(flang::Interpreter &visitor) {
    return FResult::createResult(std::make_shared<BreakObject>(tok), tok);
}

std::string flang::RangeNode::toString() const {
    return fmt::format("RngNode({}:{})",  start->toString(), end->toString());
}

flang::FResult flang::RangeNode::accept(flang::Interpreter &visitor) {
    auto start_obj = start->accept(visitor);
    if (start_obj.isError())
        return start_obj;
    auto end_obj = end->accept(visitor);
    if (end_obj.isError())
        return end_obj;

    int64_t start_val = 0;
    int64_t end_val = 0;
    if (auto startInt =  dynamic_cast<IntObject*>(start_obj.result.get())) {
        if (auto endInt = dynamic_cast<IntObject*>(end_obj.result.get())) {
            start_val = startInt->value;
            end_val = endInt->value;
        } else {
            return FResult::createError(
                    RunTimeError,
                    fmt::format("Range end takes int but received {}", end_obj.result->getTypeString()),
                    tok
            );
        }
    } else {
        return FResult::createError(
                RunTimeError,
                fmt::format("Range start takes int but received {}", end_obj.result->getTypeString()),
                tok
        );
    }

    return FResult::createResult(std::make_shared<RangeObject>(start_val, end_val, tok), tok);
}

std::string flang::ForLoopNode::toString() const {
    return fmt::format("ForLoopNode(ident: {}, expr: {}, block: {})", ident, expr->toString(), block->toString());
}

flang::FResult flang::ForLoopNode::accept(flang::Interpreter &visitor) {
    auto expr_obj = expr->accept(visitor);
    if (expr_obj.isError())
        return expr_obj;

    // preserve global symbols
    auto current_ctx_name = visitor.ctx.getName();
    auto new_name = current_ctx_name + "." + tok.value;
    visitor.ctx.setName(new_name);
    visitor.ctx.enterScope();

    FResult result;
    if (auto range = dynamic_cast<RangeObject*>(expr_obj.result.get())) {
        // TODO: handle if start is greater than end
        if (range->start > range->end) {
            return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
        }
        for (int64_t index = range->start; index < range->end; index++) {
            auto ident_obj = std::make_shared<IntObject>(index, tok);
            visitor.ctx.currenSymbol().setValue(ident, ident_obj);

            result = block->accept(visitor);
            if (result.isError() || result.result->isBreak() || result.result->isReturn())
                return result;
        }
    } else if (auto list = dynamic_cast<ListObject*>(expr_obj.result.get())) {
        for (const auto& item: list->items) {
            visitor.ctx.currenSymbol().setValue(ident, item);

            result = block->accept(visitor);
            if (result.isError() || result.result->isBreak() || result.result->isReturn())
                return result;
        }
    } else if (auto str = dynamic_cast<StringObject*>(expr_obj.result.get())) {
        for (const auto& c: str->value) {
            visitor.ctx.currenSymbol().setValue(ident, std::make_shared<StringObject>(c, tok));

            result = block->accept(visitor);
            if (result.isError() || result.result->isBreak() || result.result->isReturn())
                return result;
        }
    } else {
        return FResult::createError(
                RunTimeError,
                fmt::format("'{}' is not iterable", expr->toString()),
                tok
        );
    }

    visitor.ctx.exitScope();
    visitor.ctx.setName(current_ctx_name);

    return result;
}

