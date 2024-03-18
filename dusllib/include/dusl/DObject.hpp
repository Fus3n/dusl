#pragma once

#include <iostream>
#include <utility>
#include <memory>
#include "Lexer.hpp"
#include "DataNodes.hpp"
#include <functional>
#include <unordered_map>
#include <windows.h>
#include "ErrorType.hpp"
#include "fmt/core.h"

namespace dusl {
    class Interpreter;
    class FunctionCallNode;
    class ListObject;
    class ErrorObject;
    class ArgumentObject;

    class FResult {
    public:
        std::shared_ptr<ErrorObject> err;
        std::shared_ptr<Object> result;

        FResult() = default;

        explicit FResult(std::shared_ptr<Object> _result) : result(std::move(_result)), is_error(false) {}
        explicit FResult(std::shared_ptr<ErrorObject> _err) : err(_err), is_error(true) {}

        static dusl::FResult createError(ErrorType err_type, std::string msg, const Token& tok);
        static dusl::FResult createResult(const std::shared_ptr<Object> &_result, const Token &tok);

        [[nodiscard]] bool isError() const;
        [[nodiscard]] std::string toString() const;

    private:
        bool is_error = false;
    };

    // Base class for the objects that can be returned from visit functions
    class Object {
    public:
        using PropertyFunction = dusl::FResult (*)(Object &, Interpreter &,
                                                   const std::shared_ptr<FunctionCallNode> &);
        std::unordered_map<std::string, PropertyFunction> functions;

        Token tok;
        explicit Object(Token tok) : tok(std::move(tok)) {}

        [[nodiscard]] virtual bool isTrue() const;
        [[nodiscard]] virtual bool isReturn() const;
        [[nodiscard]] virtual bool isBreak() const;

        virtual dusl::FResult getProperty(const std::string &name, const Token &token);
        virtual dusl::FResult callProperty(Interpreter &visitor, const std::shared_ptr<dusl::FunctionCallNode> fn_node);

        [[nodiscard]] virtual std::string toString() const = 0;
        [[nodiscard]] virtual std::string getTypeString() const;
        [[nodiscard]] virtual dusl::FResult hash(const dusl::Token &token) const;

        virtual dusl::FResult call(Interpreter &visitor, ArgumentObject &args_node, const Token &token);
        virtual dusl::FResult index(std::shared_ptr<ListObject> idx_args, const std::optional<Token> token = std::nullopt);
        virtual dusl::FResult index_assign(std::shared_ptr<Object> &right, std::shared_ptr<ListObject> idx_args);

        // object functions
        virtual dusl::FResult add_to(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult sub_by(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult multiplied_by(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult divided_by(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult modulo_by(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult greater_than(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult less_than(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult greater_or_equal(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult less_or_equal(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult equal_to(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult not_equal_to(const std::shared_ptr<Object> &other, const Token &token);
        virtual dusl::FResult unary_plus(const Token &token);
        virtual dusl::FResult unary_minus(const Token &token);
        virtual dusl::FResult unary_not(const Token &token);
    };

// Derived classes for specific types
    class NoneObject : public Object {
    public:
        explicit NoneObject(Token tok) : Object(std::move(tok)) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;
    };

    class ReturnObject : public Object {
    public:
        std::shared_ptr<Object> return_obj;

        ReturnObject(std::shared_ptr<Object> _return_obj, Token tok) : Object(std::move(tok)),
                                                                       return_obj(std::move(_return_obj)) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;
        [[nodiscard]] bool isReturn() const override;
    };

    class StringObject : public Object {
    public:
        std::string value;

        explicit StringObject(std::basic_string<char> val, Token tok) : Object(std::move(tok)), value(std::move(val)) {
            init_functions();
        }

        explicit StringObject(char val, Token tok) : Object(std::move(tok)), value(std::move(std::string(1, val))) {
            init_functions();
        }

        void init_functions();

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] dusl::FResult hash(const dusl::Token &token) const override;
        dusl::FResult index(std::shared_ptr<ListObject> idx_args, const std::optional<Token> token = std::nullopt) override;
        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;

        dusl::FResult getProperty(const std::string &name, const Token &token) override;
        dusl::FResult add_to(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult equal_to(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult not_equal_to(const std::shared_ptr<Object> &other, const Token &token) override;

    private:
        // functions
        static dusl::FResult get(StringObject &str, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult split(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult to_int(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult to_float(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult to_lower(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult to_upper(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult is_digit(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult is_alpha(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult is_upper(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult is_lower(StringObject& str, Interpreter& visitor, const std::shared_ptr<FunctionCallNode>& fn_node);
        static dusl::FResult get_code_at(StringObject& str, Interpreter& visitor, const std::shared_ptr<FunctionCallNode>& fn_node);
        static dusl::FResult replace(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult join(StringObject& str, Interpreter& visitor, const std::shared_ptr<FunctionCallNode>& fn_node);
        static dusl::FResult starts_with(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult ends_with(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);


    };

    class NumberError {
    public:
        static dusl::FResult throwZeroDivisionError(const Token &leftToken, const std::shared_ptr<Object> &other);
    };

    class IntObject : public Object {
    public:
        int64_t value;

        explicit IntObject(int64_t val, Token tok) : Object(std::move(tok)), value(val) {
            init_functions();
        }

        explicit IntObject(int64_t val) : Object(Token()), value(val) {
            init_functions();
        }

        void init_functions();

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] dusl::FResult hash(const dusl::Token &token) const override;
        [[nodiscard]] bool isTrue() const override;

        [[nodiscard]] std::string getTypeString() const override;
        dusl::FResult add_to(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult sub_by(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult multiplied_by(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult divided_by(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult modulo_by(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult greater_than(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult less_than(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult less_or_equal(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult equal_to(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult not_equal_to(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult unary_plus(const Token &token) override;
        dusl::FResult unary_minus(const Token &token) override;

    private:
        // functions
        static dusl::FResult
        to_string(IntObject &integer, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);

    };

    class FloatObject : public Object {
    public:
        long double value;

        explicit FloatObject(double val, Token tok) : Object(std::move(tok)), value(val) {
            init_functions();
        }

        explicit FloatObject(double val) : Object(Token()), value(val) {
            init_functions();
        }

        void init_functions();

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] dusl::FResult hash(const dusl::Token &token) const override;

        [[nodiscard]] bool isTrue() const override;

        [[nodiscard]] std::string getTypeString() const override;

        dusl::FResult add_to(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult sub_by(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult multiplied_by(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult divided_by(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult modulo_by(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult greater_than(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult less_than(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult greater_or_equal(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult less_or_equal(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult equal_to(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult not_equal_to(const std::shared_ptr<Object> &other, const Token &token) override;
        dusl::FResult unary_plus(const Token &token) override;
        dusl::FResult unary_minus(const Token &token) override;

    private:
        // functions
        static dusl::FResult
        to_string(FloatObject &floating, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
    };


    class ListObject : public Object {
    public:
        std::vector<std::shared_ptr<Object>> items;

        explicit ListObject(Token tok) : Object(std::move(tok)) {
            functions["set"] = reinterpret_cast<PropertyFunction>(ListObject::set);
            functions["push"] = reinterpret_cast<PropertyFunction>(ListObject::push);
            functions["get"] = reinterpret_cast<PropertyFunction>(ListObject::get);
            functions["forEach"] = reinterpret_cast<PropertyFunction>(ListObject::for_each);
            functions["pop"] = reinterpret_cast<PropertyFunction>(ListObject::pop_back);
        }

        [[nodiscard]] std::string toString() const override;

        dusl::FResult index(std::shared_ptr<ListObject> idx_args, const std::optional<Token> token = std::nullopt) override;
        dusl::FResult index_assign(std::shared_ptr<Object> &right, std::shared_ptr<ListObject> idx_args) override;

        dusl::FResult add_to(const std::shared_ptr<Object>& other, const Token& token) override;

        [[nodiscard]] bool isTrue() const override;
        dusl::FResult getProperty(const std::string &name, const Token &token) override;
        std::string getTypeString() const override;
        static std::shared_ptr<Object> splitAtDelimiter(const std::string &value, const std::string &delim, const Token &tok);

    private:
        // functions
        static dusl::FResult get(ListObject &list, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult
        push(ListObject &list, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult
        set(ListObject &list, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult
        for_each(ListObject &list, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult
        pop_back(ListObject &list, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
    };

    class DictionaryObject : public Object {
    public:
        std::unordered_map<size_t, std::tuple<std::shared_ptr<Object>, std::shared_ptr<Object>>> items;

        DictionaryObject(
                std::unordered_map<size_t, std::tuple<std::shared_ptr<Object>, std::shared_ptr<Object>>> _items,
                Token tok) : Object(std::move(tok)), items(std::move(_items)) {

            functions["exists"] = reinterpret_cast<PropertyFunction>(DictionaryObject::exists);
            functions["get"] = reinterpret_cast<PropertyFunction>(DictionaryObject::get);
            functions["keys"] = reinterpret_cast<PropertyFunction>(DictionaryObject::keys);
            functions["values"] = reinterpret_cast<PropertyFunction>(DictionaryObject::values);
        }

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        dusl::FResult getProperty(const std::string &name, const Token &tok) override;
        dusl::FResult index(std::shared_ptr<ListObject> idx_args, const std::optional<Token> token = std::nullopt) override;
        dusl::FResult index_assign(std::shared_ptr<Object> &right, std::shared_ptr<ListObject> idx_args) override;

        // static
        static dusl::FResult exists(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult get(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult keys(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult values(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);

    };

    class BooleanObject : public Object {
    public:
        bool value;

        BooleanObject(bool _value, Token tok) : Object(std::move(tok)), value(_value) {}

        [[nodiscard]] dusl::FResult hash(const dusl::Token &token) const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] bool isTrue() const override;
    };

    class ArgumentObject : public Object {
    public:
        std::vector<std::shared_ptr<dusl::Object>> args {};
        std::unordered_map<std::string, std::shared_ptr<dusl::Object>> default_args {};


        ArgumentObject(
            std::vector<std::shared_ptr<dusl::Object>> _args,
            std::unordered_map<std::string, std::shared_ptr<dusl::Object>> _default_args,
            Token tok
        ): Object(std::move(tok)), args(std::move(_args)), default_args(std::move(_default_args)) {}
        [[nodiscard]] std::string toString() const override;

        ArgumentObject(
            std::vector<std::shared_ptr<dusl::Object>> _args,
            Token tok
        ): Object(std::move(tok)), args(std::move(_args)) {}

        ArgumentObject(
            std::unordered_map<std::string, std::shared_ptr<dusl::Object>> _default_args,
            Token tok
        ): Object(std::move(tok)), default_args(std::move(_default_args)) {}


    };

    class FunctionObject : public Object {
    public:
        std::string func_name;
        bool is_anon = false;

//        std::shared_ptr<ArgumentObject> args_node;
        std::vector<std::string> args;
        std::unordered_map<std::string, std::shared_ptr<dusl::Object>> default_args;

        std::shared_ptr<BlockNode> block;

        FunctionObject(
            std::string _func_name,
            std::vector<std::string> _args,
            std::unordered_map<std::string, std::shared_ptr<dusl::Object>> _default_args,
            std::shared_ptr<BlockNode> _block,
            Token tok,
            bool _is_anon = false
        ) : Object(std::move(tok)), func_name(std::move(_func_name)), args(std::move(_args)), default_args(std::move(_default_args)), block(std::move(_block)),
            is_anon(_is_anon) {}


        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] dusl::FResult hash(const dusl::Token &token) const override;
        dusl::FResult
        call(Interpreter &visitor, ArgumentObject &args, const Token &token) override;
    };

    class BuiltinFunctionObject : public Object {
    public:
        typedef dusl::FResult (*FunctionPointer)(Interpreter &, ArgumentObject &,
                                                 const dusl::Token &tok);

        FunctionPointer body_func;

        BuiltinFunctionObject(Token tok, FunctionPointer body) : Object(std::move(tok)), body_func(body) {}
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] dusl::FResult hash(const dusl::Token &token) const override;

        dusl::FResult
        call(Interpreter &visitor, ArgumentObject &args_node, const Token &token) override;
    };

    class ErrorObject : public Object {
    public:
        ErrorType err_type;
        std::string err_msg;
        const Position pos;

        ErrorObject(ErrorType _err_type, std::string _err_msg, const Position _pos, Token tok) :
                Object(std::move(tok)), err_type(_err_type), err_msg(std::move(_err_msg)), pos(_pos) {}

        [[nodiscard]] std::string toString() const override;
        std::string generateErrString() const;
    };

    class BreakObject : public Object {
    public:
        explicit BreakObject(Token tok) : Object(std::move(tok)) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        bool isBreak() const override;
    };

    class RangeObject : public Object {
    public:
        int64_t start;
        int64_t end;

        RangeObject(int64_t _start, int64_t _end,Token tok): Object(std::move(tok)), start(_start), end(_end) {
            functions["toList"] = reinterpret_cast<PropertyFunction>(RangeObject::to_list);
        }
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;

        // static
        static dusl::FResult to_list(RangeObject &list, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);

    };

    class StructObject : public Object {
    public:
        std::string name;
        std::unordered_map<std::string, std::shared_ptr<Object>> properties;

        StructObject(std::string _name, std::unordered_map<std::string, std::shared_ptr<Object>> _properties, Token tok): Object(std::move(tok)), name(std::move(_name)), properties(std::move(_properties)) {}
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        dusl::FResult call(Interpreter &visitor, ArgumentObject &args_node, const Token &token) override;
    };

    class StructInstanceObject : public Object {
    public:
        std::string name;
        std::unordered_map<std::string, std::shared_ptr<Object>> properties;

        StructInstanceObject(std::string _name, std::unordered_map<std::string, std::shared_ptr<Object>> _properties, Token tok): Object(std::move(tok)), name(std::move(_name)), properties(std::move(_properties)) {}
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        dusl::FResult getProperty(const std::string &name, const Token &tok) override;
        dusl::FResult callProperty(Interpreter &visitor, const std::shared_ptr<dusl::FunctionCallNode> fn_node) override;
    };

}