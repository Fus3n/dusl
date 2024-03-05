#include <fstream>
#include <dusl/utils/common.h>

std::vector<std::string_view> dusl::split_lines(const std::string_view str) {
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
void dusl::str_ltrim(std::string &s) {
    s.erase (s.begin (), std::find_if (s.begin (), s.end (), [] (unsigned char ch) {
        return !std::isspace (ch);
    }));
}

/// str_trim std::string from end (in place)
void dusl::str_rtrim(std::string &s) {
    s.erase (std::find_if (s.rbegin (), s.rend (), [] (unsigned char ch) {
        return !std::isspace (ch);
    }).base (), s.end ());
}

/// str_trim std::string from both ends (in place)
void dusl::str_trim(std::string &s) {
    dusl::str_rtrim(s);
    dusl::str_ltrim(s);
}

std::string dusl::getOsName()
{
#ifdef _WIN32
    return "Windows 32-bit";
#elif _WIN64
    return "Windows 64-bit";
  #elif __APPLE__ || __MACH__
  return "Mac OSX";
  #elif __linux__
  return "Linux";
  #elif __FreeBSD__
  return "FreeBSD";
  #elif __unix || __unix__
  return "Unix";
  #else
  return "Unknown";
#endif
}

std::string dusl::read_file(const std::string& file_name) {
    std::string contents;
    std::ifstream file(file_name);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
    while (!file.eof()) {
        std::string line;
        std::getline(file, line);
        contents += line + "\n";
    }
    file.close();
    return contents;
}

void dusl::write_file(const std::string &file_name, const std::string &contents) {
    std::ofstream file(file_name);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
    file << contents;
    file.close();
}
