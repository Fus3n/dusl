#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "DObject.hpp"

namespace dusl {
    class SymbolTable {
    public:
        std::unordered_map<std::string, std::shared_ptr<Object>> variables;

        SymbolTable() = default;

        void setValue(const std::string& key, std::shared_ptr<Object> value);
        std::shared_ptr<Object> getValue(const std::string& key);

        [[maybe_unused]] bool hasValue(const std::string& key);
        bool hasKey(const std::string& key) const;
        void merge(SymbolTable& other);

    };
}