#include "flang/ErrorType.hpp"
#include <fmt/core.h>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <fmt/ranges.h>

std::vector<std::string_view> split_lines(const std::string_view str) {
    std::vector<std::string_view> lines;

    size_t start = 0;
    size_t pos = str.find('\n');

    while (pos != std::string_view::npos) {
        lines.emplace_back(str.data() + start, pos - start);
        start = pos + 1;
        pos = str.find('\n', start);
    }

    // Add the last line (or the only line if there's no '\n' in the string)
    lines.emplace_back(str.data() + start, str.size() - start);

    return lines;
}

std::string flang::FError::generateErrString() {
	// TODO: Implement a very basic error message
    auto msg = fmt::format(
            "{}: {} \n\tat {}:{}",
            errTypeToString(m_err_type),
            m_msg,
            m_pos.line + 1,
            m_pos.row
    );

    auto lines = split_lines(m_pos.code);
    auto line = lines[m_pos.line];
    msg += fmt::format("\n{}\n", line);
    msg += fmt::format("{: >{}}", "", m_pos.row);
    msg += fmt::format(
                "{:^>{}}", "",
                line.substr(m_pos.row, line.length() - 1).length()
            );

//    fmt::println("res = {}", line.substr(m_pos.row, line.length() - 1) );
//    std::cout << line << '\n';
	return msg;
}

void flang::FError::Throw() {
	auto err = generateErrString();
	fmt::print("{}\n", err);
    std::exit(-1);
//    throw std::runtime_error(err);
}

std::string flang::errTypeToString(ErrorType err_type) {
	switch (err_type) {
		case Error:
			return "Error";
		case SyntaxError:
			return "SyntaxError";
		case RunTimeError:
			return "RunTimeError";
        case UnimplementedError:
            return "UnimplementedError";
        case NameError:
            return "NameError";
        case ZeroDivisionError:
            return "ZeroDivisionError";
        case IndexError:
            return "IndexError";
		default:
			return "UnknownError";
	}
}