#pragma once

#include <vector>
#include <dusl/Serializable.hpp>

#include "Position.hpp"

namespace dusl {

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
        DoubleDot, // ..
        Comma, // ,
        SemiColon, // ;
        Colon, // ;
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

	struct Token: public Serializable {
        std::string value;
		TokenType tok;
        Position pos;

        Token(std::string val, TokenType _tok, const Position& _pos): value(std::move(val)), tok(_tok), pos(_pos) {}
        explicit Token() : tok(TokenType::Eof) {}
		// a copy constructor
		Token(const Token& _tok) : value(_tok.value), tok(_tok.tok), pos(_tok.pos) {}

        [[nodiscard]] bool cmp(TokenType t) const;
        [[nodiscard]] bool cmp(TokenType t, const std::string_view&  val) const;
		std::string toString();
		nlohmann::ordered_json toJson() const override;
	};

	class Lexer {
	public:
		static const char END_OF_FILE = '\0';

		Lexer();
		void nextTok();
        std::vector<Token> tokenize(const std::string& _file_name, const std::string& code, const std::string& repl_code = "");
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