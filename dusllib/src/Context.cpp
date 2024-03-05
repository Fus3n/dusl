#include "Context.hpp"
#include <utility>


void dusl::Context::setName(std::string& context_name) {
    this->name = context_name;
}

std::string dusl::Context::getName() const {
    return name;
}

std::string dusl::Context::getBaseCtxName() const {
    return base_ctx_name;
}

void dusl::Context::setBaseCtxName(std::string &context_name) {
    this->base_ctx_name = context_name;
}


void dusl::Context::enterScope() {
    if (sym_table_stack.size() > MAX_STACK_SIZE) {
        throw std::runtime_error("Maximum stack size exceeded");
    }
    // Create a new symbol table that inherits from the parent's symbol table
    if (sym_table_stack.empty()) {
        sym_table_stack.push(std::move(SymbolTable()));
        return;
    }

    SymbolTable new_table(sym_table_stack.top());
    sym_table_stack.push(std::move(new_table));
}

void dusl::Context::exitScope() {
    if (sym_table_stack.size() > 1) {
        // Pop the top symbol table from the stack
        SymbolTable current_table = std::move(sym_table_stack.top());
        sym_table_stack.pop();

        // Merge changes made in the current scope back to the parent scope
        sym_table_stack.top().merge(current_table);
    }
}

dusl::SymbolTable &dusl::Context::currentSymbol() {
    return sym_table_stack.top();
}

void dusl::Context::setCurrentSymbol(dusl::SymbolTable& sym_table) {
    sym_table_stack.top() = sym_table;
}



