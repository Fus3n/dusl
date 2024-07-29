#pragma once
#include <vector>
#include "Lexer.hpp"
#include "DataNodes.hpp"
#include <optional>
#include <memory>

namespace dusl {

    class Parser {

    public:
        Parser();
        void eat(TokenType tt);
        dusl::ProgramNode parse(const std::string& code, const std::string& file_name, const std::vector<Token> &tokens);
        dusl::ProgramNode program();

        DataNode * block();
        DataNode * statement();
        DataNode * expression();
        DataNode * comparison();
        DataNode * rangeExpr();
        DataNode * postExpr();
        DataNode * preExpr();
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
        DataNode * forStatement();
        DataNode * returnStatement();
        DataNode * importStatement();

        BlockNode * scopeBlock(std::optional<Token> tok = std::nullopt);
        dusl::ConditionNode capture_condition(Token tok);

        // helpers
        DataNode * parseAssignment();
        DataNode * parseStructBody();
        dusl::ArgumentNode parseFuncArgument(bool is_define=false, Token _tok = Token());
        ListNode * parseList();
        DataNode * parseDict();

        std::vector<std::shared_ptr<dusl::DataNode>> argumentParser();
        std::optional<Token> peek();

    private:
        std::vector<Token> m_toks;
        Token m_token;
        int m_tok_idx = 0;
        std::string_view m_code = "";
        std::string_view m_file_name = "";
    };

}
