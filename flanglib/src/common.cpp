#include "utils/common.h"


std::vector<std::string_view> flang::split_lines(const std::string_view str) {
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

/// str_trim from std::string start (in place)
void flang::str_ltrim(std::string &s) {
    s.erase (s.begin (), std::find_if (s.begin (), s.end (), [] (unsigned char ch) {
        return !std::isspace (ch);
    }));
}

/// str_trim std::string from end (in place)
void flang::str_rtrim(std::string &s) {
    s.erase (std::find_if (s.rbegin (), s.rend (), [] (unsigned char ch) {
        return !std::isspace (ch);
    }).base (), s.end ());
}

/// str_trim std::string from both ends (in place)
void flang::str_trim(std::string &s) {
    flang::str_rtrim(s);
    flang::str_ltrim(s);
}
