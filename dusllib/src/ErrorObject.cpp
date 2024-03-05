#include "DObject.hpp"
#include "utils/common.h"

std::string dusl::ErrorObject::toString() const {
    return generateErrString();
}

std::string dusl::ErrorObject::generateErrString() const {
    fmt::println(" Tracback:\nFile {}", pos.file_name);
    auto msg = fmt::format(
            " {}: {} \n\tat {}:{}",
            errTypeToString(err_type),
            err_msg,
            pos.line + 1,
            pos.row
    );

    const auto lines = split_lines(pos.code);
    auto line = lines[pos.line];
    msg += fmt::format("\n{}\n", line);
    msg += fmt::format("{: >{}}", "", pos.row);
    msg += fmt::format(
            "{:^>{}}", "",
            line.substr(pos.row, line.length() - 1).length()
    );

    return msg;
}
