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
#include <tuple>

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

        /**
        * @brief Creates a result object with encapsulated result
        *
        * @param _result object
        * @param tok token that will be used to create NoneObject.
        * @return int dusl::FResult Object
        */
        static dusl::FResult createResult(const std::shared_ptr<Object> &_result, const Token &tok);

        /**
        * @brief Creates a result that returns NoneObject
        *
        * @param tok token that will be used to create NoneObject.
        * @return int dusl::FResult Object
        */
        static dusl::FResult createResult(const Token &tok);

        [[nodiscard]] bool isError() const;
        [[nodiscard]] std::string toString() const;

    private:
        bool is_error = false;
    };


    // Base class for the objects that can be returned from visit functions
    class Object {
    public:
        virtual ~Object() = default;

        using PropertyFunction = dusl::FResult (*)(Object &, Interpreter &,
                                                   const std::shared_ptr<FunctionCallNode> &);
        std::unordered_map<std::string, PropertyFunction> functions;

        Token tok;
        std::string doc_str;

        // TODO: changed Token tok > const Token& tok
        explicit Object(const Token& tok, std::string _doc_str="") : tok(tok), doc_str(std::move(_doc_str)) {}

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
    class NoneObject final: public Object {
    public:
        explicit NoneObject(Token tok) : Object(std::move(tok)) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;
    };

    class ReturnObject final: public Object {
    public:
        std::shared_ptr<Object> return_obj;

        ReturnObject(std::shared_ptr<Object> _return_obj, Token tok) : Object(std::move(tok)),
                                                                       return_obj(std::move(_return_obj)) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] bool isTrue() const override;
        [[nodiscard]] std::string getTypeString() const override;
        [[nodiscard]] bool isReturn() const override;
    };

    class StringObject final: public Object {
    public:
        std::string value;

        explicit StringObject(std::basic_string<char> val, Token tok) : Object(tok), value(std::move(val)) {
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
        static dusl::FResult find(StringObject &str, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);

    };

    class NumberError {
    public:
        static dusl::FResult throwZeroDivisionError(const Token &leftToken, const std::shared_ptr<Object> &other);
    };

    class IntObject final: public Object {
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

    class FloatObject final: public Object {
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

    class BooleanObject final: public Object {
    public:
        bool value;

        BooleanObject(const bool _value, const Token &tok) : Object(tok), value(_value) {}

        [[nodiscard]] dusl::FResult hash(const dusl::Token &token) const override;
        [[nodiscard]] std::string toString() const override;
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
    };


    class ListObject final: public Object {
    public:
        std::vector<std::shared_ptr<Object>> items;

        explicit ListObject(const Token &tok) : Object(tok) {
            functions["set"] = reinterpret_cast<PropertyFunction>(ListObject::set);
            functions["push"] = reinterpret_cast<PropertyFunction>(ListObject::push);
            functions["get"] = reinterpret_cast<PropertyFunction>(ListObject::get);
            functions["forEach"] = reinterpret_cast<PropertyFunction>(ListObject::for_each);
            functions["pop"] = reinterpret_cast<PropertyFunction>(ListObject::pop_back);
            functions["map"] = reinterpret_cast<PropertyFunction>(ListObject::map);
        }

        [[nodiscard]] std::string toString() const override;

        dusl::FResult index(std::shared_ptr<ListObject> idx_args, std::optional<Token> token = std::nullopt) override;
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
        static dusl::FResult
        map(ListObject &list, Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
    };

    class DictionaryObject final: public Object {
    public:

        // hashed key as key, key object, value object
        std::unordered_map<size_t, std::tuple<std::shared_ptr<Object>, std::shared_ptr<Object>>> items;

        DictionaryObject(
                std::unordered_map<size_t, std::tuple<std::shared_ptr<Object>, std::shared_ptr<Object>>> _items,
                const Token& tok);

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        dusl::FResult getProperty(const std::string &name, const Token &tok) override;
        dusl::FResult callProperty(Interpreter &visitor, const std::shared_ptr<dusl::FunctionCallNode> fn_node) override;
        dusl::FResult index(std::shared_ptr<ListObject> idx_args, const std::optional<Token> token = std::nullopt) override;
        dusl::FResult index_assign(std::shared_ptr<Object> &right, std::shared_ptr<ListObject> idx_args) override;

        // static
        static dusl::FResult exists(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult get(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult keys(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);
        static dusl::FResult values(DictionaryObject &dict, dusl::Interpreter &visitor, const std::shared_ptr<FunctionCallNode> &fn_node);

        static dusl::FResult loadJson(std::string& jsn, dusl::Interpreter &visitor, const Token &tok);
    };

    class ArgumentObject final: public Object {
    public:
        std::vector<std::shared_ptr<dusl::Object>> args {};
        std::unordered_map<std::string, std::shared_ptr<dusl::Object>> default_args {};

        ArgumentObject(
            std::vector<std::shared_ptr<dusl::Object>> _args,
            std::unordered_map<std::string, std::shared_ptr<dusl::Object>> _default_args,
            const Token& tok
        ): Object(tok), args(std::move(_args)), default_args(std::move(_default_args)) {}
        [[nodiscard]] std::string toString() const override;

        ArgumentObject(
            std::vector<std::shared_ptr<dusl::Object>> _args,
            const Token& tok
        ): Object(tok), args(std::move(_args)) {}

        ArgumentObject(
            std::unordered_map<std::string, std::shared_ptr<dusl::Object>> _default_args,
            const Token& tok
        ): Object(tok), default_args(std::move(_default_args)) {}

    };

    class FunctionObject final: public Object {
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
            const Token& tok,
            bool _is_anon = false
        );


        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] dusl::FResult hash(const dusl::Token &token) const override;
        dusl::FResult
        call(Interpreter &visitor, ArgumentObject &args, const Token &token) override;
    };

    class BuiltinFunctionObject final: public Object {
    public:
        typedef dusl::FResult (*FunctionPointer)(Interpreter &, ArgumentObject &,
                                                 const dusl::Token &tok);

        FunctionPointer body_func;

        BuiltinFunctionObject(const Token& tok, FunctionPointer body, std::string _doc_str="") : Object(tok, std::move(_doc_str)), body_func(body){}
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] dusl::FResult hash(const dusl::Token &token) const override;

        dusl::FResult
        call(Interpreter &visitor, ArgumentObject &args_node, const Token &token) override;

    };

    class ErrorObject final: public Object {
    public:
        ErrorType err_type;
        std::string err_msg;
        const Position pos;

        ErrorObject(ErrorType _err_type, std::string _err_msg, const Position _pos, Token tok) :
                Object(std::move(tok)), err_type(_err_type), err_msg(std::move(_err_msg)), pos(_pos) {}

        [[nodiscard]] std::string toString() const override;
        std::string generateErrString() const;
    };

    class BreakObject final: public Object {
    public:
        explicit BreakObject(Token tok) : Object(std::move(tok)) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        bool isBreak() const override;
    };

    class RangeObject final: public Object {
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


    // Kinda unstable
    class StructObject final: public Object {
    public:
        std::string name;
        std::unordered_map<std::string, std::shared_ptr<Object>> properties;

        StructObject(std::string _name, std::unordered_map<std::string, std::shared_ptr<Object>> _properties, Token tok): Object(std::move(tok)), name(std::move(_name)), properties(std::move(_properties)) {}
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        dusl::FResult call(Interpreter &visitor, ArgumentObject &args_node, const Token &token) override;
    };

    class StructInstanceObject final: public Object {
    public:
        std::string name;
        std::unordered_map<std::string, std::shared_ptr<Object>> properties;

        StructInstanceObject(std::string _name, std::unordered_map<std::string, std::shared_ptr<Object>> _properties, Token tok): Object(std::move(tok)), name(std::move(_name)), properties(std::move(_properties)) {}
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        dusl::FResult getProperty(const std::string &name, const Token &tok) override;
        dusl::FResult callProperty(Interpreter &visitor, const std::shared_ptr<dusl::FunctionCallNode> fn_node) override;
    };

    // TODO: temporarily here
    class DBaseStruct {
    public:
        virtual ~DBaseStruct() = default;

        struct StructCreationResult {
            bool is_error = false;
            std::shared_ptr<DBaseStruct> result;
            std::shared_ptr<ErrorObject> err;

            explicit StructCreationResult(std::shared_ptr<DBaseStruct> _result): result(std::move(_result)), is_error(false) {}
            StructCreationResult(ErrorType err_type, std::string msg, const Token& tok): err(std::make_shared<ErrorObject>(err_type, msg, tok.pos, tok)), is_error(true) {}
        };

        DBaseStruct() = default;
        static StructCreationResult init_(dusl::ArgumentObject& args_node);

        virtual std::string getTypeString() = 0;

        virtual std::string getClassName() { return className; }
        virtual bool isTrue() { return true; };
        virtual void setClassName(const std::string& name) { className = name; };
    private:
        std::string className = "DBaseStruct";
    };


    using ExposedFunctions = dusl::FResult(*)(std::shared_ptr<DBaseStruct>, Interpreter&,
        const std::shared_ptr<FunctionCallNode>&);

    using CreatorFunction = DBaseStruct::StructCreationResult(*)(ArgumentObject&);
    //using CreatorFunction = std::function<std::shared_ptr<DBaseStruct>(ArgumentObject&)>;


    class StructProxyObject final: public Object {
    public:
        std::string cls_name;
        // the class that has inherited DBaseStruct
        //std::shared_ptr<DBaseStruct> cls_ref;

        CreatorFunction& creator_func;
        std::map<std::string, ExposedFunctions> exp_funcs;

        StructProxyObject(std::string _cls_name, CreatorFunction& _creator_func, const Token& tok, std::string _doc_str="") : Object(tok, std::move(_doc_str)), cls_name(std::move(_cls_name)), creator_func(_creator_func) {}

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;

        dusl::FResult call(Interpreter& visitor, ArgumentObject& args_node, const Token& token) override;

        // takes in a static function that has the same signature as ExposedFunctions
        void addFunc(const std::string& name, ExposedFunctions func);
    };

    class StructProxyInstanceObject final: public Object {
    public:
        std::string cls_name;
        // the class that has inherited DBaseStruct
        std::shared_ptr<DBaseStruct> cls_ref;
        std::map<std::string, ExposedFunctions> exp_funcs;

        StructProxyInstanceObject(
            std::string _cls_name, 
            std::shared_ptr<DBaseStruct> _cls_ref, 
			std::map<std::string, ExposedFunctions> _exp_funcs,
            const Token& tok
        ) : Object(tok), cls_name(std::move(_cls_name)), cls_ref(std::move(_cls_ref)),   exp_funcs(std::move(_exp_funcs)) { }

        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string getTypeString() const override;
        dusl::FResult callProperty(Interpreter& visitor, std::shared_ptr<dusl::FunctionCallNode> fn_node) override;
      
    };

}