#pragma once
#include <dusl/Interpreter.hpp>
#include <dusl/DObject.hpp>
#include <vector>

namespace dusl {
    /// DEVELOPMENT ONLY PATH, Visual Studio
    static const std::string DUSL_STD_PATH = "../../../dusl_std";

    [[maybe_unused]] bool loadSTL(dusl::Interpreter &visitor);
    std::vector<std::shared_ptr<dusl::StructProxyObject>> loadObjects(dusl::Interpreter &visitor);

    dusl::FResult print_line(dusl::Interpreter&, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult print_same_line(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult read_line(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult get_type(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult get_time(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult get_elapsed_time_ms(dusl::Interpreter & visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult get_elapsed_time_ns(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult fill_list(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult hash_object(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult random_int(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult system(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult exit_dusl(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult error_dusl(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult math_round(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult math_floor(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok);
    dusl::FResult from_char_code(dusl::Interpreter& visitor, ArgumentObject& args_node, const dusl::Token& tok);
    dusl::FResult time_sleep(dusl::Interpreter& visitor, ArgumentObject& args_node, const dusl::Token& tok);


    class DFile : public dusl::DBaseStruct {
    public:

        std::string file_name;
        std::string op_mode = "r";

        DFile(std::string _file_name);

        static StructCreationResult init_(dusl::ArgumentObject &args_node);

        std::string getTypeString() override;

        static dusl::FResult exists(std::shared_ptr<DFile> cls, dusl::Interpreter &visitor,
                                    const std::shared_ptr<dusl::FunctionCallNode> &fn_node);

        static dusl::FResult file_size(std::shared_ptr<DFile> cls, dusl::Interpreter &visitor,
                                       const std::shared_ptr<dusl::FunctionCallNode> &fn_node);

        static dusl::FResult read(std::shared_ptr<DFile> cls, dusl::Interpreter &visitor,
                                  const std::shared_ptr<dusl::FunctionCallNode> &fn_node);

    };
}
