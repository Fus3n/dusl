#include "flang/ErrorType.hpp"
#include <fmt/core.h>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <fmt/ranges.h>
#include "utils/common.h"


std::string flang::FError::generateErrString() {
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

	return msg;
}

void flang::FError::Throw() {
	auto err = generateErrString();
	fmt::print("{}\n", err);
    std::exit(-1);
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
        case ImportError:
            return "ImportError";
        case TypeError:
            return "TypeError";
		default:
			return "UnknownError";
	}
}