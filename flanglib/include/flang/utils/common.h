#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>

namespace flang {
    [[maybe_unused]] std::string read_file(const std::string& file_name);
    [[maybe_unused]] void write_file(const std::string& file_name, const std::string& contents);
    std::vector<std::string_view> split_lines(std::string_view str);
    void str_ltrim(std::string &s);
    void str_rtrim(std::string &s);
    void str_trim(std::string &s);
    std::string getOsName();
};
