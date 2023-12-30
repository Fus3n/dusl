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

        std::shared_ptr<Object> visit(const std::shared_ptr<DataNode>& node);

        std::shared_ptr<Object> visit(ProgramNode& node);
        std::shared_ptr<Object> visit(BlockNode& node);
    };
}