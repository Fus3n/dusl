#pragma once

#include <iostream>
#include <utility>
#include <memory>
#include <optional>
#include <unordered_map>

#include "Lexer.hpp"


namespace flang {
    class Interpreter;
    class Object;

    class DataNode {
    public:
        Token tok;

        explicit DataNode(Token tok): tok(std::move(tok)) {}
        virtual ~DataNode() = default;
        [[nodiscard]] virtual std::string toString() const;

        virtual std::shared_ptr<Object> accept(Interpreter& visitor) = 0;
    };

    class ProgramNode: public DataNode {
    public:
        std::vector<std::shared_ptr<DataNode>> statements;

        explicit ProgramNode(Token tok): DataNode(std::move(tok)) {}

        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class BlockNode: public DataNode {
    public:
        std::vector<std::shared_ptr<DataNode>> statements;

        explicit BlockNode(Token tok): DataNode(std::move(tok)) {}
        BlockNode(std::vector<std::shared_ptr<DataNode>> _statements, Token tok): DataNode(std::move(tok)), statements(std::move(_statements)) {}

        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class StringNode: public DataNode {
    public:
        std::string value;

        StringNode(std::string val, Token tok): DataNode(std::move(tok)), value(std::move(val)) {}
        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class IntNode: public DataNode {
    public:
        int64_t value;
        IntNode(int64_t val, Token tok): DataNode(std::move(tok)), value(val) {}
        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class FloatNode: public DataNode {
    public:
        long double value;
        FloatNode(long double val, Token tok): DataNode(std::move(tok)), value(val) {}
        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class ListNode: public DataNode {
    public:
        std::vector<std::shared_ptr<flang::DataNode>> items;
        ListNode(std::vector<std::shared_ptr<flang::DataNode>> _items, Token tok): DataNode(std::move(tok)), items(std::move(_items)) {}
        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class DictionaryNode: public DataNode {
    public:
        typedef std::vector<std::tuple<std::shared_ptr<flang::DataNode>, std::shared_ptr<flang::DataNode>>> KeyValTuples;
        KeyValTuples items;

        DictionaryNode(KeyValTuples _items, Token tok): DataNode(std::move(tok)), items(std::move(_items)) {}

        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class VarAccessNode: public DataNode {
    public:
        std::string value;
        explicit VarAccessNode(Token _tok): DataNode(std::move(_tok)) {}
        explicit VarAccessNode(std::string _value, Token _tok): DataNode(std::move(_tok)), value(std::move(_value)) {}
        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class AssignmentNode: public DataNode {
    public:

        std::shared_ptr<DataNode> expr;
        AssignmentNode(DataNode* _expr, Token tok): DataNode(std::move(tok)), expr(_expr) {}

        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class FunctionDefNode: public DataNode {
    public:
        std::vector<std::shared_ptr<flang::DataNode>> args;
        std::shared_ptr<BlockNode> block;

        FunctionDefNode(Token tok, std::vector<std::shared_ptr<flang::DataNode>> _args, BlockNode* _block):
            DataNode(std::move(tok)), args(std::move(_args)), block(_block) {}

        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class FunctionCallNode: public DataNode {
    public:
        std::vector<std::shared_ptr<flang::DataNode>> args;

        FunctionCallNode(Token tok, std::vector<std::shared_ptr<flang::DataNode>> _args):
        DataNode(std::move(tok)), args(std::move(_args)) {}

        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };


    class StructDefNode: public DataNode {
    public:
        std::vector<std::shared_ptr<DataNode>> values;

        explicit StructDefNode(Token tok): DataNode(std::move(tok)) {}
        StructDefNode(std::vector<std::shared_ptr<DataNode>> _values, Token tok): DataNode(std::move(tok)), values(std::move(_values)) {}
        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class MemberAccessNode: public DataNode {
    public:
        std::shared_ptr<DataNode> left_node;
        std::shared_ptr<DataNode> right_node;

        MemberAccessNode(DataNode* _left_node, DataNode* _right_node, Token tok)
                : DataNode(std::move(tok)), left_node(_left_node), right_node(_right_node) {}

        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
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
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

    class IFNode: public DataNode {
    public:
        ConditionNode cond_node;
        std::shared_ptr<BlockNode> else_node;
        std::vector<ConditionNode> else_ifs;

        explicit IFNode(Token tok, ConditionNode _condNode) : DataNode(std::move(tok)), cond_node(std::move(_condNode)) {}

        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
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
        std::shared_ptr<Object> accept(Interpreter& visitor) override;

        static std::string OpToString(Operations op);

    };

    class ReturnNode: public DataNode {
    public:
        std::shared_ptr<DataNode> return_node;

        ReturnNode(DataNode* _return_node, Token tok) : DataNode(std::move(tok)), return_node(_return_node) {}
        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };


    class WhileLoopNode: public DataNode {
    public:
        ConditionNode cond_node;

        WhileLoopNode(ConditionNode _cond_node, Token tok): DataNode(std::move(tok)), cond_node(std::move(_cond_node)) {}
        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
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
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
        static std::string OpToString(Operations op);
        static Operations TokToOperation(const Token& token);
    };

    class NoneNode: public DataNode {
    public:

        explicit NoneNode(Token tok): DataNode(std::move(tok)){}
        [[nodiscard]] std::string toString() const override;
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
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
        std::shared_ptr<Object> accept(Interpreter& visitor) override;
    };

}
