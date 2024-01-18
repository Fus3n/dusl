#pragma once
#include <memory>
#include "FObject.hpp"
#include "DataNodes.hpp"
#include "Context.hpp"

namespace flang {
    class Interpreter {
    public:
        Context ctx;

        void setContext(Context _ctx);

        flang::FResult visit(const std::shared_ptr<DataNode>& node);

        flang::FResult visit(ProgramNode &node);
        flang::FResult visit(BlockNode &node);
    };
}