#pragma once
#include <utility>
#include "SymbolTable.hpp"
#include <stack>

namespace flang {
    const int MAX_STACK_SIZE = 1000;

    class Context {
    public:
        std::stack<SymbolTable> sym_table_stack;

        Context() = default;

        std::string getName();
        void setName(std::string& context_name);

        void enterScope();
        void exitScope();
        SymbolTable& currenSymbol();  // Added method

    private:
        std::string name = "_default";

    };
}


