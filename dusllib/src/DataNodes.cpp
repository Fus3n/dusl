#include <fmt/core.h>
#include <sstream>
#include <filesystem>

#include <dusl/utils/dusl_standard.hpp>
#include <dusl/DataNodes.hpp>
#include <dusl/Interpreter.hpp> // Include the full definition of Interpreter
#include <dusl/ErrorType.hpp>
#include <dusl/utils/common.h>
#include <dusl/DObject.hpp>
#include <dusl/Parser.hpp>


std::string dusl::DataNode::toString() const {
    return {};
}

nlohmann::ordered_json dusl::DataNode::toJson() const {
    return {
        {"tok", tok.toJson()},
    };
}

std::string dusl::StringNode::toString() const {
    return fmt::format("StringNode(\"{}\")", value);
}

std::string dusl::IntNode::toString() const {
    return fmt::format("IntNode({})", value);
}


std::string dusl::FloatNode::toString() const {
    return fmt::format("FloatNode({})", value);
}

std::string dusl::ListNode::toString() const {
    std::stringstream ss;
    ss << "[";
    for (auto& item: items) {
        ss << item->toString() << ", ";
    }
    ss << "]";
    return fmt::format("ListNode({})", ss.str());
}

std::string dusl::VarAccessNode::toString() const {
    return fmt::format("VarAccessNode({})", tok.value);
}

std::string dusl::AssignmentNode::toString() const {
    return fmt::format("AssignmentNode(\"{}\" -> {})", tok.value, expr->toString());
}

std::string dusl::FunctionDefNode::toString() const {
    return fmt::format("FunctionDefNode({}, \nargs={}\nblock={})", func_name, "<TODO>", block->toString());
}

std::string dusl::FunctionCallNode::toString() const {
    return fmt::format("FunctionCallNode({}, args={})", tok.value, args_node.toString());
}

std::string dusl::BlockNode::toString() const {
    std::ostringstream oss;
    oss << "BlockNode(\n";
    for (const auto& statement: this->statements) {
        oss << "\t" + statement->toString() << "\n";
    }
    oss << ")";
    return oss.str();
}


std::string dusl::BinOpNode::toString() const {
    std::string out;

    auto left_str = left_node->toString();
    auto right_str = right_node->toString();

    return fmt::format("BinOpNode({} {} {})", left_str, BinOpNode::OpToString(op), right_str);
}

std::string dusl::BinOpNode::OpToString(dusl::BinOpNode::Operations op) {
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

std::string dusl::NoneNode::toString() const {
    return "none";
}

std::string dusl::StructDefNode::toString() const {
    std::string values_str = "[\n";
    for (const auto& value: this->values) {
        values_str += "\t" + value->toString() + "\n";
    }
    values_str += "]";

    return fmt::format("StructDefNode({})", values_str);
}

std::string dusl::MemberAccessNode::toString() const {
    return fmt::format("MemberAccessNode({} dotted by {})", left_node->toString(), right_node->toString());
}

dusl::FResult dusl::ProgramNode::accept(Interpreter &visitor) {
    dusl::FResult val;
    for (auto& statement: statements) {
        val = statement->accept(visitor);
        if (val.isError())
            return val;
    }
    return val;
}

nlohmann::ordered_json dusl::ProgramNode::toJson() const {
    nlohmann::ordered_json json;
    json["type"] = "ProgramNode";
    json["statements"] = nlohmann::ordered_json::array();
    for (auto& statement: statements) {
        json["statements"].push_back(statement->toJson());
    }
    return json;
}

dusl::FResult dusl::BlockNode::accept(Interpreter &visitor) {
    dusl::FResult val;
    for (auto& statement: statements) {
        val = statement->accept(visitor);

        if (val.isError() || val.result->isBreak() || val.result->isReturn())
            return val;
    }
    return val;
}

nlohmann::ordered_json dusl::BlockNode::toJson() const {
    nlohmann::ordered_json json;
    json["type"] = "BlockNode";
    json["statements"] = nlohmann::ordered_json::array();
    for (auto& statement: statements) {
        json["statements"].push_back(statement->toJson());
    }
    return json;
}

dusl::FResult dusl::StringNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<StringObject>(value, tok), tok);
}

nlohmann::ordered_json dusl::StringNode::toJson() const {
    return {
        {"type","StringNode"},
        {"value",value},
    };
}

dusl::FResult dusl::IntNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<IntObject>(value, tok), tok);
}

nlohmann::ordered_json dusl::IntNode::toJson() const {
    return {
        {"type","IntNode"},
        {"value",value},
    };
}

dusl::FResult dusl::FloatNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<FloatObject>(value, tok), tok);
}

nlohmann::ordered_json dusl::FloatNode::toJson() const {
    return {
        {"type","FloatNode"},
        {"value",value},
    };
}

dusl::FResult dusl::VarAccessNode::accept(Interpreter &visitor) {
    if (!visitor.ctx.currentSymbol().hasKey(tok.value)) {
        return FResult::createError(
            NameError,
            fmt::format("variable '{}' is not defined", tok.value),
            tok
        );
    }

    return FResult::createResult(visitor.ctx.currentSymbol().getValue(tok.value), tok);
}

nlohmann::ordered_json dusl::VarAccessNode::toJson() const {
    return {
        {"type", "VarAccessNode"},
        {"value", tok.value}
    };
}

dusl::FResult dusl::AssignmentNode::accept(Interpreter &visitor) {
    auto xp = expr->accept(visitor);
    if (xp.isError())
        return xp;

    visitor.ctx.currentSymbol().setValue(tok.value, xp.result);
    return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
}

nlohmann::ordered_json dusl::AssignmentNode::toJson() const {
    return {
        {"type", "AssignmentNode"},
        {"key", tok.value},
        {"value", expr->toJson()}
    };
}

dusl::FResult dusl::FunctionDefNode::accept(Interpreter &visitor) {
    std::vector<std::string> args;
    std::unordered_map<std::string, std::shared_ptr<dusl::Object>> default_args;

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

nlohmann::ordered_json dusl::FunctionDefNode::toJson() const {
    return {
        {"type", "FunctionDefNode"},
        {"args", args_node.toJson()},
        {"block", block->toJson()}
    };
}

dusl::FResult dusl::FunctionCallNode::accept(Interpreter &visitor) {
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

nlohmann::ordered_json dusl::FunctionCallNode::toJson() const {
    return {
        {"type", "FunctionCallNode"},
        {"args", args_node.toJson()}
    };
}

dusl::FResult dusl::BinOpNode::accept(Interpreter &visitor) {
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

nlohmann::ordered_json dusl::BinOpNode::toJson() const {
    return {
        {"type", "BinOpNode"},
        {"left", left_node->toJson()},
        {"op", BinOpNode::OpToString(op)},
        {"right", right_node->toJson()}
    };
}

dusl::BinOpNode::Operations dusl::BinOpNode::TokToOperation(const dusl::Token& token) {
    switch (token.tok) {
        case dusl::TokenType::Plus:
            return OP_PLUS;
        case dusl::TokenType::Minus:
            return OP_MINUS;
        case dusl::TokenType::Multiply:
            return OP_MUL;
        case dusl::TokenType::Divide:
            return OP_DIV;
        case dusl::TokenType::Modulo:
            return OP_MODULO;
        case dusl::TokenType::LessThan:
            return OP_LESS_THAN;
        case dusl::TokenType::GreaterThan:
            return OP_GREATER_THAN;
        case dusl::TokenType::GreaterOrEqual:
            return OP_GREATER_OR_EQUAL;
        case dusl::TokenType::LessOrEqual:
            return OP_LESS_OR_EQUAL;
        case dusl::TokenType::EqualTo:
            return OP_EQUAL_TO;
        case dusl::TokenType::NotEqualTo:
            return OP_NOT_EQUAL_TO;
        default:
            DError(RunTimeError,
                   fmt::format("Invalid binary operation '{}'", tokToString(token.tok)),
                   token.pos
            ).Throw();
            return OP_GREATER_OR_EQUAL;
    }

}

dusl::FResult dusl::NoneNode::accept(Interpreter &visitor) {
    return FResult::createResult(std::make_shared<NoneObject>(tok), tok);
}

nlohmann::ordered_json dusl::NoneNode::toJson() const {
    return {
        {"type", "NoneNode"}
    };
}

dusl::FResult dusl::StructDefNode::accept(Interpreter &visitor) {
    std::unordered_map<std::string, std::shared_ptr<Object>> properties;

    // evaluate the values
    for (auto& val: values) {
        if (const auto assignNode = dynamic_cast<AssignmentNode*>(val.get())) {
            auto prop_res = assignNode->expr->accept(visitor);
            if (prop_res.isError()) return prop_res;
            properties[assignNode->tok.value] = prop_res.result;
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

nlohmann::ordered_json dusl::StructDefNode::toJson() const {
    nlohmann::ordered_json json;
    json["type"] = "StructDefNode";
    json["properties"] = nlohmann::ordered_json::array();
    for (auto& prop: values) {
        json["properties"].push_back(prop->toJson());
    }
    return json;
}

dusl::FResult dusl::MemberAccessNode::accept(Interpreter &visitor) {
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

nlohmann::ordered_json dusl::MemberAccessNode::toJson() const {
    return {
        {"type", "MemberAccessNode"},
        {"left", left_node->toJson()},
        {"right", right_node->toJson()}
    };
}

dusl::FResult dusl::ListNode::accept(Interpreter &visitor) {
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

nlohmann::ordered_json dusl::ListNode::toJson() const {
    nlohmann::ordered_json j;
    j["type"] = "ListNode";
    j["items"] = nlohmann::ordered_json::array();
    for (auto& item: items) {
        j["items"].push_back(item->toJson());
    }
    return j;
}

std::string dusl::IFNode::toString() const {
    std::string else_node_str = "[\n";
    for (const auto& value: this->else_ifs) {
        else_node_str += "\t" + value.toString() + "\n";
    }
    else_node_str += "]";
    return fmt::format("IFNode(\n\t\tcondition: {}\n\t\telse_ifs: {}\n\t\telse_node: {})", cond_node.toString(), else_node_str, else_node ? else_node->toString() : "");
}

dusl::FResult dusl::IFNode::accept(Interpreter &visitor) {
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

nlohmann::ordered_json dusl::IFNode::toJson() const {
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

std::string dusl::ConditionNode::toString() const {
    return fmt::format("ConditionNode(cond: {}, body: {})", condition_node->toString(), body_node->toString());
}

dusl::FResult dusl::ConditionNode::accept(Interpreter &visitor) {
    return {};
}

nlohmann::ordered_json dusl::ConditionNode::toJson() const {
    return {
        {"type", "ConditionNode"},
        {"condition", condition_node->toJson()},
        {"body", body_node->toJson()}
    };
}

std::string dusl::LogicalOpNode::toString() const {
    return fmt::format("LogicalOpNode(left: {}, op: {}, right: {})", left_node->toString(), OpToString(op),
                       right_node->toString());
}
std::string dusl::LogicalOpNode::OpToString(dusl::LogicalOpNode::Operations op) {
    switch (op) {
        case OP_AND:
            return "and";
        case OP_OR:
            return "or";
        default:
            return "UnknownLogicalOP";
    }
}

nlohmann::ordered_json dusl::LogicalOpNode::toJson() const {
    return {
        {"type", "LogicalOpNode"},
        {"left", left_node->toJson()},
        {"op", OpToString(op)},
        {"right", right_node->toJson()}
    };
}

dusl::FResult dusl::LogicalOpNode::accept(Interpreter &visitor) {
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


std::string dusl::ReturnNode::toString() const {
    return DataNode::toString();
}

dusl::FResult dusl::ReturnNode::accept(Interpreter &visitor) {
    auto ret = return_node->accept(visitor);
    if (ret.isError())
        return ret;

    return FResult::createResult(std::make_shared<ReturnObject>(ret.result, tok), tok);
}

nlohmann::ordered_json dusl::ReturnNode::toJson() const {
    return {
        {"type", "ReturnNode"},
        {"node_to_return", return_node->toJson()}
    };
}

std::string dusl::UnaryOpNode::toString() const {
    return fmt::format("UnaryOpNode(op: {}, expr: {})", OpToString(op), right_node->toString());
}

std::string dusl::UnaryOpNode::OpToString(dusl::UnaryOpNode::Operations op) {
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

dusl::UnaryOpNode::Operations dusl::UnaryOpNode::TokToOperation(const dusl::Token &token) {
    switch (token.tok) {
        case Plus:
            return OP_PLUS;
        case Minus:
            return OP_MINUS;
        case Keyword: // keyword only needs to be "not"
            return OP_NOT;
        default:
            DError(RunTimeError,
                   fmt::format("Invalid unary operation '{}'", tokToString(token.tok)),
                   token.pos
            ).Throw();
            return OP_PLUS;
    }
}

nlohmann::ordered_json dusl::UnaryOpNode::toJson() const {
    return {
        {"type","UnaryOpNode"},
        {"op", OpToString(op)},
        {"expr", right_node->toJson()}
    };
}

dusl::FResult dusl::UnaryOpNode::accept(Interpreter &visitor) {
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
        default:
            return FResult::createError(ErrorType::Error, "Unknown unary operator", tok);
    }
}

std::string dusl::WhileLoopNode::toString() const {
    return fmt::format("WhileLoopNode(cond: {})", cond_node.toString());
}

dusl::FResult dusl::WhileLoopNode::accept(Interpreter &visitor) {
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

nlohmann::ordered_json dusl::WhileLoopNode::toJson() const {
    return {
        {"type", "WhileLoopNode"},
        {"condition", cond_node.toJson()}
    };
}

std::string dusl::DictionaryNode::toString() const {
    // TODO: Finish this
    return "DictionaryNode";
}

dusl::FResult dusl::DictionaryNode::accept(Interpreter &visitor) {
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

nlohmann::ordered_json dusl::DictionaryNode::toJson() const {
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

std::string dusl::FunctionCallNodeEXPR::toString() const {
    return fmt::format("FunctionCallNodeEXPR(func: {}, args: {})", function_expr->toString(), args_node.toString());
}

dusl::FResult dusl::FunctionCallNodeEXPR::accept(Interpreter &visitor) {
    auto functionObj = function_expr->accept(visitor);
    if (functionObj.isError())
        return functionObj;

    auto args_result = args_node.accept(visitor);
    if (args_result.isError())
        return args_result;

    ArgumentObject arg_obj_new = *dynamic_cast<ArgumentObject*>(args_result.result.get());

    return functionObj.result->call(visitor, arg_obj_new, tok);
}

nlohmann::ordered_json dusl::FunctionCallNodeEXPR::toJson() const {
    return {
        {"type", "FunctionCallNodeEXPR"},
        {"args", args_node.toJson()},
        {"function_expr", function_expr->toJson()}
    };
}

dusl::FResult dusl::IndexNode::accept(Interpreter &visitor) {
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

std::string dusl::IndexNode::toString() const {
    return fmt::format("IndexNode(expr: {}, index_args: {})", expr->toString(), index_args->toString());
}

nlohmann::ordered_json dusl::IndexNode::toJson() const {
    return {
        {"type", "IndexNode"},
        {"left", expr->toJson()},
        {"index_args", index_args->toJson()}
    };
}

std::string dusl::IndexAssignNode::toString() const {
    return DataNode::toString();
}

nlohmann::ordered_json dusl::IndexAssignNode::toJson() const {
    return {
        {"type", "IndexAssignNode"},
        {"left", left_node->toJson()},
        {"right", right_node->toJson()},
        {"index_args", index_args->toJson()}
    };
}

dusl::FResult dusl::IndexAssignNode::accept(Interpreter &visitor) {
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

std::string dusl::BreakNode::toString() const {
    return "BreakNode";
}

dusl::FResult dusl::BreakNode::accept(dusl::Interpreter &visitor) {
    return FResult::createResult(std::make_shared<BreakObject>(tok), tok);
}

nlohmann::ordered_json dusl::BreakNode::toJson() const {
    return {
        {"type", "BreakNode"}
    };
}

std::string dusl::RangeNode::toString() const {
    return fmt::format("RngNode({}:{})",  start->toString(), end->toString());
}

dusl::FResult dusl::RangeNode::accept(dusl::Interpreter &visitor) {
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

nlohmann::ordered_json dusl::RangeNode::toJson() const {
    return {
        {"type", "RangeNode"},
        {"start", start->toJson()},
        {"end", end->toJson()}
    };
}

std::string dusl::ForLoopNode::toString() const {
    return fmt::format("ForLoopNode(ident: {}, expr: {}, block: {})", ident, expr->toString(), block->toString());
}

nlohmann::ordered_json dusl::ForLoopNode::toJson() const {
    return {
        {"type", "ForLoopNode"},
        {"expr", expr->toJson()},
        {"block", block->toJson()}
    };
}

dusl::FResult dusl::ForLoopNode::accept(dusl::Interpreter &visitor) {
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

std::string dusl::ImportNode::toString() const {
    std::stringstream ss;
    ss << "ImportNode(symbols: [";
    for (const auto& ident: symbols) {
        ss << ident << ", ";
    }
    ss << "])";
    return ss.str();
}

nlohmann::ordered_json dusl::ImportNode::toJson() const {
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

dusl::FResult dusl::ImportNode::accept(dusl::Interpreter &visitor) {
    dusl::Lexer lexer;
    dusl::Parser parser;
    std::string code;

    std::filesystem::path path;

    // check if string starts with "std:"
    if (module_path.size() > 4 && module_path.substr(0, 4) == "std:") {
        auto base = module_path.substr(4);
        path = std::filesystem::path(DUSL_STD_PATH) / base;
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
        code = dusl::read_file(path.string());
    } catch (std::exception &e) {
        fmt::println("{}, FILE: {}", e.what(), path.string());
        return FResult::createError(
            ImportError,
            fmt::format("Could not import module '{}'", path.string()),
            tok
        );
    }

    dusl::str_trim(code);
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

std::string dusl::ArgumentNode::toString() const {
    return "ArgumentNode([])"; // TODO: Finish this
}

dusl::FResult dusl::ArgumentNode::accept(dusl::Interpreter &visitor) {
    std::vector<std::shared_ptr<dusl::Object>> args_obj;
    std::unordered_map<std::string, std::shared_ptr<dusl::Object>> default_args_obj;

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

nlohmann::ordered_json dusl::ArgumentNode::toJson() const {
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
