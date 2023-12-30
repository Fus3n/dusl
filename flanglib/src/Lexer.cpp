#include <fmt/format.h>
#include <string>

#include "flang/Lexer.hpp"
#include "flang/ErrorType.hpp"
#include "flang/LanguageManager.hpp"

#include <algorithm>

std::string flang::tokToString(const TokenType tok) {
	switch (tok) {
		case TokenType::STRING:
			return "STRING";
		case TokenType::INT:
			return "INT";
		case TokenType::FLOAT:
			return "FLOAT";
		case TokenType::LBracket:
			return "(";
		case TokenType::RBracket:
			return ")";
		case TokenType::LBrace:
			return "{";
		case TokenType::RBrace:
			return "}";
		case TokenType::LSqrBracket:
			return "[";
		case TokenType::RSqrBrack:
			return "]";
		case TokenType::Ident:
			return "Identifier";
		case TokenType::Keyword:
			return "Keyword";
		case TokenType::SemiColon:
			return ";";
		case TokenType::Comma:
			return ",";
        case TokenType::Plus:
            return "+";
        case TokenType::Dot:
            return ".";
        case TokenType::Minus:
            return "-";
        case TokenType::Multiply:
            return "*";
        case TokenType::Divide:
            return "/";
        case TokenType::Equal:
            return "=";
        case TokenType::Modulo:
            return "%";
        case TokenType::LessThan:
            return "<";
        case TokenType::LessOrEqual:
            return "<=";
        case TokenType::GreaterThan:
            return ">";
        case TokenType::GreaterOrEqual:
            return ">=";
        case TokenType::EqualTo:
            return "==";
        case TokenType::NotEqualTo:
            return "!=";
        case TokenType::Eof:
            return "Eof";
        default:
            return "UnknownType";
	}
}


// Token
std::string flang::Token::ToString()
{
	return fmt::format("[Token(val='{}', type={}, pos={})]", this->value, tokToString(this->tok), pos.toString());
}
bool flang::Token::cmp(flang::TokenType t) const {
    return this->tok == t;
}

bool flang::Token::cmp(flang::TokenType t, const std::string_view &val) const {
    return this->tok == t && value == val;
}

// Lexer
void flang::Lexer::nextTok() {
	if (m_tok_idx < m_code.length()) {
		m_tok = m_code[m_tok_idx];
		m_tok_idx++;
		m_pos.update(m_tok);
	}
	else {
		m_tok = END_OF_FILE;
	}
	
}

bool flang::Lexer::isEof() const {
	return m_tok == END_OF_FILE;
}

std::vector<flang::Token> flang::Lexer::getTokens() {
	return m_tokens;
}

flang::Lexer::Lexer(): m_tok('\0'), m_tok_idx(0) {}

std::vector<flang::Token> flang::Lexer::tokenize(std::string_view code, std::string_view _file_name)
{
    m_code = code;
    file_name = _file_name;
    m_pos.code = code;
    m_pos.file_name = _file_name;
    nextTok(); // load the first character to tok

	while (m_tok != END_OF_FILE) {
        if (isspace(m_tok)) {
            nextTok();
            continue;
        }

        switch (m_tok) {
            case '#':
                while (m_tok != END_OF_FILE && m_tok != '\n') {
                    nextTok();
                }
                break;
            case '\\':
                nextTok();
                while (m_tok != END_OF_FILE && m_tok != '\\') {
                    nextTok();
                }
                nextTok();
                break;
            case ',':
                m_tokens.emplace_back(std::string(1, m_tok), TokenType::Comma, m_pos);
                nextTok();
                break;
            case '.':
                m_tokens.emplace_back(std::string(1, m_tok), TokenType::Dot, m_pos);
                nextTok();
                break;
            case ';':
                m_tokens.emplace_back(std::string(1, m_tok), TokenType::SemiColon, m_pos);
                nextTok();
                break;
            case '=':
                parse_equal();
                break;
            case '>':
                parseGreaterOrEqual();
                break;
            case '<':
                parseLessOrEqual();
                break;
            case '+':
                m_tokens.emplace_back(std::string(1, m_tok), TokenType::Plus, m_pos);
                nextTok();
                break;
            case '-':
                m_tokens.emplace_back(std::string(1, m_tok), TokenType::Minus, m_pos);
                nextTok();
                break;
            case '*':
                m_tokens.emplace_back(std::string(1, m_tok), TokenType::Multiply, m_pos);
                nextTok();
                break;
            case '/':
                m_tokens.emplace_back(std::string(1, m_tok), TokenType::Divide, m_pos);
                nextTok();
                break;
            case '%':
                m_tokens.emplace_back(std::string(1, m_tok), TokenType::Modulo, m_pos);
                nextTok();
                break;
            case '!':
                nextTok();
                if (m_tok == '=') {
                    nextTok();
                    m_tokens.emplace_back("!=", TokenType::NotEqualTo, m_pos);
                } else {
                    FError(SyntaxError, fmt::format("Unexpected character '{}'", m_tok), m_pos).Throw();
                }
                break;
            default:
                if (std::strchr("()[]{}", m_tok)) {
                    switch (m_tok) {
                        case '(':
                            m_tokens.emplace_back(std::string(1, m_tok), TokenType::LBracket, m_pos);
                            nextTok();
                            break;
                        case ')':
                            m_tokens.emplace_back(std::string(1, m_tok), TokenType::RBracket, m_pos);
                            nextTok();
                            break;
                        case '[':
                            m_tokens.emplace_back(std::string(1, m_tok), TokenType::LSqrBracket, m_pos);
                            nextTok();
                            break;
                        case ']':
                            m_tokens.emplace_back(std::string(1, m_tok), TokenType::RSqrBrack, m_pos);
                            nextTok();
                            break;
                        case '{':
                            m_tokens.emplace_back(std::string(1, m_tok), TokenType::LBrace, m_pos);
                            nextTok();
                            break;
                        case '}':
                            m_tokens.emplace_back(std::string(1, m_tok), TokenType::RBrace, m_pos);
                            nextTok();
                            break;
                    }
                }
                else if (m_tok == '\'' || m_tok == '"')
                    parseAndPushString();
                else if (isdigit(m_tok))
                    parseAndPushNumber();
                else if (isalpha(m_tok))
                    parseAndPushIdentifier();
                else {
                    FError(SyntaxError, fmt::format("Unknown character '{}'", m_tok), m_pos).Throw();
                }
                break;
        }
	}
    return m_tokens;
}


void flang::Lexer::parseAndPushString() {
	const char quote = m_tok;
    auto pos = m_pos.copy();
	nextTok();

	std::string collected;

	while (!isEof() && m_tok != quote) {
		// TODO: allow backslashing character
		collected += m_tok;
		nextTok();
	}

	if (m_tok != quote && !isEof()) {
		FError(SyntaxError, "unterminated string literal", m_pos)
			.Throw();
	}
	else {
		// step-over ending quote
		nextTok();
	}
	
	m_tokens.emplace_back(collected, STRING, pos);
}

void flang::Lexer::parseAndPushNumber() {
	std::string number;
    auto pos = m_pos.copy();
	bool isFloat = false;

	while (!isEof() && isdigit(m_tok) || m_tok == '.' || m_tok == '_') {
		if (m_tok == '_')
			continue;

		if (m_tok == '.') {
			if (isFloat) {
				FError(SyntaxError, fmt::format("invalid syntax '{}'", m_tok), m_pos)
					.Throw();
			}
			number += m_tok;
			nextTok();
            isFloat = true;
			continue;
		}

		number += m_tok;
		nextTok();
	}

	if (isFloat)
		m_tokens.emplace_back(number, FLOAT, pos);
	else
		m_tokens.emplace_back(number, INT, pos);
}

/// Collects and pushes keyword if it's an existing or identifier otherwise
void flang::Lexer::parseAndPushIdentifier() {
	std::string ident;
    auto pos = m_pos.copy();

	while (!isEof() && isalnum(m_tok) || m_tok == '_') {
		// add to ident until we have alpha numeric values and underscore
		ident += m_tok;
		nextTok();
	}

    bool isKeyword = std::find_if(LanguageManager::keywords.begin(), LanguageManager::keywords.end(), [&](const auto& pair) {
        return pair.second == ident;
    }) != LanguageManager::keywords.end();

    if (isKeyword) {
        m_tokens.emplace_back(ident, Keyword, pos);
    } else {
//        auto t = Token(ident, Ident, pos);
//        m_tokens.push_back(t);
	    m_tokens.emplace_back(ident, Ident, pos);
    }

}

void flang::Lexer::parseGreaterOrEqual() {
    nextTok();
    if (m_tok == '=') {
        nextTok();
        m_tokens.emplace_back(">=", TokenType::GreaterOrEqual, m_pos);
        return;
    }

    m_tokens.emplace_back(">", TokenType::GreaterThan, m_pos);
}

void flang::Lexer::parseLessOrEqual() {
    nextTok();
    if (m_tok == '=') {
        nextTok();
        m_tokens.emplace_back("<=", TokenType::LessOrEqual, m_pos);
        return;
    }

    m_tokens.emplace_back("<", TokenType::LessThan, m_pos);
}

void flang::Lexer::parse_equal() {
    nextTok();
    if (m_tok == '=') {
        nextTok();
        m_tokens.emplace_back("==", TokenType::EqualTo, m_pos);
        return;
    }

    m_tokens.emplace_back("=", TokenType::Equal, m_pos);
}

