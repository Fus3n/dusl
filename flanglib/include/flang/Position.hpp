#pragma once

#include <iostream>
#include <cstdint>
#include <utility>

namespace flang {
	struct Position {
	public:
		uint32_t line;
		uint32_t row;

        std::string_view code;
        std::string_view file_name;

        Position() : line(0), row(0) {}
        Position(const Position& pos) : line(pos.line), row(pos.row), code(pos.code), file_name(pos.file_name) {}

        Position(uint32_t _line, uint32_t _row) : line(_line), row(_row) {}

		void update(char tok);
        void setCodeAndFile(std::string_view _code, std::string_view _file_name);
        [[nodiscard]] Position copy() const;
        std::string toString();
	};
}
