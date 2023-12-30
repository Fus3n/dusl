#pragma once

#include <iostream>
#include <vector>

#include "Position.hpp"


namespace flang {

	enum TokenType {
		// Data types
		STRING,
		INT,
		FLOAT,

		// Brackets
		LBracket, // (
		RBracket, // )
		LBrace, // {
		RBrace, // }
		LSqrBracket, // [
		RSqrBrack, // ]

        // Arithmetics
        Plus,
        Minus,
        Multiply,
        Divide,
        Modulo,

		// Essentials
		Ident,
		Keyword,
        Dot, // .
        Comma, // ,
        SemiColon, // ;
        Equal, // =
        LessThan, // <
        LessOrEqual, // <=
        GreaterThan, // >
        GreaterOrEqual, // >=
        EqualTo, // ==
        NotEqualTo, // !=
        Eof,
	};

	std::string tokToString(TokenType tok);

	struct Token {
        std::string value;
		TokenType tok;
        Position pos;

        Token(std::string val, TokenType _tok, Position _pos): value(std::move(val)), tok(_tok), pos(_pos) {}
        explicit Token() : tok(TokenType::Eof) {}

        [[nodiscard]] bool cmp(TokenType t) const;
        [[nodiscard]] bool cmp(TokenType t, const std::string_view&  val) const;
		std::string ToString();
    };

	
	class Lexer {
	public:
		static const char END_OF_FILE = '\0';

		Lexer();
		void nextTok();
        std::vector<Token> tokenize(std::string_view code, std::string_view _file_name);
		void parseAndPushString();
		void parseAndPushNumber();
		void parseAndPushIdentifier();
        void parseLessOrEqual();
        void parseGreaterOrEqual();
        void parse_equal();

		std::vector<Token> getTokens();

		// returns if m_tok == END_OF_FILE
		[[nodiscard]] bool isEof() const;

	private:

        std::string_view m_code;
		std::string_view file_name;
        char m_tok; // current tok
		int m_tok_idx; // current tok index
		std::vector<Token> m_tokens;
		Position m_pos;


	};


}