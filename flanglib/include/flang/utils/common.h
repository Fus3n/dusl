#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>

namespace flang {
    std::vector<std::string_view> split_lines(std::string_view str);
    void str_ltrim(std::string &s);
    void str_rtrim(std::string &s);
    void str_trim(std::string &s);

};
