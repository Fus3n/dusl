#pragma once

#include <cstdint>
#include <dusl/Serializable.hpp>

namespace dusl {
	struct Position: public Serializable {
	public:
		uint32_t line;
		uint32_t row;

        std::string repl_code = "";
        std::string file_name = "";

        Position() : line(0), row(0) {}
        Position(const Position& pos) : line(pos.line), row(pos.row), repl_code(pos.repl_code), file_name(pos.file_name) {}

       // Position(const Position& pos) : line(pos.line), row(pos.row), file_name(pos.file_name) {}
        Position(const uint32_t _line, const uint32_t _row) : line(_line), row(_row) {}

		void update(char tok);
        void setCodeAndFile(const std::string& _code, const std::string& _file_name);
        [[nodiscard]] Position copy() const;
        std::string toString();
		nlohmann::ordered_json toJson() const override;
	};
}
