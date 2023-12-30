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
        std::shared_ptr<flang::ProgramNode> parse(std::string_view code, std::string_view file_name, const std::vector<Token> &tokens);
        std::shared_ptr<flang::ProgramNode> program();
        std::shared_ptr<flang::DataNode> block();
        std::shared_ptr<flang::DataNode> statement();
        std::shared_ptr<flang::DataNode> expression();
        std::shared_ptr<flang::DataNode> comparison();
        std::shared_ptr<flang::DataNode> dotExpr();
        std::shared_ptr<flang::DataNode> factor();
        std::shared_ptr<flang::DataNode> term();
        std::shared_ptr<flang::DataNode> atom();
        std::shared_ptr<flang::DataNode> parseUnary();
        std::shared_ptr<flang::DataNode> funcCall();
        std::shared_ptr<flang::DataNode> funcDef();
        std::shared_ptr<flang::DataNode> structDef();
        std::shared_ptr<flang::DataNode> bracketExpr();
        std::shared_ptr<flang::DataNode> ifStatement();
        std::shared_ptr<flang::DataNode> whileStatement();
        std::shared_ptr<flang::DataNode> returnStatement();

        std::shared_ptr<flang::BlockNode> scopeBlock(std::optional<Token> tok = std::nullopt);
        std::shared_ptr<flang::ConditionNode> capture_condition();

        // helpers
        std::shared_ptr<flang::DataNode> parseAssignment();
        std::shared_ptr<flang::DataNode> parseStructBody();
        std::shared_ptr<flang::DataNode> parseList();

        std::vector<std::shared_ptr<flang::DataNode>> argumentParser();
        std::optional<Token> peek();

    private:
        std::vector<Token> m_toks;
        Token m_token;
        int m_tok_idx = 0;
        std::string_view m_code;
        std::string_view m_file_name;
        std::shared_ptr<DataNode> m_ast;
        
    };

}
