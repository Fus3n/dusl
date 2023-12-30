#pragma once
#include <iostream>
#include <utility>
#include <memory>
#include "Lexer.hpp"
#include "DataNodes.hpp"
#include <functional>
#include <unordered_map>

namespace flang {
    class Interpreter;
    class FunctionCallNode;


    // Base class for the objects that can be returned from visit functions
    class Object {
    public:
        Token tok;
        explicit Object(Token tok): tok(std::move(tok)) {}

        [[nodiscard]] virtual std::string toString() const = 0;
        [[nodiscard]] virtual bool isTrue() const;
        virtual std::shared_ptr<flang::Object> getProperty(const std::string& name);
        virtual std::shared_ptr<flang::Object> callProperty(Interpreter& visitor, const std::shared_ptr<flang::FunctionCallNode>& fn_node);

        [[nodiscard]] virtual std::string getTypeString() const;
        [[nodiscard]] virtual size_t hash(const flang::Token &token) const;

        [[nodiscard]] virtual bool isReturn() const;

        virtual std::shared_ptr<flang::Object> call(Interpreter& visitor, std::vector<std::shared_ptr<Object>>& arguments, const Token &token);

        // object functions
        virtual std::shared_ptr<flang::Object> add_to(const std::shared_ptr<Object>& other, const Token &token);
        virtual std::shared_ptr<flang::Object> sub_by(const std::shared_ptr<Object>& other, const Token &token);
        virtual std::shared_ptr<flang::Object> multiplied_by(const std::shared_ptr<Object> &other, const Token &token);
        virtual std::shared_ptr<flang::Object> divided_by(const std::shared_ptr<Object> &other, const Token &token);
        virtual std::shared_ptr<flang::Object> modulo_by(const std::shared_ptr<Object> &other, const Token &token);
        virtual std::shared_ptr<flang::Object> greater_than(const std::shared_ptr<Object> &other, const Token &token);
        virtual std::shared_ptr<flang::Object> less_than(const std::shared_ptr<Object> &other, const Token &token);
        virtual std::shared_ptr<flang::Object> greater_or_equal(const std::shared_ptr<Object> &other, const Token &token);
        virtual std::shared_ptr<flang::Object> less_or_equal(const std::shared_ptr<Object> &other, const Token &token);
        virtual std::shared_ptr<flang::Object> equal_to(const std::shared_ptr<Object> &other, const Token &token);
        virtual std::shared_ptr<flang::Object> not_equal_to(const std::shared_ptr<Object> &other, const Token &token);

        virtual std::shared_ptr<flang::Object> unary_plus(const Token &token);
        virtual std::shared_ptr<flang::Object> unary_minus(const Token &token);
        virtual std::shared_ptr<flang::Object> unary_not(const Token &token);
    };

// Derived classes for specific types
    class NoneObject : public Object {
    public:
        explicit NoneObject(Token tok) : Object(std::move(tok)) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;
    };

    class ReturnObject: public Object {
    public:
        std::shared_ptr<Object> return_obj;

        ReturnObject(std::shared_ptr<Object> _return_obj, Token tok) : Object(std::move(tok)), return_obj(std::move(_return_obj)) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;
        [[nodiscard]] bool isReturn() const override;
    };

    class StringObject : public Object {
    public:
        std::string value;

        explicit StringObject(std::basic_string<char> val, Token tok) : Object(std::move(tok)), value(std::move(val)) {}
        explicit StringObject(char val, Token tok) : Object(std::move(tok)), value(std::move(std::string(1, val))) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] size_t hash(const flang::Token &token) const override;

        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;
        std::shared_ptr<flang::Object> getProperty(const std::string &name) override;
        std::shared_ptr<flang::Object> callProperty(Interpreter& visitor, const std::shared_ptr<flang::FunctionCallNode> &fn_node) override;
        std::shared_ptr<flang::Object> add_to(const std::shared_ptr<Object> &other, const flang::Token &token) override;
        std::shared_ptr<flang::Object> equal_to(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> not_equal_to(const std::shared_ptr<Object> &other, const Token &token) override;
    };

    class NumberError {
    public:
        static void throwZeroDivisionError(const Token &leftToken, const std::shared_ptr<Object>& other);
    };

    class IntObject : public Object {
    public:
        int64_t value;

        explicit IntObject(int64_t val, Token tok) : Object(std::move(tok)), value(val) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] size_t hash(const flang::Token &token) const override;
        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;

        std::shared_ptr<flang::Object> add_to(const std::shared_ptr<Object>& other, const Token &token) override;
        std::shared_ptr<flang::Object> sub_by(const std::shared_ptr<Object> &other, const flang::Token &token) override;
        std::shared_ptr<flang::Object> multiplied_by(const std::shared_ptr<Object> &other, const flang::Token &token) override;
        std::shared_ptr<flang::Object> divided_by(const std::shared_ptr<Object> &other, const flang::Token &token) override;
        std::shared_ptr<flang::Object> modulo_by(const std::shared_ptr<Object> &other, const flang::Token &token) override;
        std::shared_ptr<flang::Object> greater_than(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> less_than(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> less_or_equal(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> equal_to(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> not_equal_to(const std::shared_ptr<Object> &other, const Token &token) override;

        std::shared_ptr<flang::Object> unary_plus(const Token &token) override;
        std::shared_ptr<flang::Object> unary_minus(const Token &token) override;
    };

    class FloatObject : public Object {
    public:
        long double value;

        explicit FloatObject(double val, Token tok) : Object(std::move(tok)), value(val) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] size_t hash(const flang::Token &token) const override;
        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;

        std::shared_ptr<flang::Object> add_to(const std::shared_ptr<Object>& other, const Token &token) override;
        std::shared_ptr<flang::Object> sub_by(const std::shared_ptr<Object> &other, const flang::Token &token) override;
        std::shared_ptr<flang::Object> multiplied_by(const std::shared_ptr<Object> &other, const flang::Token &token) override;
        std::shared_ptr<flang::Object> divided_by(const std::shared_ptr<Object> &other, const flang::Token &token) override;
        std::shared_ptr<flang::Object> modulo_by(const std::shared_ptr<Object> &other, const flang::Token &token) override;
        std::shared_ptr<flang::Object> greater_than(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> less_than(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> less_or_equal(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> equal_to(const std::shared_ptr<Object> &other, const Token &token) override;
        std::shared_ptr<flang::Object> not_equal_to(const std::shared_ptr<Object> &other, const Token &token) override;

        std::shared_ptr<flang::Object> unary_plus(const Token &token) override;
        std::shared_ptr<flang::Object> unary_minus(const Token &token) override;
    };


    class ListObject: public Object {
    public:
        typedef std::shared_ptr<Object> (*PropertyFunction)(ListObject&, Interpreter&, const std::shared_ptr<FunctionCallNode>&);
        std::unordered_map<std::string, PropertyFunction> functions;

        std::vector<std::shared_ptr<Object>> items;

        explicit ListObject(Token tok);

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] bool isTrue() const override;
        std::shared_ptr<flang::Object> getProperty(const std::string &name) override;
        std::shared_ptr<flang::Object> callProperty(Interpreter& visitor, const std::shared_ptr<flang::FunctionCallNode> &fn_node) override;
        std::string getTypeString() const override;

        void preload_functions();
        // functions
        static std::shared_ptr<flang::Object> get(ListObject& list, flang::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static std::shared_ptr<flang::Object> push(ListObject& list, Interpreter& visitor, const std::shared_ptr<FunctionCallNode>& fn_node);
        static std::shared_ptr<flang::Object> set(ListObject& list, Interpreter& visitor, const std::shared_ptr<FunctionCallNode>& fn_node);

        // STATICS
        static std::shared_ptr<Object> splitAtDelimiter(const std::string &value, const std::string& delim, const Token& tok);
    };

    class DictionaryObject: public Object {
    public:
        std::unordered_map<size_t, std::tuple<std::shared_ptr<Object>, std::shared_ptr<Object>>> items;

        DictionaryObject(std::unordered_map<size_t, std::tuple<std::shared_ptr<Object>, std::shared_ptr<Object>>> _items, Token tok): Object(std::move(tok)), items(std::move(_items)) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        std::shared_ptr<flang::Object> callProperty(Interpreter& visitor, const std::shared_ptr<flang::FunctionCallNode> &fn_node) override;
        std::shared_ptr<flang::Object> getProperty(const std::string &name) override;
    };

    class BooleanObject: public Object {
    public:
        bool value;
        BooleanObject(bool _value, Token tok) : Object(std::move(tok)), value(_value) {}

        [[nodiscard]] size_t hash(const flang::Token &token) const override;

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] bool isTrue() const override;
    };

    class FunctionObject: public Object {
    public:
        std::string func_name;
        std::vector<std::shared_ptr<flang::DataNode>> args;
        std::shared_ptr<BlockNode> block;

        FunctionObject(
                std::string _func_name,
                std::vector<std::shared_ptr<flang::DataNode>> _args,
                std::shared_ptr<BlockNode> _block,
                Token tok
                ): Object(std::move(tok)), func_name(std::move(_func_name)), args(std::move(_args)), block(std::move(_block)) {}


        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] size_t hash(const flang::Token &token) const override;

        std::shared_ptr<flang::Object> call(flang::Interpreter &visitor, std::vector<std::shared_ptr<Object>> &args, const flang::Token &token) override;
    };

    class BuiltinFunctionObject: public Object {
    public:
        typedef std::shared_ptr<Object> (*FunctionPointer)(Interpreter&, const std::vector<std::shared_ptr<Object>>&, const flang::Token& tok);
        FunctionPointer body_func;
        BuiltinFunctionObject(Token tok, FunctionPointer body): Object(std::move(tok)), body_func(body) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] size_t hash(const flang::Token &token) const override;

        std::shared_ptr<flang::Object> call(flang::Interpreter &visitor, std::vector<std::shared_ptr<Object>> &arguments, const flang::Token &token) override;

    };


}