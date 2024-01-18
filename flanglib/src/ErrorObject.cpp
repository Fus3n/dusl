#include "FObject.hpp"
#include "utils/common.h"

std::string flang::ErrorObject::toString() const {
    return generateErrString();
}

std::string flang::ErrorObject::generateErrString() const {
    auto msg = fmt::format(
            "{}: {} \n\tat {}:{}",
            errTypeToString(err_type),
            err_msg,
            pos.line + 1,
            pos.row
    );

    auto lines = split_lines(pos.code);
    auto line = lines[pos.line];
    msg += fmt::format("\n{}\n", line);
    msg += fmt::format("{: >{}}", "", pos.row);
    msg += fmt::format(
            "{:^>{}}", "",
            line.substr(pos.row, line.length() - 1).length()
    );

    return msg;
}
