#include "flang/Position.hpp"
#include <fmt/format.h>

void flang::Position::update(const char tok) {
	if (tok == '\n') {
		this->line++;
		this->row = 0;
		return;
	}

	this->row++;
}

std::string flang::Position::toString() {
   return fmt::format("Position {{ filename = {}, line = {}, row = {} }}", file_name, line, row);
}

flang::Position flang::Position::copy() const {
    auto pos = flang::Position(line, row);
    pos.code = code;
    pos.file_name = file_name;
    return pos;
}

void flang::Position::setCodeAndFile(std::string_view _code, std::string_view _file_name) {
    code = _code;
    file_name = _file_name;
}

