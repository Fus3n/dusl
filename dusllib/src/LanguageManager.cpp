#include <dusl/LanguageManager.hpp>

// Assign Default keywords
std::unordered_map<dusl::KeywordType, std::string_view> dusl::LanguageManager::keywords = {
        {IfKey, "if"},
        {ElseKey, "else"},
        {ElseIfKey, "elseif"},
        {FuncDefKey, "fn"},
        {ForKey, "for"},
        {WhileKey, "while"},
        {BreakKey, "break"},
        {StructKey, "struct"},
        {AndKey, "and"},
        {OrKey, "or"},
        {NotKey, "not"},
        {ReturnKey, "return"},
        {FromKey, "from"},
        {InKey, "in"},
        {ImportKey, "import"}
};

void dusl::LanguageManager::reAssignKeyword(dusl::KeywordType type, std::string_view keyword) {
    keywords[type] = keyword;
}

std::string_view dusl::LanguageManager::getValue(dusl::KeywordType type) {
    return keywords[type];
}
