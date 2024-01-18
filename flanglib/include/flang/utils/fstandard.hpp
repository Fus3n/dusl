#pragma once
#include "flang/Interpreter.hpp"
#include "flang/FObject.hpp"
#include <vector>

namespace flang {
    [[maybe_unused]] bool loadSTL(flang::Interpreter &visitor);

    flang::FResult
    print_line(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    print_same_line(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    read_line(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    get_type(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    get_time(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    get_elapsed_time_ms(flang::Interpreter & visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    get_elapsed_time_ns(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    fill_list(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    hash_object(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    random_int(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
    flang::FResult
    system(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok);
}
