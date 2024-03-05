#include "flang/LanguageManager.hpp"

// Assign Default keywords
std::unordered_map<flang::KeywordType, std::string_view> flang::LanguageManager::keywords = {
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

void flang::LanguageManager::reAssignKeyword(flang::KeywordType type, std::string_view keyword) {
    keywords[type] = keyword;
}

std::string_view flang::LanguageManager::getValue(flang::KeywordType type) {
    return keywords[type];
}
