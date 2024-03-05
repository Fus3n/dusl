#pragma once
#include <memory>
#include "DObject.hpp"
#include "DataNodes.hpp"
#include "Context.hpp"

namespace dusl {
    class Interpreter {
    public:
        Context ctx;

        void setContext(Context _ctx);

        dusl::FResult visit(const std::shared_ptr<DataNode>& node);

        dusl::FResult visit(ProgramNode &node);
        dusl::FResult visit(BlockNode &node);
    };
}