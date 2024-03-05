#include "flang/DataNodes.hpp"
#include <fmt/core.h>
#include <sstream>
#include "flang/Interpreter.hpp" // Include the full definition of Interpreter
#include "flang/ErrorType.hpp"
#include "FObject.hpp"
#include "flang/utils/common.h"
#include "Parser.hpp"
#include "flang/utils/fstandard.hpp"
#include <filesystem>

std::string flang::DataNode::toString() const {
    return {};
}

nlohmann::ordered_json flang::DataNode::toJson() const {
    return {
        {"tok", tok.toJson()},
    };
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
    return fmt::format("FunctionDefNode({}, \nargs={}\nblock={})", func_name, "<TODO>", block->toString());
}

std::string flang::FunctionCallNode::toString() const {
    return fmt::format("FunctionCallNode({}, args={})", tok.value, args_node.toString());
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

nlohmann::ordered_json flang::ProgramNode::toJson() const {
    nlohmann::ordered_json json;
    json["type"] = "ProgramNode";
    json["statements"] = nlohmann::ordered_json::array();
    for (auto& statement: statements) {
        json["statements"].push_back(statement->toJson());
    }
    return json;
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

nlohmann::ordered_json flang::BlockNode::toJson() const {
    nlohmann::ordered_json json;
    json["type"] = "BlockNode";
    json["statements"] = nlohmann::ordered_json::array();
    for (auto& statement: statements) {
        json["statements"].push_back(statement->toJson());
    }
    return json;
}

flang::FResult flang::StringNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<StringObject>(value, tok), tok);
}

nlohmann::ordered_json flang::StringNode::toJson() const {
    return {
        {"type","StringNode"},
        {"value",value},
    };
}

flang::FResult flang::IntNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<IntObject>(value, tok), tok);
}

nlohmann::ordered_json flang::IntNode::toJson() const {
    return {
        {"type","IntNode"},
        {"value",value},
    };
}

flang::FResult flang::FloatNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<FloatObject>(value, tok), tok);
}

nlohmann::ordered_json flang::FloatNode::toJson() const {
    return {
        {"type","FloatNode"},
        {"value",value},
    };
}

flang::FResult flang::VarAccessNode::accept(Interpreter &visitor) {
    if (!visitor.ctx.currentSymbol().hasKey(tok.value)) {
        return FResult::createError(
            NameError,
            fmt::format("variable '{}' is not defined", tok.value),
            tok
        );
    }

    return FResult::createResult(visitor.ctx.currentSymbol().getValue(tok.value), tok);
}

nlohmann::ordered_json flang::VarAccessNode::toJson() const {
    return {
        {"type", "VarAccessNode"},
        {"value", tok.value}
    };
}

flang::FResult flang::AssignmentNode::accept(Interpreter &visitor) {
    auto xp = expr->accept(visitor);
    if (xp.isError())
        return xp;

    visitor.ctx.currentSymbol().setValue(tok.value, xp.result);
    return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
}

nlohmann::ordered_json flang::AssignmentNode::toJson() const {
    return {
        {"type", "AssignmentNode"},
        {"key", tok.value},
        {"value", expr->toJson()}
    };
}

flang::FResult flang::FunctionDefNode::accept(Interpreter &visitor) {
    std::vector<std::string> args;
    std::unordered_map<std::string, std::shared_ptr<flang::Object>> default_args;

    args.reserve(args_node.args.size());
    default_args.reserve(args_node.default_args.size());


    for (auto & arg : args_node.args) {
        args.push_back(arg->tok.value);
    }

    for (auto& [key, value]: args_node.default_args) {
        auto arg_val = value->accept(visitor);
        if (arg_val.isError()) return arg_val;
        default_args[key] = arg_val.result;
    }

    auto func_obj = std::make_shared<FunctionObject>(func_name, args, default_args, std::move(block), tok, is_anon);
    if (!is_anon) {
        // push to symbol table if not an anonymous function
        visitor.ctx.currentSymbol().setValue(func_name, func_obj);
    }

    return FResult::createResult(func_obj, tok);
}

nlohmann::ordered_json flang::FunctionDefNode::toJson() const {
    return {
        {"type", "FunctionDefNode"},
        {"args", args_node.toJson()},
        {"block", block->toJson()}
    };
}

flang::FResult flang::FunctionCallNode::accept(Interpreter &visitor) {
    if (!visitor.ctx.currentSymbol().hasKey(tok.value)) {
        return FResult::createError(
            NameError,
            fmt::format("variable '{}' is not defined", tok.value),
            tok
        );
    }

    auto function = visitor.ctx.currentSymbol().getValue(tok.value);

    auto args_result = args_node.accept(visitor);
    if (args_result.isError())
        return args_result;

    ArgumentObject arg_obj_new = *dynamic_cast<ArgumentObject*>(args_result.result.get());
    return function->call(visitor, arg_obj_new, tok);
}

nlohmann::ordered_json flang::FunctionCallNode::toJson() const {
    return {
        {"type", "FunctionCallNode"},
        {"args", args_node.toJson()}
    };
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

nlohmann::ordered_json flang::BinOpNode::toJson() const {
    return {
        {"type", "BinOpNode"},
        {"left", left_node->toJson()},
        {"op", BinOpNode::OpToString(op)},
        {"right", right_node->toJson()}
    };
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

nlohmann::ordered_json flang::NoneNode::toJson() const {
    return {
        {"type", "NoneNode"}
    };
}

flang::FResult flang::StructDefNode::accept(Interpreter &visitor) {
    std::unordered_map<std::string, std::shared_ptr<Object>> properties;

    // evaluate the values
    for (auto& val: values) {
        if (const auto assingNode = dynamic_cast<AssignmentNode*>(val.get())) {
            auto prop_res = assingNode->expr->accept(visitor);
            if (prop_res.isError()) return prop_res;
            properties[assingNode->tok.value] = prop_res.result;
        } else if (const auto funcNode = dynamic_cast<FunctionDefNode*>(val.get())) {
            auto prop_res = funcNode->accept(visitor);
            if (prop_res.isError()) return prop_res;
            properties[funcNode->func_name] = prop_res.result;
        }
        // TODO: handle other cases...
    }
    auto structObj = std::make_shared<StructObject>(tok.value, properties, tok);
    visitor.ctx.currentSymbol().setValue(tok.value, structObj);
    return FResult::createResult(structObj, tok);
}

nlohmann::ordered_json flang::StructDefNode::toJson() const {
    nlohmann::ordered_json json;
    json["type"] = "StructDefNode";
    json["properties"] = nlohmann::ordered_json::array();
    for (auto& prop: values) {
        json["properties"].push_back(prop->toJson());
    }
    return json;
}

flang::FResult flang::MemberAccessNode::accept(Interpreter &visitor) {
    auto left_obj = left_node->accept(visitor);
    if (left_obj.isError())
        return left_obj;

    if (auto prop = dynamic_cast<VarAccessNode*>(right_node.get())) {
        return left_obj.result->getProperty(prop->tok.value, prop->tok);
    } else if (auto func = dynamic_cast<FunctionCallNode*>(right_node.get())) {
        if (auto leftStruct = dynamic_cast<StructInstanceObject*>(left_obj.result.get())) {
            // add left as argument as first value
            func->args_node.args.insert(func->args_node.args.begin(), left_node);
        }
        return left_obj.result->callProperty(visitor, std::make_shared<FunctionCallNode>(*func));
    }

    return FResult::createError(
        RunTimeError,
        fmt::format("Invalid property '{}'", right_node->toString()),
        tok
    );
}

nlohmann::ordered_json flang::MemberAccessNode::toJson() const {
    return {
        {"type", "MemberAccessNode"},
        {"left", left_node->toJson()},
        {"right", right_node->toJson()}
    };
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

nlohmann::ordered_json flang::ListNode::toJson() const {
    nlohmann::ordered_json j;
    j["type"] = "ListNode";
    j["items"] = nlohmann::ordered_json::array();
    for (auto& item: items) {
        j["items"].push_back(item->toJson());
    }
    return j;
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

nlohmann::ordered_json flang::IFNode::toJson() const {
    nlohmann::ordered_json j = {
        {"type", "IFNode"},
        {"base_condition", cond_node.toJson()}
    };

    j["else_ifs"] = nlohmann::ordered_json::array();
    for (const auto& elif: else_ifs) {
        j["else_ifs"].push_back(elif.toJson());
    }

    return j;
}

std::string flang::ConditionNode::toString() const {
    return fmt::format("ConditionNode(cond: {}, body: {})", condition_node->toString(), body_node->toString());
}

flang::FResult flang::ConditionNode::accept(Interpreter &visitor) {
    return {};
}

nlohmann::ordered_json flang::ConditionNode::toJson() const {
    return {
        {"type", "ConditionNode"},
        {"condition", condition_node->toJson()},
        {"body", body_node->toJson()}
    };
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

nlohmann::ordered_json flang::LogicalOpNode::toJson() const {
    return {
        {"type", "LogicalOpNode"},
        {"left", left_node->toJson()},
        {"op", OpToString(op)},
        {"right", right_node->toJson()}
    };
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

nlohmann::ordered_json flang::ReturnNode::toJson() const {
    return {
        {"type", "ReturnNode"},
        {"node_to_return", return_node->toJson()}
    };
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

nlohmann::ordered_json flang::UnaryOpNode::toJson() const {
    return {
        {"type","UnaryOpNode"},
        {"op", OpToString(op)},
        {"expr", right_node->toJson()}
    };
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

nlohmann::ordered_json flang::WhileLoopNode::toJson() const {
    return {
        {"type", "WhileLoopNode"},
        {"condition", cond_node.toJson()}
    };
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

nlohmann::ordered_json flang::DictionaryNode::toJson() const {
    nlohmann::ordered_json j;
    j["type"] = "DictionaryNode";
    j["items"] = nlohmann::ordered_json::array();

    for (auto& [key, value]: items) {
        nlohmann::ordered_json subj = {
            {key->toJson(), value->toJson()}
        };
        j["items"].push_back(subj);
    }
    return j;
}

std::string flang::FunctionCallNodeEXPR::toString() const {
    return fmt::format("FunctionCallNodeEXPR(func: {}, args: {})", function_expr->toString(), args_node.toString());
}

flang::FResult flang::FunctionCallNodeEXPR::accept(Interpreter &visitor) {
    auto functionObj = function_expr->accept(visitor);
    if (functionObj.isError())
        return functionObj;

    auto args_result = args_node.accept(visitor);
    if (args_result.isError())
        return args_result;

    ArgumentObject arg_obj_new = *dynamic_cast<ArgumentObject*>(args_result.result.get());

    return functionObj.result->call(visitor, arg_obj_new, tok);
}

nlohmann::ordered_json flang::FunctionCallNodeEXPR::toJson() const {
    return {
        {"type", "FunctionCallNodeEXPR"},
        {"args", args_node.toJson()},
        {"function_expr", function_expr->toJson()}
    };
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

nlohmann::ordered_json flang::IndexNode::toJson() const {
    return {
        {"type", "IndexNode"},
        {"left", expr->toJson()},
        {"index_args", index_args->toJson()}
    };
}

std::string flang::IndexAssignNode::toString() const {
    return DataNode::toString();
}

nlohmann::ordered_json flang::IndexAssignNode::toJson() const {
    return {
        {"type", "IndexAssignNode"},
        {"left", left_node->toJson()},
        {"right", right_node->toJson()},
        {"index_args", index_args->toJson()}
    };
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

nlohmann::ordered_json flang::BreakNode::toJson() const {
    return {
        {"type", "BreakNode"}
    };
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

nlohmann::ordered_json flang::RangeNode::toJson() const {
    return {
        {"type", "RangeNode"},
        {"start", start->toJson()},
        {"end", end->toJson()}
    };
}

std::string flang::ForLoopNode::toString() const {
    return fmt::format("ForLoopNode(ident: {}, expr: {}, block: {})", ident, expr->toString(), block->toString());
}

nlohmann::ordered_json flang::ForLoopNode::toJson() const {
    return {
        {"type", "ForLoopNode"},
        {"expr", expr->toJson()},
        {"block", block->toJson()}
    };
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
            visitor.ctx.currentSymbol().setValue(ident, ident_obj);

            result = block->accept(visitor);
            if (result.isError() || result.result->isBreak() || result.result->isReturn())
                return result;
        }
    } else if (auto list = dynamic_cast<ListObject*>(expr_obj.result.get())) {
        for (const auto& item: list->items) {
            visitor.ctx.currentSymbol().setValue(ident, item);

            result = block->accept(visitor);
            if (result.isError() || result.result->isBreak() || result.result->isReturn())
                return result;
        }
    } else if (auto str = dynamic_cast<StringObject*>(expr_obj.result.get())) {
        for (const auto& c: str->value) {
            visitor.ctx.currentSymbol().setValue(ident, std::make_shared<StringObject>(c, tok));

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

std::string flang::ImportNode::toString() const {
    std::stringstream ss;
    ss << "ImportNode(symbols: [";
    for (const auto& ident: symbols) {
        ss << ident << ", ";
    }
    ss << "])";
    return ss.str();
}

nlohmann::ordered_json flang::ImportNode::toJson() const {
    nlohmann::ordered_json j = {
        {"type", "ImportNode"},
        {"import_all", import_all}
    };

    j["symbols"] = nlohmann::ordered_json::array();
    for (auto& syms: symbols) {
        j["symbols"].push_back(syms);
    }
    return j;
}

flang::FResult flang::ImportNode::accept(flang::Interpreter &visitor) {
    flang::Lexer lexer;
    flang::Parser parser;
    std::string code;

    std::filesystem::path path;

    // check if string starts with "std:"
    if (module_path.size() > 4 && module_path.substr(0, 4) == "std:") {
        auto base = module_path.substr(4);
        path = std::filesystem::path(FLANG_STD_PATH) / base;
    } else {
        path = std::filesystem::path(module_path);
    }

    if (!path.has_extension()) {
        path = path.concat(".flin"); // TODO: file extension
    }

    auto file_context_name = path.stem().string();
    // check if trying to import same file
    if (file_context_name == visitor.ctx.getBaseCtxName()) { // TODO: checks failing
        return FResult::createError(
            ImportError,
            fmt::format("Cannot import same file '{}'", module_path),
            tok
        );
    }

    try {
        code = flang::read_file(path.string());
    } catch (std::exception &e) {
        fmt::println("{}, FILE: {}", e.what(), path.string());
        return FResult::createError(
            ImportError,
            fmt::format("Could not import module '{}'", path.string()),
            tok
        );
    }

    flang::str_trim(code);
    if (code.empty()) {
        return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
    }

    auto tokens = lexer.tokenize(code, module_path);
    auto ast = parser.parse(code, module_path, tokens);

    SymbolTable base_table = visitor.ctx.currentSymbol();

    auto current_ctx_name = visitor.ctx.getName();
    auto new_name = current_ctx_name + "." + file_context_name;
    visitor.ctx.setName(new_name);
    visitor.ctx.setBaseCtxName(file_context_name);
    visitor.ctx.enterScope();

    auto result_val = visitor.visit(ast);
    if (result_val.isError())
        return result_val;


    // if no specific symbols are specified merge all symbols
    if (import_all) {
        auto current_symbols = visitor.ctx.currentSymbol();
        visitor.ctx.setName(current_ctx_name);
        visitor.ctx.exitScope();
        visitor.ctx.setCurrentSymbol(current_symbols);
        return result_val;
    }

    for (auto & sym : symbols) {
        if (visitor.ctx.currentSymbol().hasKey(sym)) {
            base_table.setValue(sym, visitor.ctx.currentSymbol().getValue(sym));
            continue;
        }

        return FResult::createError(
            ImportError,
            fmt::format("module '{}' doesn't contain symbol '{}'", module_path, sym),
            tok
        );
    }
    visitor.ctx.setName(current_ctx_name);
    visitor.ctx.exitScope();
    visitor.ctx.setCurrentSymbol(base_table);

    return result_val;
}

std::string flang::ArgumentNode::toString() const {
    return "ArgumentNode([])"; // TODO: Finish this
}

flang::FResult flang::ArgumentNode::accept(flang::Interpreter &visitor) {
    std::vector<std::shared_ptr<flang::Object>> args_obj;
    std::unordered_map<std::string, std::shared_ptr<flang::Object>> default_args_obj;

    for (auto& arg : args) {
        auto res = arg->accept(visitor);
        if (res.isError()) return res;
        args_obj.push_back(res.result);
    }

    for (auto& [name, value] : default_args) {
        auto value_res = value->accept(visitor);
        if (value_res.isError()) return value_res;
        default_args_obj[name] = value_res.result;
    }

    return FResult::createResult(std::make_shared<ArgumentObject>(args_obj, default_args_obj, tok), tok);
}

nlohmann::ordered_json flang::ArgumentNode::toJson() const {
    nlohmann::ordered_json j;
    j["type"] = "ArgumentNode";

    j["args"] = nlohmann::ordered_json::array();
    for (const auto& arg: args) {
        j["args"].push_back(arg->toJson());
    }

    j["default_arguments"] = nlohmann::ordered_json::array();
    for (const auto& [name, expr]: default_args) {
        j["default_arguments"].push_back({
            {name, expr->toJson()}
        });
    }

    return j;
}
