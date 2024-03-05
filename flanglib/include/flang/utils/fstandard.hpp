#pragma once
#include "flang/Interpreter.hpp"
#include "flang/FObject.hpp"
#include <vector>

namespace flang {
    static const std::string FLANG_STD_PATH = "../fl_std"; // TODO: update it

    [[maybe_unused]] bool loadSTL(flang::Interpreter &visitor);

    flang::FResult print_line(flang::Interpreter&, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult print_same_line(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult read_line(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult get_type(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult get_time(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult get_elapsed_time_ms(flang::Interpreter & visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult get_elapsed_time_ns(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult fill_list(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult hash_object(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult random_int(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult system(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult exit_flang(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult error_flang(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult math_round(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);
    flang::FResult math_floor(flang::Interpreter &visitor, ArgumentObject &args_node, const flang::Token& tok);


}
