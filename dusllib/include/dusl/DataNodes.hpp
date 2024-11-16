#pragma once

#include <iostream>
#include <utility>
#include <memory>
#include <optional>
#include <unordered_map>

#include "Serializable.hpp"
#include "Lexer.hpp"

namespace dusl {
    class Interpreter;
    class Object;
    class FResult;

    class DataNode: public Serializable {
    public:
        Token tok;

        explicit DataNode(const Token& tok): tok(tok) {}
        ~DataNode() override = default;
        [[nodiscard]] virtual std::string toString() const;

        virtual dusl::FResult accept(Interpreter& visitor) = 0;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class ProgramNode final : public DataNode {
    public:
        std::vector<std::shared_ptr<DataNode>> statements;

        explicit ProgramNode(const Token& tok): DataNode(tok) {}

        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class BlockNode final : public DataNode {
    public:
        std::vector<std::shared_ptr<DataNode>> statements;

        explicit BlockNode(const Token& tok): DataNode(tok) {}
        BlockNode(std::vector<std::shared_ptr<DataNode>> _statements, const Token &tok): DataNode(tok), statements(std::move(_statements)) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class StringNode final : public DataNode {
    public:
        std::string value;

        StringNode(std::string val, const Token& tok): DataNode(tok), value(std::move(val)) {}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class IntNode final : public DataNode {
    public:
        int64_t value;
        IntNode(const int64_t val, const Token& tok): DataNode(tok), value(val) {}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class FloatNode final : public DataNode {
    public:
        long double value;
        FloatNode(const long double val, const Token &tok): DataNode(tok), value(val) {}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class ListNode final : public DataNode {
    public:
        std::vector<std::shared_ptr<dusl::DataNode>> items;
        ListNode(std::vector<std::shared_ptr<dusl::DataNode>> _items, const Token &tok): DataNode(tok), items(std::move(_items)) {}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class DictionaryNode final : public DataNode {
    public:
        typedef std::vector<std::tuple<std::shared_ptr<dusl::DataNode>, std::shared_ptr<dusl::DataNode>>> KeyValTuples;
        KeyValTuples items;

        DictionaryNode(KeyValTuples _items, const Token& tok): DataNode(tok), items(std::move(_items)) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class VarAccessNode final: public DataNode {
    public:
        std::string value;
        explicit VarAccessNode(const Token &_tok): DataNode(_tok), value(_tok.value) {}
        explicit VarAccessNode(std::string _value, const Token& _tok): DataNode(_tok), value(std::move(_value)) {}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class AssignmentNode final : public DataNode {
    public:

        std::shared_ptr<DataNode> expr;
        AssignmentNode(DataNode* _expr, const Token& tok): DataNode(tok), expr(_expr) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class ArgumentNode final : public DataNode {
    public:
        std::vector<std::shared_ptr<dusl::DataNode>> args;
        std::unordered_map<std::string, std::shared_ptr<dusl::DataNode>> default_args;

        ArgumentNode(std::vector<std::shared_ptr<dusl::DataNode>> _args, std::unordered_map<std::string, std::shared_ptr<dusl::DataNode>> _default_args, const Token &tok)
                : DataNode(tok), args(std::move(_args)), default_args(std::move(_default_args)) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class FunctionDefNode final : public DataNode {
    public:
        std::string func_name;
        bool is_anon = false;

        ArgumentNode args_node;
        std::shared_ptr<BlockNode> block;

        FunctionDefNode(
            std::string _func_name,
            ArgumentNode _args_node,
            BlockNode* _block,
            const Token &tok,
            const bool _is_anon=false
        ): DataNode(tok), func_name(std::move(_func_name)), is_anon(_is_anon), args_node(std::move(_args_node)), block(_block) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class FunctionCallNode final : public DataNode {
    public:
        dusl::ArgumentNode args_node;

        FunctionCallNode(const Token& tok, dusl::ArgumentNode _args_node):
        DataNode(tok), args_node(std::move(_args_node)) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class FunctionCallNodeEXPR final : public DataNode {
    public:
        std::unique_ptr<dusl::DataNode> function_expr;
        dusl::ArgumentNode args_node;

        FunctionCallNodeEXPR(DataNode* _functionExpr, ArgumentNode _args_node, const Token& tok):
                DataNode(tok), function_expr(_functionExpr), args_node(std::move(_args_node)) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class StructDefNode final : public DataNode {
    public:
        std::vector<std::shared_ptr<DataNode>> values;

        explicit StructDefNode(const Token& tok): DataNode(tok) {}
        StructDefNode(std::vector<std::shared_ptr<DataNode>> _values, const Token& tok): DataNode(tok), values(std::move(_values)) {}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class MemberAccessNode final : public DataNode {
    public:
        std::shared_ptr<DataNode> left_node;
        std::shared_ptr<DataNode> right_node;

        MemberAccessNode(DataNode* _left_node, DataNode* _right_node, const Token &tok)
                : DataNode(tok), left_node(_left_node), right_node(_right_node) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class ConditionNode final : public DataNode {
    public:
        std::shared_ptr<DataNode> condition_node;
        std::shared_ptr<DataNode> body_node;

        explicit ConditionNode(const Token& tok): DataNode(tok){}
        explicit ConditionNode(DataNode* _condition_node, DataNode* _body_node, const Token& tok):
            DataNode(tok),
            condition_node(_condition_node),
            body_node(_body_node)
            {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class IFNode final : public DataNode {
    public:
        ConditionNode cond_node;
        std::shared_ptr<BlockNode> else_node;
        std::vector<ConditionNode> else_ifs;

        explicit IFNode(const Token& tok, ConditionNode _condNode) : DataNode(tok), cond_node(std::move(_condNode)) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class LogicalOpNode final : public DataNode {
    public:
        enum Operations {
            OP_AND,
            OP_OR
        };

        std::shared_ptr<DataNode> left_node;
        std::shared_ptr<DataNode> right_node;
        Operations op;

        LogicalOpNode(DataNode* _left, DataNode *_right, const Operations _op, const Token& tok) :
        DataNode(tok), left_node(_left), right_node(_right), op(_op) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;

        static std::string OpToString(Operations op);
        [[nodiscard]] nlohmann::ordered_json toJson() const override;

    };

    class ReturnNode final : public DataNode {
    public:
        std::shared_ptr<DataNode> return_node;

        ReturnNode(DataNode* _return_node, const Token& tok) : DataNode(tok), return_node(_return_node) {}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class WhileLoopNode final : public DataNode {
    public:
        ConditionNode cond_node;

        WhileLoopNode(ConditionNode _cond_node, const Token& tok): DataNode(tok), cond_node(std::move(_cond_node)) {}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class UnaryOpNode final : public DataNode {
    public:
        enum Operations {
            OP_MINUS,
            OP_PLUS,
            OP_NOT,
        };

        std::shared_ptr<DataNode> right_node;
        Operations op;

        UnaryOpNode(DataNode* _right_node, const Operations _op, const Token& tok) : DataNode(tok), right_node(std::move(_right_node)), op(_op) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter &visitor) override;
        static std::string OpToString(Operations op);
        static Operations TokToOperation(const Token& token);
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class IndexNode final : public DataNode {
    public:
        std::unique_ptr<DataNode> expr;
        std::unique_ptr<ListNode> index_args;

        IndexNode(DataNode* _expr, ListNode* _index_args, const Token &tok): DataNode(tok), expr(_expr), index_args(_index_args) {}
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class IndexAssignNode final : public DataNode {
    public:
        std::unique_ptr<DataNode> left_node;
        std::unique_ptr<DataNode> right_node;
        std::unique_ptr<ListNode> index_args;

        IndexAssignNode(DataNode* _left_node, DataNode* _right_node, ListNode* _index_args, const Token& tok): DataNode(tok), left_node(_left_node), right_node(_right_node), index_args(_index_args) {}
        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class NoneNode final : public DataNode {
    public:

        explicit NoneNode(const Token &tok): DataNode(tok){}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class BreakNode final : public DataNode {
    public:
        explicit BreakNode(const Token& tok): DataNode(tok){}
        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class RangeNode final : public DataNode {
    public:
        const std::unique_ptr<DataNode> start;
        const std::unique_ptr<DataNode> end;

        RangeNode(DataNode* _start, DataNode* _end,const Token& tok): DataNode(tok), start(_start), end(_end) {}

        [[nodiscard]] std::string toString() const override;
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class ForLoopNode final : public DataNode {
    public:
        std::string ident;
        std::unique_ptr<DataNode> expr;
        std::unique_ptr<BlockNode> block;

        ForLoopNode(std::string _ident, DataNode* _expr, BlockNode* _block, const Token& tok):
            DataNode(tok), ident(std::move(_ident)), expr(_expr), block(_block) {}

        dusl::FResult accept(Interpreter &visitor) override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class ImportNode final : public DataNode {
    public:
        std::string module_path;
        std::vector<std::string> symbols;
        bool import_all = false;

        ImportNode(std::string _module_path, std::vector<std::string> _symbols, const bool _import_all, const Token& tok): DataNode(tok), module_path(std::move(_module_path)), symbols(std::move(_symbols)), import_all(_import_all) {}
        dusl::FResult accept(Interpreter &visitor) override;

        dusl::FResult loadDynamicLibrary(const std::string &path, Interpreter &visitor, const Token &tok);

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

    class BinOpNode final : public DataNode {
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

        BinOpNode(DataNode* _left, DataNode* _right, const Operations _op, const Token& tok) :
                DataNode(tok), left_node(_left), right_node(_right), op(_op) {}

        [[nodiscard]] std::string toString() const override;
        static std::string OpToString(Operations op);
        static Operations TokToOperation(const Token& token);
        dusl::FResult accept(Interpreter& visitor) override;
        [[nodiscard]] nlohmann::ordered_json toJson() const override;
    };

}
