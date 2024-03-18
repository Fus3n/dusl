#pragma once
#include <dusl/Interpreter.hpp>
#include <dusl/DObject.hpp>
#include <vector>

namespace dusl {
    /// DEVELOPMENT ONLY PATH, Visual Studio
    static const std::string DUSL_STD_PATH = "../../../dusl_std";

    [[maybe_unused]] bool loadSTL(dusl::Interpreter &visitor);

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


}
