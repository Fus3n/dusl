#include "DObject.hpp"

// MAIN STRUCt
std::string dusl::StructObject::toString() const {
    return fmt::format("struct<{}>", name);
}

std::string dusl::StructObject::getTypeString() const {
    return name;
}

dusl::FResult dusl::StructObject::call(Interpreter &visitor, ArgumentObject &args_node, const Token &token) {

    // Assume Object has a copy constructor
    std::unordered_map<std::string, std::shared_ptr<Object>> ins_props;
    for (const auto&[fst, snd] : properties) {
        // Create a new shared_ptr that points to a copy of the object
        if (args_node.default_args.count(fst) > 0) {
            ins_props[fst] = args_node.default_args.at(fst);
        } else {
            ins_props[fst] = snd;
        }
    }


    const auto inst = new StructInstanceObject(name, ins_props, tok);
    return FResult::createResult(std::shared_ptr<StructInstanceObject>(inst), token);
}


// INSTANCE Object
std::string dusl::StructInstanceObject::toString() const {
    const auto * address = static_cast<const void*>(this);
    return fmt::format("{}<{}>", name, address);
}

std::string dusl::StructInstanceObject::getTypeString() const {
    return name;
}

dusl::FResult dusl::StructInstanceObject::getProperty(const std::string &name, const Token &tok) {
    if (properties.count(name) == 0) {
        return FResult::createError(RunTimeError, fmt::format("Property {} not found", name), tok);
    }
    return FResult::createResult(properties.at(name), tok);
}

dusl::FResult dusl::StructInstanceObject::callProperty(Interpreter &visitor, const std::shared_ptr<dusl::FunctionCallNode> fn_node) {
    if (properties.count(fn_node->tok.value) == 0) {
        return Object::callProperty(visitor, fn_node);
    }

    auto args_result = fn_node->args_node.accept(visitor);
    if (args_result.isError())
        return args_result;

    ArgumentObject arg_obj_new = *dynamic_cast<ArgumentObject*>(args_result.result.get());

    auto& func = properties.at(fn_node->tok.value);
    return func->call(visitor, arg_obj_new, tok);
}
