#pragma once

#include <string_view>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace flang {

    enum KeywordType {
        IfKey,
        ElseKey,
        ElseIfKey,
        FuncDefKey,
        WhileKey,
        BreakKey,
        ForKey,
        StructKey,
        AndKey,
        OrKey,
        NotKey,
        ReturnKey,
        FromKey,
        InKey,
    };

    class LanguageManager {
    public:
        static std::unordered_map<KeywordType, std::string_view> keywords;
        static void reAssignKeyword(flang::KeywordType type, std::string_view keyword);
        static std::string_view getValue(KeywordType type);
    };

}