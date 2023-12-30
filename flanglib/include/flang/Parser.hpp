#pragma once
#include <vector>
#include "Lexer.hpp"
#include "DataNodes.hpp"
#include <optional>
#include <memory>

namespace flang {

    class Parser {

    public:
        Parser();
        void eat(TokenType tt);
        flang::ProgramNode parse(std::string_view code, std::string_view file_name, const std::vector<Token> &tokens);
        flang::ProgramNode program();

        DataNode * block();
        DataNode * statement();
        DataNode * expression();
        DataNode * comparison();
        DataNode * dotExpr();
        DataNode * factor();
        DataNode * term();
        DataNode * atom();
        DataNode * parseUnary();
        DataNode * funcCall();
        DataNode * funcDef();
        DataNode * structDef();
        DataNode * bracketExpr();
        DataNode * ifStatement();
        DataNode * whileStatement();
        DataNode * returnStatement();

        BlockNode * scopeBlock(std::optional<Token> tok = std::nullopt);
        flang::ConditionNode capture_condition(Token tok);

        // helpers
        DataNode * parseAssignment();
        DataNode * parseStructBody();
        DataNode * parseList();
        DataNode * parseDict();

        std::vector<std::shared_ptr<flang::DataNode>> argumentParser();
        std::optional<Token> peek();

    private:
        std::vector<Token> m_toks;
        Token m_token;
        int m_tok_idx = 0;
        std::string_view m_code;
        std::string_view m_file_name;
    };

}
