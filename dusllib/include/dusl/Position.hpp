#pragma once

#include <cstdint>
#include <dusl/Serializable.hpp>

namespace dusl {
	struct Position: public Serializable {
	public:
		uint32_t line;
		uint32_t row;

        std::string_view code;
        std::string_view file_name;

        Position() : line(0), row(0) {}
        Position(const Position& pos) : line(pos.line), row(pos.row), code(pos.code), file_name(pos.file_name) {}
        Position(const uint32_t _line, const uint32_t _row) : line(_line), row(_row) {}

		void update(char tok);
        void setCodeAndFile(std::string_view _code, std::string_view _file_name);
        [[nodiscard]] Position copy() const;
        std::string toString();
		nlohmann::ordered_json toJson() const override;
	};
}
