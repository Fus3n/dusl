#include "flang/Parser.hpp"
#include <fmt/core.h>
#include "flang/ErrorType.hpp"
#include "flang/LanguageManager.hpp"

#include <utility>

flang::Parser::Parser() = default;

void flang::Parser::eat(TokenType tt) {
    if (m_token.tok == tt) {
        if (m_tok_idx < m_toks.size()) {
            m_token = m_toks[m_tok_idx];
            m_tok_idx++;
        }
        else {
            m_token = Token(); // EOF token
        }
    } else {
        FError(SyntaxError,
                fmt::format("Invalid Syntax {}, expected {}", tokToString(m_token.tok), tokToString(tt)),
                m_token.pos
                ).Throw();

    }
}

flang::ProgramNode flang::Parser::parse(std::string_view code, std::string_view file_name, const std::vector<Token> &tokens) {
    m_toks = tokens;
    // move the cursor to the first token
    m_token = m_toks[m_tok_idx];
    m_tok_idx++;
    m_code = code;
    m_file_name = file_name;

    auto ast = program();
    return ast;
}

flang::ProgramNode flang::Parser::program() {
    auto program = ProgramNode(this->m_token);
    program.statements.emplace_back(block());
    return program;
}

flang::DataNode * flang::Parser::statement() {
    if (m_token.cmp(TokenType::Keyword)) {
        if (m_token.value == LanguageManager::getValue(FuncDefKey)) {
            return funcDef();
        } else if (m_token.value == LanguageManager::getValue(StructKey)) {
            return structDef();
        } else if (m_token.value == LanguageManager::getValue(IfKey)) {
            return ifStatement();
        } else if (m_token.value == LanguageManager::getValue(ReturnKey)) {
            return returnStatement();
        } else if (m_token.value == LanguageManager::getValue(WhileKey)) {
            return whileStatement();
        } else {
            m_token.pos.setCodeAndFile(m_code, m_file_name);
            FError(UnimplementedError,
                   fmt::format("Keyword \"{}\", is not yet supported", m_token.value),
                   m_token.pos
            ).Throw();
        }
    }

    return expression();
}

flang::DataNode * flang::Parser::block() {
    auto block = new BlockNode(this->m_token);

    while (m_token.tok != Eof) {
        block->statements.emplace_back(statement());
    }

    return block;
}
flang::BlockNode * flang::Parser::scopeBlock(std::optional<Token> tok)  {
    auto block = new BlockNode(tok.has_value() ? tok.value() : this->m_token);
    eat(LBrace);
    while (!m_token.cmp(Eof) && !m_token.cmp(RBrace)) {
        block->statements.emplace_back(statement());
    }
    eat(RBrace);
    return block;
}

flang::DataNode * flang::Parser::expression() {
    DataNode* left = comparison(); // comparison returns a DataNode*

    while (
            m_token.cmp(TokenType::Keyword, LanguageManager::getValue(KeywordType::AndKey)) |
            m_token.cmp(TokenType::Keyword, LanguageManager::getValue(KeywordType::OrKey))
            ) {
        auto op = m_token;
        eat(op.tok);
        auto right = comparison();

        if (op.value == LanguageManager::getValue(KeywordType::AndKey)) {
            left = new LogicalOpNode(left, right, LogicalOpNode::OP_AND, op);
        } else {
            left = new LogicalOpNode(left, right, LogicalOpNode::OP_OR, op);
        }
    }

    return left;
}

flang::DataNode * flang::Parser::comparison() {
    DataNode* left = atom();

    while (
            m_token.cmp(TokenType::GreaterThan) |
            m_token.cmp(TokenType::LessThan) |
            m_token.cmp(TokenType::GreaterOrEqual) |
            m_token.cmp(TokenType::LessOrEqual) |
            m_token.cmp(TokenType::EqualTo) |
            m_token.cmp(TokenType::NotEqualTo)
            ) {

        auto op = m_token;
        eat(op.tok);
        auto right = atom();
        left = new BinOpNode(left, right, BinOpNode::TokToOperation(op), op);
    }

    return left;
}

flang::DataNode * flang::Parser::atom() {
    DataNode* left = term();

    while (m_token.tok == TokenType::Plus || m_token.tok == TokenType::Minus) {
        auto op = m_token;
        eat(op.tok);
        auto right = term();
        left = new BinOpNode(left, right, BinOpNode::TokToOperation(op), op);
    }

    return left;
}

flang::DataNode * flang::Parser::term() {
    DataNode* left = dotExpr();

    while (m_token.tok == TokenType::Multiply || m_token.tok == TokenType::Divide || m_token.tok == TokenType::Modulo) {
        auto op = m_token;
        eat(op.tok);
        auto right = dotExpr();
        left = new BinOpNode(left, right, BinOpNode::TokToOperation(op), op);
    }

    return left;
}

flang::DataNode * flang::Parser::dotExpr() {
    DataNode* left = factor();

    while (m_token.cmp(TokenType::Dot)) {
        auto op = m_token;
        eat(op.tok);
        auto right = factor();
        left = new MemberAccessNode(left, right, op);
    }

    return left;
}

flang::DataNode * flang::Parser::factor() {
    Token curr_tok = m_token;

    if (m_token.tok == TokenType::STRING) {
        auto value = m_token.value;
        eat(TokenType::STRING);
        return new StringNode(value, curr_tok);
    } else if (m_token.tok == TokenType::INT) {
        auto value = m_token.value;
        eat(TokenType::INT);
        return new IntNode(std::stoi(value), curr_tok);
    } else if (m_token.tok == TokenType::FLOAT) {
        auto value = m_token.value;
        eat(TokenType::FLOAT);
        return new FloatNode(std::stold(value), curr_tok);
    } else if (m_token.tok == TokenType::Ident) {
        auto peeked = peek();
        if (peeked.has_value() && peeked.value().cmp(flang::LBracket)) {
            return funcCall(); // TODO: Might need to move it out from here
        } else if (peeked.has_value() && peeked.value().cmp(flang::Equal)) {
            return parseAssignment();
        }
        eat(flang::Ident);
        return new VarAccessNode(curr_tok);
    } else if (m_token.tok == TokenType::LBracket) {
        return bracketExpr();
    } else if (m_token.cmp(TokenType::LSqrBracket)) {
        return parseList();
    } else if (m_token.cmp(TokenType::Plus) || m_token.cmp(TokenType::Minus)) {
        return parseUnary();
    } else if (m_token.cmp(TokenType::Keyword) && m_token.value == LanguageManager::getValue(NotKey)) {
        auto op = m_token;
        eat(Keyword);
        auto expr = expression();
        return new UnaryOpNode(expr, UnaryOpNode::Operations::OP_NOT, op);
    } else if (m_token.cmp(TokenType::LBrace)) {
        return parseDict();
    }
    m_token.pos.setCodeAndFile(m_code, m_file_name);
    FError(SyntaxError,
           fmt::format("Unexpected expression \"{}\"", m_token.ToString()),
           m_token.pos
    ).Throw();

    return new NoneNode(curr_tok);
}

flang::DataNode * flang::Parser::bracketExpr() {
    this->eat(TokenType::LBracket);
    auto innerExpr = expression();
    this->eat(TokenType::RBracket);
    return innerExpr;
}

flang::DataNode * flang::Parser::funcCall() {
    auto func_name = m_token;
    eat(Ident);
    auto arguments = argumentParser();
    return new FunctionCallNode(func_name, std::move(arguments));
}

flang::DataNode * flang::Parser::funcDef() {
    eat(Keyword); // skip "fn" keyword

    auto func_name = m_token;
    eat(Ident);

    auto arguments = argumentParser();
    auto block = scopeBlock(func_name);
    return new FunctionDefNode(func_name, std::move(arguments), block);
}

std::vector<std::shared_ptr<flang::DataNode>> flang::Parser::argumentParser() {

    // TODO: IMPORTANT, Update it to send back a new node called ArgumentNode and have way to check for arithmetics,
    // which might be needed for positional arguments
    std::vector<std::shared_ptr<DataNode>> arguments;

    eat(LBracket);
    // collect the arguments inside bracket
    while (!m_token.cmp(Eof) && !m_token.cmp(RBracket)) {
        arguments.emplace_back(expression());

        if (m_token.cmp(Comma)) {
            eat(Comma);
        }
    }
    eat(RBracket);
    return arguments;
}

std::optional<flang::Token> flang::Parser::peek() {
    if (m_tok_idx < m_toks.size()) {
        return m_toks[m_tok_idx];
    }
    return {};
}

flang::DataNode * flang::Parser::parseList() {
    auto curr_tok = m_token;
    std::vector<std::shared_ptr<DataNode>> items;
    eat(LSqrBracket);

    while (!m_token.cmp(Eof) && !m_token.cmp(RSqrBrack)) {
        items.emplace_back(expression());

        if (m_token.cmp(Comma)) {
            eat(Comma);
        }
    }
    eat(RSqrBrack);
    return new ListNode(std::move(items), curr_tok);
}

flang::DataNode * flang::Parser::parseAssignment() {
    auto curr_tok = m_token;
    eat(flang::Ident);
    eat(flang::Equal);
    auto expr = expression();

    return new AssignmentNode(expr, curr_tok);
}

flang::DataNode * flang::Parser::structDef() {
    eat(Keyword);

    auto struct_name = m_token;
    eat(Ident);

    eat(LBrace);
    auto struct_def = new StructDefNode(struct_name);
    while (!m_token.cmp(Eof) && !m_token.cmp(RBrace)) {
        struct_def->values.emplace_back(parseStructBody());
    }
    eat(RBrace);

    return struct_def;
}

flang::DataNode* flang::Parser::parseStructBody() {
    // Allow what a struct body should have
    if (m_token.cmp(TokenType::Ident)) {
        auto peeked = peek();
        if (peeked.has_value() && peeked.value().cmp(flang::Equal)) {
            return parseAssignment();
        } else {
            m_token.pos.setCodeAndFile(m_code, m_file_name);
            FError(SyntaxError,
                   fmt::format("Invalid Syntax: {} not allowed inside struct body", flang::tokToString(m_token.tok)),
                   m_token.pos
            ).Throw();
        }
    } else if (m_token.value == LanguageManager::getValue(FuncDefKey)) {
        return funcDef();
    }

    m_token.pos.setCodeAndFile(m_code, m_file_name);
    FError(UnimplementedError,
           fmt::format("Keyword \"{}\" is not yet supported", m_token.value),
           m_token.pos
    ).Throw();

    return new NoneNode(m_token);
}

flang::ConditionNode flang::Parser::capture_condition(Token tok) {
    auto cond_node = expression();
    auto body_node = scopeBlock();
    return ConditionNode(cond_node, body_node, std::move(tok));
}

flang::DataNode * flang::Parser::ifStatement() {
    auto if_tok = m_token;
    auto if_node = new IFNode(if_tok, ConditionNode(flang::Token()));
    auto cond_tok = m_token;
    eat(Keyword); // eat "if"

    auto cond_node = expression();
    auto body_cond_node = scopeBlock();
    if_node->cond_node = ConditionNode(cond_node, body_cond_node, cond_tok);

    while (!m_token.cmp(Eof) && m_token.cmp(TokenType::Keyword) && m_token.value == LanguageManager::getValue(ElseIfKey)) {
        auto key_tok = m_token;
        eat(Keyword); // eat "elseif"
        if_node->else_ifs.push_back(capture_condition(key_tok));
    }

    if (m_token.cmp(TokenType::Keyword)) {
        if (m_token.value == LanguageManager::getValue(ElseKey)) {
            eat(Keyword); // eat "else"
            if_node->else_node = std::shared_ptr<BlockNode>(scopeBlock());
        }
    }

    return if_node;
}

flang::DataNode * flang::Parser::returnStatement() {
    auto return_tok = m_token;
    eat(Keyword); // skip "return" keyword
    auto exprToReturn = expression();
    return new ReturnNode(exprToReturn, return_tok);
}

flang::DataNode * flang::Parser::parseUnary() {
    auto op = m_token;
    eat(op.tok);
    auto expr = expression();
    return new UnaryOpNode(expr, UnaryOpNode::TokToOperation(op), op);
}

flang::DataNode * flang::Parser::whileStatement() {
    auto tok = m_token;
    eat(Keyword); // skip "while" keyword

    auto cond_expr = expression();
    auto body_scope = scopeBlock();
    auto cond_node = ConditionNode(cond_expr, body_scope, tok);

    return new WhileLoopNode(std::move(cond_node), tok);
}

flang::DataNode *flang::Parser::parseDict() {
   auto curr_tok = m_token;
   eat(LBrace);
   DictionaryNode::KeyValTuples items = {};

    while (!m_token.cmp(Eof) && !m_token.cmp(RBrace)) {
        auto first = expression();
        eat(Colon);
        auto second = expression();
        items.emplace_back(std::make_pair(first, second));

        if (m_token.cmp(RBrace))
            break;
        else if (m_token.cmp(Comma))
            eat(Comma);
    }

    eat(RBrace);
    return new DictionaryNode(std::move(items), curr_tok);
}


