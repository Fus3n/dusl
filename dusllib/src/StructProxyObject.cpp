#include "DObject.hpp"

std::string dusl::StructProxyObject::toString() const {
    return fmt::format("type<{}>", cls_name);
}

std::string dusl::StructProxyObject::getTypeString() const {
	return cls_name;
}

dusl::FResult dusl::StructProxyInstanceObject::callProperty(Interpreter& visitor, const std::shared_ptr<dusl::FunctionCallNode> fn_node) {

	auto func = exp_funcs.find(fn_node->tok.value);
	if (func != exp_funcs.end()) {
		return func->second(this->cls_ref, visitor, fn_node);
	}	

	return Object::callProperty(visitor, fn_node);
}

std::string dusl::StructProxyInstanceObject::toString() const {
	const void* address = static_cast<const void*>(this);
	return fmt::format("{}<{}>", cls_name, address);
}

std::string dusl::StructProxyInstanceObject::getTypeString() const {
	if (cls_ref == nullptr) {
		return "None";
	}
	return cls_ref->getTypeString();
}


void dusl::StructProxyObject::addFunc(const std::string& name, dusl::ExposedFunctions func) {
	exp_funcs[name] = func;
}

dusl::FResult dusl::StructProxyObject::call(Interpreter& visitor, ArgumentObject& args_node, const Token& token) {

	//fmt::println("hello {}", creator_func == nullptr);
	if (creator_func) {
		auto instance_res = creator_func(args_node);
        if (instance_res.is_error) {
            return FResult(instance_res.err);
        }

		return FResult::createResult(
			std::make_shared<StructProxyInstanceObject>(
				cls_name,
                instance_res.result,
				exp_funcs,
				tok
			),
			token
		);
	}

	return Object::call(visitor, args_node, token);
}

