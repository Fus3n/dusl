#include <dusl/Position.hpp>
#include <fmt/format.h>

void dusl::Position::update(const char tok) {
	if (tok == '\n') {
		this->line++;
		this->row = 0;
		return;
	}

	this->row++;
}

std::string dusl::Position::toString() {
   return fmt::format("Position {{ filename = {}, line = {}, row = {} }}", file_name, line, row);
}

dusl::Position dusl::Position::copy() const {
    auto pos = dusl::Position(line, row);
    pos.repl_code = repl_code;
    pos.file_name = file_name;
    return pos;
}

void dusl::Position::setCodeAndFile(const std::string& _code, const std::string& _file_name) {
    repl_code = _code;
    file_name = _file_name;
}

nlohmann::ordered_json dusl::Position::toJson() const {
	return {
		{"line", line},
		{"row", row},
		{"code", "<code>"},
		{"file_name", file_name}
	};
}