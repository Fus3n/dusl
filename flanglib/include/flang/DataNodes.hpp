#pragma once

#include <iostream>
#include <utility>
#include <memory>
#include <optional>
#include <unordered_map>

#include "Serializable.h"
#include "Lexer.hpp"

namespace flang {
    class Interpreter;
    class Object;
    class FResult;

    class DataNode: public Serializable {
    public:
        Token tok;

        explicit DataNode(Token tok): tok(std::move(tok)) {}
        virtual ~DataNode() = default;
        [[nodiscard]] virtual std::string toString() const;

        virtual flang::FResult accept(Interpreter& visitor) = 0;
        nlohmann::ordered_json toJson() const override;
    };

    class ProgramNode: public DataNode {
    public:
        std::vector<std::shared_ptr<DataNode>> statements;

        explicit ProgramNode(Token tok): DataNode(std::move(tok)) {}

        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class BlockNode: public DataNode {
    public:
        std::vector<std::shared_ptr<DataNode>> statements;

        explicit BlockNode(Token tok): DataNode(std::move(tok)) {}
        BlockNode(std::vector<std::shared_ptr<DataNode>> _statements, Token tok): DataNode(std::move(tok)), statements(std::move(_statements)) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class StringNode: public DataNode {
    public:
        std::string value;

        StringNode(std::string val, Token tok): DataNode(std::move(tok)), value(std::move(val)) {}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class IntNode: public DataNode {
    public:
        int64_t value;
        IntNode(int64_t val, Token tok): DataNode(std::move(tok)), value(val) {}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class FloatNode: public DataNode {
    public:
        long double value;
        FloatNode(long double val, Token tok): DataNode(std::move(tok)), value(val) {}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class ListNode: public DataNode {
    public:
        std::vector<std::shared_ptr<flang::DataNode>> items;
        ListNode(std::vector<std::shared_ptr<flang::DataNode>> _items, Token tok): DataNode(std::move(tok)), items(std::move(_items)) {}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class DictionaryNode: public DataNode {
    public:
        typedef std::vector<std::tuple<std::shared_ptr<flang::DataNode>, std::shared_ptr<flang::DataNode>>> KeyValTuples;
        KeyValTuples items;

        DictionaryNode(KeyValTuples _items, Token tok): DataNode(std::move(tok)), items(std::move(_items)) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class VarAccessNode: public DataNode {
    public:
        std::string value;
        explicit VarAccessNode(Token _tok): DataNode(std::move(_tok)), value(_tok.value) {}
        explicit VarAccessNode(std::string _value, Token _tok): DataNode(std::move(_tok)), value(std::move(_value)) {}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class AssignmentNode: public DataNode {
    public:

        std::shared_ptr<DataNode> expr;
        AssignmentNode(DataNode* _expr, Token tok): DataNode(std::move(tok)), expr(_expr) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class ArgumentNode: public DataNode {
    public:
        std::vector<std::shared_ptr<flang::DataNode>> args;
        std::unordered_map<std::string, std::shared_ptr<flang::DataNode>> default_args;

        ArgumentNode(std::vector<std::shared_ptr<flang::DataNode>> _args, std::unordered_map<std::string, std::shared_ptr<flang::DataNode>> _default_args, Token tok)
                : DataNode(std::move(tok)), args(std::move(_args)), default_args(std::move(_default_args)) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class FunctionDefNode: public DataNode {
    public:
        std::string func_name;
        bool is_anon = false;

        ArgumentNode args_node;
        std::shared_ptr<BlockNode> block;

        FunctionDefNode(
            std::string _func_name,
            ArgumentNode _args_node,
            BlockNode* _block,
            Token tok,
            bool _is_anon=false
        ): DataNode(std::move(tok)), args_node(std::move(_args_node)), block(_block), func_name(std::move(_func_name)), is_anon(_is_anon) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class FunctionCallNode: public DataNode {
    public:
        flang::ArgumentNode args_node;

        FunctionCallNode(Token tok, flang::ArgumentNode _args_node):
        DataNode(std::move(tok)), args_node(std::move(_args_node)) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class FunctionCallNodeEXPR: public DataNode {
    public:
        std::unique_ptr<flang::DataNode> function_expr;
        flang::ArgumentNode args_node;

        FunctionCallNodeEXPR(DataNode* _functionExpr, ArgumentNode _args_node, Token tok):
                DataNode(std::move(tok)), args_node(std::move(_args_node)), function_expr(_functionExpr) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class StructDefNode: public DataNode {
    public:
        std::vector<std::shared_ptr<DataNode>> values;

        explicit StructDefNode(Token tok): DataNode(std::move(tok)) {}
        StructDefNode(std::vector<std::shared_ptr<DataNode>> _values, Token tok): DataNode(std::move(tok)), values(std::move(_values)) {}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class MemberAccessNode: public DataNode {
    public:
        std::shared_ptr<DataNode> left_node;
        std::shared_ptr<DataNode> right_node;

        MemberAccessNode(DataNode* _left_node, DataNode* _right_node, Token tok)
                : DataNode(std::move(tok)), left_node(_left_node), right_node(_right_node) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class ConditionNode: public DataNode {
    public:
        std::shared_ptr<DataNode> condition_node;
        std::shared_ptr<DataNode> body_node;

        explicit ConditionNode(Token tok): DataNode(std::move(tok)){}
        explicit ConditionNode(DataNode* _condition_node, DataNode* _body_node, Token tok):
            DataNode(std::move(tok)),
            condition_node(_condition_node),
            body_node(_body_node)
            {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class IFNode: public DataNode {
    public:
        ConditionNode cond_node;
        std::shared_ptr<BlockNode> else_node;
        std::vector<ConditionNode> else_ifs;

        explicit IFNode(Token tok, ConditionNode _condNode) : DataNode(std::move(tok)), cond_node(std::move(_condNode)) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class LogicalOpNode: public DataNode {
    public:
        enum Operations {
            OP_AND,
            OP_OR
        };

        std::shared_ptr<DataNode> left_node;
        std::shared_ptr<DataNode> right_node;
        Operations op;

        LogicalOpNode(DataNode* _left, DataNode *_right, Operations _op, Token tok) :
        DataNode(std::move(tok)), left_node(_left), right_node(_right), op(_op) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;

        static std::string OpToString(Operations op);
        nlohmann::ordered_json toJson() const override;

    };

    class ReturnNode: public DataNode {
    public:
        std::shared_ptr<DataNode> return_node;

        ReturnNode(DataNode* _return_node, Token tok) : DataNode(std::move(tok)), return_node(_return_node) {}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class WhileLoopNode: public DataNode {
    public:
        ConditionNode cond_node;

        WhileLoopNode(ConditionNode _cond_node, Token tok): DataNode(std::move(tok)), cond_node(std::move(_cond_node)) {}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class UnaryOpNode : public DataNode {
    public:
        enum Operations {
            OP_MINUS,
            OP_PLUS,
            OP_NOT,
        };

        std::shared_ptr<DataNode> right_node;
        Operations op;

        UnaryOpNode(DataNode* _right_node, Operations _op, Token tok) : DataNode(std::move(tok)), right_node(std::move(_right_node)), op(_op) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter &visitor) override;
        static std::string OpToString(Operations op);
        static Operations TokToOperation(const Token& token);
        nlohmann::ordered_json toJson() const override;
    };

    class IndexNode: public DataNode {
    public:
        std::unique_ptr<DataNode> expr;
        std::unique_ptr<ListNode> index_args;

        IndexNode(DataNode* _expr, ListNode* _index_args, Token tok): DataNode(std::move(tok)), expr(_expr), index_args(_index_args) {}
        flang::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] std::string toString() const override;
        nlohmann::ordered_json toJson() const override;
    };

    class IndexAssignNode: public DataNode {
    public:
        std::unique_ptr<DataNode> left_node;
        std::unique_ptr<DataNode> right_node;
        std::unique_ptr<ListNode> index_args;

        IndexAssignNode(DataNode* _left_node, DataNode* _right_node, ListNode* _index_args, Token tok): DataNode(std::move(tok)), left_node(_left_node), right_node(_right_node), index_args(_index_args) {}
        flang::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] std::string toString() const override;
        nlohmann::ordered_json toJson() const override;
    };

    class NoneNode: public DataNode {
    public:

        explicit NoneNode(Token tok): DataNode(std::move(tok)){}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class BreakNode: public DataNode {
    public:
        explicit BreakNode(Token tok): DataNode(std::move(tok)){}
        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class RangeNode: public DataNode {
    public:
        const std::unique_ptr<DataNode> start;
        const std::unique_ptr<DataNode> end;

        RangeNode(DataNode* _start, DataNode* _end,Token tok): DataNode(std::move(tok)), start(_start), end(_end) {}

        [[nodiscard]] std::string toString() const override;
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

    class ForLoopNode: public DataNode {
    public:
        std::string ident;
        std::unique_ptr<DataNode> expr;
        std::unique_ptr<BlockNode> block;

        ForLoopNode(std::string _ident, DataNode* _expr, BlockNode* _block, Token tok):
            DataNode(std::move(tok)), ident(std::move(_ident)), expr(_expr), block(_block) {}

        flang::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] std::string toString() const override;
        nlohmann::ordered_json toJson() const override;
    };

    class ImportNode: public DataNode {
    public:
        std::string module_path;
        std::vector<std::string> symbols;
        bool import_all = false;

        ImportNode(std::string _module_path, std::vector<std::string> _symbols, bool _import_all, Token tok): DataNode(std::move(tok)), module_path(std::move(_module_path)), symbols(std::move(_symbols)), import_all(_import_all) {}
        flang::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] std::string toString() const override;
        nlohmann::ordered_json toJson() const override;
    };

    class BinOpNode : public DataNode {
    public:
        enum Operations {
            OP_PLUS,
            OP_MINUS,
            OP_MUL,
            OP_DIV,
            OP_MODULO,
            OP_LESS_THAN,
            OP_GREATER_THAN,
            OP_LESS_OR_EQUAL,
            OP_GREATER_OR_EQUAL,
            OP_EQUAL_TO,
            OP_NOT_EQUAL_TO
        };

        std::shared_ptr<DataNode> left_node;
        std::shared_ptr<DataNode> right_node;
        Operations op;

        BinOpNode(DataNode* _left, DataNode* _right, Operations _op, Token tok) :
                DataNode(std::move(tok)), left_node(_left), right_node(_right), op(_op) {}

        [[nodiscard]] std::string toString() const override;
        static std::string OpToString(Operations op);
        static Operations TokToOperation(const Token& token);
        flang::FResult accept(Interpreter& visitor) override;
        nlohmann::ordered_json toJson() const override;
    };

}
