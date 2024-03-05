#pragma once
#include <utility>
#include "SymbolTable.hpp"
#include <stack>

namespace dusl {
    const int MAX_STACK_SIZE = 1000;

    class Context {
    public:
        std::stack<SymbolTable> sym_table_stack;

        Context() = default;

        std::string getName() const;
        void setName(std::string& context_name);

        std::string getBaseCtxName() const;
        void setBaseCtxName(std::string& context_name);

        void enterScope();
        void exitScope();
        SymbolTable& currentSymbol();  // Added method
        void setCurrentSymbol(dusl::SymbolTable& sym_table);

    private:
        std::string name = "_default";
        std::string base_ctx_name = "_module";
    };
}


