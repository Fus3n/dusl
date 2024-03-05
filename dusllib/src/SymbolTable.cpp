#include <dusl/SymbolTable.hpp>

#include <utility>

std::shared_ptr<dusl::Object> dusl::SymbolTable::getValue(const std::string &key) {
   return variables[key]; // TODO: add error checking
}

void dusl::SymbolTable::setValue(const std::string &key, std::shared_ptr<Object> value) {
    variables[key] = std::move(value); // TODO: moving might cause issue check later
}

[[maybe_unused]] [[maybe_unused]] bool dusl::SymbolTable::hasValue(const std::string &key) {
    if (variables.empty())
        return false;
    return variables.find(key) != variables.end();
}

bool dusl::SymbolTable::hasKey(const std::string &key) const {
    if (variables.empty())
        return false;
    return variables.count(key);
}

void dusl::SymbolTable::merge(dusl::SymbolTable &other) {
    // Merge the contents of the other symbol table into this one
    for (const auto& entry : other.variables) {
        auto it = variables.find(entry.first);
        if (it != variables.end()) {
            // Update existing key
            it->second = entry.second;
        }
        // Ignore new keys
    }
}

