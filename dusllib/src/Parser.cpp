#include <fmt/core.h>
#include <dusl/Parser.hpp>
#include <dusl/ErrorType.hpp>
#include <dusl/LanguageManager.hpp>

#include <utility>

dusl::Parser::Parser() = default;

void dusl::Parser::eat(TokenType tt) {
    if (m_token.tok == tt) {
        if (m_tok_idx < m_toks.size()) {
            m_token = m_toks[m_tok_idx];
            m_tok_idx++;
        }
        else {
            m_token = Token(); // EOF token
        }
    } else {
        DError(SyntaxError,
               fmt::format(R"(Invalid Syntax "{}", expected "{}")", tokToString(m_token.tok), tokToString(tt)),
               m_token.pos
                ).Throw();

    }
}

dusl::ProgramNode dusl::Parser::parse(const std::string& code, const std::string& file_name, const std::vector<Token> &tokens) {
    m_toks = tokens;
    // move the cursor to the first token
    m_token = m_toks[m_tok_idx];
    m_tok_idx++;
    m_code = code;
    m_file_name = file_name;

    auto ast = program();
    return ast;
}

dusl::ProgramNode dusl::Parser::program() {
    auto program = ProgramNode(this->m_token);
    program.statements.emplace_back(block());
    return program;
}

dusl::DataNode * dusl::Parser::statement() {
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
        } else if (m_token.value == LanguageManager::getValue(ForKey)) {
            return forStatement();
        } else if (m_token.value == LanguageManager::getValue(ImportKey)) {
            return importStatement();
        } else if (m_token.value == LanguageManager::getValue(BreakKey)) {
            auto break_tok = m_token;
            eat(TokenType::Keyword);
            eat(TokenType::SemiColon);
            return new BreakNode(break_tok);
        }
        else {
            DError(UnimplementedError,
                   fmt::format("Keyword \"{}\", is not yet supported", m_token.value),
                   m_token.pos
            ).Throw();
        }
    }

    return expression();
}

dusl::DataNode * dusl::Parser::block() {
    auto block = new BlockNode(this->m_token);

    while (m_token.tok != Eof) {
        block->statements.emplace_back(statement());
    }

    return block;
}
dusl::BlockNode * dusl::Parser::scopeBlock(std::optional<Token> tok)  {
    auto block = new BlockNode(tok.has_value() ? tok.value() : this->m_token);
    eat(LBrace);
    while (!m_token.cmp(Eof) && !m_token.cmp(RBrace)) {
        block->statements.emplace_back(statement());
    }
    eat(RBrace);
    return block;
}

dusl::DataNode * dusl::Parser::expression() {
    DataNode* left = rangeExpr();

    while (
            m_token.cmp(TokenType::Keyword, LanguageManager::getValue(KeywordType::AndKey)) ||
            m_token.cmp(TokenType::Keyword, LanguageManager::getValue(KeywordType::OrKey))
            ) {
        auto op = m_token;
        eat(op.tok);
        auto right = rangeExpr();

        if (op.value == LanguageManager::getValue(KeywordType::AndKey)) {
            left = new LogicalOpNode(left, right, LogicalOpNode::OP_AND, op);
        } else {
            left = new LogicalOpNode(left, right, LogicalOpNode::OP_OR, op);
        }
    }

    return left;
}

dusl::DataNode * dusl::Parser::rangeExpr() {
    DataNode* left = comparison();

    if (m_token.cmp(TokenType::DoubleDot)) {
        auto op = m_token;
        eat(op.tok);

        auto right = comparison();

        left = new RangeNode(left, right, op);
    }

    return left;
}

dusl::DataNode * dusl::Parser::comparison() {
    DataNode* left = atom();

    while (
            m_token.cmp(TokenType::GreaterThan) ||
            m_token.cmp(TokenType::LessThan) ||
            m_token.cmp(TokenType::GreaterOrEqual) ||
            m_token.cmp(TokenType::LessOrEqual) ||
            m_token.cmp(TokenType::EqualTo) ||
            m_token.cmp(TokenType::NotEqualTo)
            ) {

        auto op = m_token;
        eat(op.tok);
        auto right = atom();
        left = new BinOpNode(left, right, BinOpNode::TokToOperation(op), op);
    }

    return left;
}

dusl::DataNode * dusl::Parser::atom() {
    DataNode* left = term();

    while (m_token.tok == TokenType::Plus || m_token.tok == TokenType::Minus) {
        auto op = m_token;
        eat(op.tok);
        auto right = term();
        left = new BinOpNode(left, right, BinOpNode::TokToOperation(op), op);
    }

    return left;
}

dusl::DataNode * dusl::Parser::term() {
    DataNode* left = preExpr();

    while (m_token.tok == TokenType::Multiply || m_token.tok == TokenType::Divide || m_token.tok == TokenType::Modulo) {
        auto op = m_token;
        eat(op.tok);
        auto right = preExpr();
        left = new BinOpNode(left, right, BinOpNode::TokToOperation(op), op);
    }

    return left;
}

dusl::DataNode * dusl::Parser::preExpr() {
    DataNode* left = nullptr;

    while (m_token.cmp(TokenType::Keyword) && m_token.value == LanguageManager::getValue(NotKey)) {
        auto op = m_token;
        eat(Keyword);
        auto expr = postExpr();
        left = new UnaryOpNode(expr, UnaryOpNode::Operations::OP_NOT, op);
    }

    if (!left)
        return postExpr();
    else
        return left;
}

dusl::DataNode * dusl::Parser::postExpr() {
    DataNode* left = factor();

    while (m_token.cmp(TokenType::LBracket) || m_token.cmp(TokenType::LSqrBracket) || m_token.cmp(TokenType::Dot)) {
        auto tok = m_token;
        if (tok.cmp(TokenType::LBracket)) {
            auto args = parseFuncArgument();
            left = new FunctionCallNodeEXPR(left, args, tok);
        } else if (tok.cmp(TokenType::LSqrBracket)) {
            auto indexArgs = parseList();

            if (m_token.cmp(TokenType::Equal)) {
                eat(Equal);
                auto right = expression(); // TODO: possibly needs to change to "expression"
                left = new IndexAssignNode(left, right, indexArgs, tok);
            } else {
                left = new IndexNode(left, indexArgs, tok);
            }
        } else if (tok.cmp(TokenType::Dot)) {
            auto op = m_token;
            eat(op.tok);

            auto right = factor();
            left = new MemberAccessNode(left, right, op);
        }
    }

    return left;
}

dusl::DataNode * dusl::Parser::factor() {
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
        if (peeked.has_value() && peeked.value().cmp(dusl::LBracket)) {
            return funcCall(); // TODO: Might need to move it out from here
        } else if (peeked.has_value() && peeked.value().cmp(dusl::Equal)) {
            return parseAssignment();
        }
        eat(dusl::Ident);
        return new VarAccessNode(curr_tok);
    } else if (m_token.tok == TokenType::LBracket) {
        return bracketExpr();
    } else if (m_token.cmp(TokenType::LSqrBracket)) {
        return parseList();
    } else if (m_token.cmp(TokenType::Plus) || m_token.cmp(TokenType::Minus)) {
        return parseUnary();
    }
    else if (m_token.cmp(TokenType::LBrace)) {
        return parseDict();
    } else if (m_token.value == LanguageManager::getValue(FuncDefKey)) {
        return funcDef();
    }

    DError(SyntaxError,
           fmt::format("Unexpected expression \"{}\"", m_token.toString()),
           m_token.pos
    ).Throw();

    return new NoneNode(curr_tok);
}

dusl::DataNode * dusl::Parser::bracketExpr() {
    auto tok = m_token;
    eat(TokenType::LBracket);
    auto xp = expression();
    eat(TokenType::RBracket);
    return xp;
}

dusl::DataNode * dusl::Parser::funcCall() {
    auto func_name = m_token;
    eat(Ident);
    auto arguments = parseFuncArgument(false, func_name);
    return new FunctionCallNode(func_name, arguments);
}

dusl::DataNode * dusl::Parser::funcDef() {
    auto tok = m_token;
    eat(Keyword); // skip "fn" keyword

    std::string func_name = m_token.value;
    bool is_anon = false;
    if (m_token.cmp(Ident)) {
        eat(Ident);
    } else {
        func_name = "anonymous";
        is_anon = true;
    }

    auto arguments = parseFuncArgument(true);
    auto block = scopeBlock(tok);
    return new FunctionDefNode(func_name, arguments, block, tok, is_anon);
}

dusl::ArgumentNode dusl::Parser::parseFuncArgument(bool is_define, Token _tok) {
    Token tok = m_token;
    if (_tok.tok != TokenType::Eof) {
        tok = _tok;
    }

    std::vector<std::shared_ptr<DataNode>> args;
    std::unordered_map<std::string, std::shared_ptr<DataNode>> default_args;

    eat(LBracket);
    // collect the arguments inside bracket

    while (!m_token.cmp(Eof) && !m_token.cmp(RBracket)) {
        auto expr = expression();
        // check if its default argument
        if (auto varAccess = dynamic_cast<VarAccessNode*>(expr)) {
            args.emplace_back(expr);
        } else if (auto assignNode = dynamic_cast<AssignmentNode*>(expr)) {
            default_args.emplace(assignNode->tok.value, assignNode->expr);
        } else if (is_define) {
            DError(SyntaxError,
                   "Invalid argument",
                   expr->tok.pos
            ).Throw();
        } else {
            args.emplace_back(expr);
        }

        if (m_token.cmp(Comma)) {
            eat(Comma);
        }
    }
    eat(RBracket);

    return {std::move(args), std::move(default_args), tok};
}

std::vector<std::shared_ptr<dusl::DataNode>> dusl::Parser::argumentParser() {

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

std::optional<dusl::Token> dusl::Parser::peek() {
    if (m_tok_idx < m_toks.size()) {
        return m_toks[m_tok_idx];
    }
    return {};
}

dusl::ListNode * dusl::Parser::parseList() {
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

dusl::DataNode * dusl::Parser::parseAssignment() {
    auto curr_tok = m_token;
    eat(dusl::Ident);
    eat(dusl::Equal);
    auto expr = expression();

    return new AssignmentNode(expr, curr_tok);
}

dusl::DataNode * dusl::Parser::structDef() {
    eat(Keyword);

    const auto struct_name = m_token;
    eat(Ident);

    eat(LBrace);
    auto struct_def = new StructDefNode(struct_name);
    while (!m_token.cmp(Eof) && !m_token.cmp(RBrace)) {
        struct_def->values.emplace_back(parseStructBody());
    }
    eat(RBrace);

    return struct_def;
}

dusl::DataNode* dusl::Parser::parseStructBody() {
    // Allow what a struct body should have
    if (m_token.cmp(TokenType::Ident)) {
        const auto peeked = peek();
        if (peeked.has_value() && peeked.value().cmp(dusl::Equal)) {
            return parseAssignment();
        } else {
            DError(SyntaxError,
                   fmt::format("Invalid Syntax: {} not allowed inside struct body", dusl::tokToString(m_token.tok)),
                   m_token.pos
            ).Throw();
        }
    } else if (m_token.value == LanguageManager::getValue(FuncDefKey)) {
        return funcDef();
    }

    DError(UnimplementedError,
           fmt::format("Keyword \"{}\" is not yet supported", m_token.value),
           m_token.pos
    ).Throw();

    return new NoneNode(m_token);
}

dusl::ConditionNode dusl::Parser::capture_condition(Token tok) {
    auto cond_node = expression();
    auto body_node = scopeBlock();
    return ConditionNode(cond_node, body_node, std::move(tok));
}

dusl::DataNode * dusl::Parser::ifStatement() {
    auto if_tok = m_token;
    auto if_node = new IFNode(if_tok, ConditionNode(dusl::Token()));
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

dusl::DataNode * dusl::Parser::returnStatement() {
    auto return_tok = m_token;
    eat(Keyword); // skip "return" keyword
    auto exprToReturn = expression();
    return new ReturnNode(exprToReturn, return_tok);
}

dusl::DataNode * dusl::Parser::parseUnary() {
    auto op = m_token;
    eat(op.tok);
    auto expr = expression();
    return new UnaryOpNode(expr, UnaryOpNode::TokToOperation(op), op);
}

dusl::DataNode * dusl::Parser::whileStatement() {
    auto tok = m_token;
    eat(Keyword); // skip "while" keyword

    auto cond_expr = expression();
    auto body_scope = scopeBlock();
    auto cond_node = ConditionNode(cond_expr, body_scope, tok);

    return new WhileLoopNode(std::move(cond_node), tok);
}

dusl::DataNode *dusl::Parser::parseDict() {
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

dusl::DataNode *dusl::Parser::forStatement() {
    auto curr_tok = m_token;
    eat(Keyword); // skip "for" keyword

    auto ident = m_token;
    eat(Ident);

    auto from_tok = m_token;
    eat(Keyword); // skip "from" keyword
    if (from_tok.value != LanguageManager::getValue(FromKey)) {
        DError(SyntaxError,
               fmt::format("Expected keyword \"{}\" ", LanguageManager::getValue(FromKey)),
               from_tok.pos
               ).Throw();
    }

    auto expr = expression();
    auto block = scopeBlock();

    return new ForLoopNode(ident.value, expr, block, curr_tok);
}

dusl::DataNode *dusl::Parser::importStatement() {
    auto import_tok = m_token;
    eat(Keyword); // skip "import" keyword

    // symbols to import
    std::vector<std::string> symbols{};
    std::string module_path;
    bool import_all = false;

    if (m_token.cmp(TokenType::LSqrBracket)) {
        eat(LSqrBracket);
        while (!m_token.cmp(Eof) && !m_token.cmp(RSqrBrack)) {
            auto sym = m_token;
            eat(Ident);
            symbols.push_back(sym.value);
            if (m_token.cmp(RSqrBrack))
                break;
            else if (m_token.cmp(Comma))
                eat(Comma);
        }
        eat(RSqrBrack);
        if  (!(m_token.cmp(Keyword) && m_token.value == LanguageManager::getValue(FromKey))) {
            DError(SyntaxError,
                   fmt::format("Expected keyword \"{}\" ", LanguageManager::getValue(FromKey)),
                   m_token.pos
            ).Throw();
        }
        eat(Keyword);
        module_path = m_token.value;
        eat(STRING);
    } else {
        module_path = m_token.value;
        eat(STRING);
        import_all = true;
    }

    return new ImportNode(module_path, std::move(symbols), import_all, import_tok);
}

