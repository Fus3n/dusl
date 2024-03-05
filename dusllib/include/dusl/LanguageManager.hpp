#pragma once

#include <string_view>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace dusl {

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
        ImportKey
    };

    class LanguageManager {
    public:
        static std::unordered_map<KeywordType, std::string_view> keywords;
        static void reAssignKeyword(dusl::KeywordType type, std::string_view keyword);
        static std::string_view getValue(KeywordType type);
    };

}