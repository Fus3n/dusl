#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <flang/utils/fcore.h>
#include <flang/Context.hpp>
#include <flang/SymbolTable.hpp>
#include <flang/ErrorType.hpp>
#include <memory>
#include <chrono>

namespace fs = std::filesystem;


std::shared_ptr<flang::Object> printer(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    for (const auto& arg:  args) {
        std::cout << arg->ToString() << " ";
    }
    std::cout << std::endl;
    return std::make_shared<flang::NoneObject>(flang::Token());
}

std::shared_ptr<flang::Object> get_type(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    auto r = verifyArgsCount(args.size(), 1, tok);
    if (r.has_value()) {
        flang::FError(flang::RunTimeError, r.value(), tok.pos).Throw();
    }
    return std::make_shared<flang::StringObject>(args[0]->getTypeString(), args[0]->tok);
}

// some kind of timing function to start a timer and end then calcuate
std::shared_ptr<flang::Object> get_time(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    auto r = verifyArgsCount(args.size(), 0, tok);
    if (r.has_value()) {
        flang::FError(flang::RunTimeError, r.value(), tok.pos).Throw();
    }
    // Get the current time point
    auto now = std::chrono::high_resolution_clock::now();

    // Convert it to milliseconds
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds >(now);

    // Return the number of ticks of the duration
    return std::make_shared<flang::IntObject>(now_ms.time_since_epoch().count(), tok);
}

std::shared_ptr<flang::Object> get_elapsed_time(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    auto r = verifyArgsCount(args.size(), 2, tok);
    if (r.has_value()) {
        flang::FError(flang::RunTimeError, r.value(), tok.pos).Throw();
    }

    int64_t start = 0;
    int64_t end = 0;
    if (auto intObject = std::dynamic_pointer_cast<flang::IntObject>(args[0]))
        start = intObject->value;

    if (auto intObject = std::dynamic_pointer_cast<flang::IntObject>(args[1]))
        end = intObject->value;

    // Convert the integers to duration objects
    auto start_ms = std::chrono::milliseconds (start);
    auto end_ms = std::chrono::milliseconds (end);

    // Calculate the difference in duration
    auto duration = end_ms - start_ms;


    // Return the number of ticks of the duration as a double
    return std::make_shared<flang::IntObject>(duration.count(), tok);
}

// fill a list with 0
std::shared_ptr<flang::Object> fill_list(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    auto r = verifyArgsCount(args.size(), 2, tok);
    if (r.has_value()) {
        flang::FError(flang::RunTimeError, r.value(), tok.pos).Throw();
    }
    if (auto listObject = std::dynamic_pointer_cast<flang::ListObject>(args[0])) {
        int64_t count = 0;
        if (auto intObject = std::dynamic_pointer_cast<flang::IntObject>(args[1])) {
            count = intObject->value;
        }
        while (listObject->items.size() < count) {
            listObject->items.push_back(std::make_shared<flang::IntObject>(0, tok));
        }

        return listObject;
    }
}



int main()
{
    std::string file_name = "../test.flin";
    flang::Interpreter visitor;

    flang::Context global_context;
    global_context.enterScope();
    auto global_symbol = flang::SymbolTable();
    visitor.setContext(global_context);

    flang::createFunction(visitor, "print", &printer);
    flang::createFunction(visitor, "type", &get_type);
    flang::createFunction(visitor, "getTime", &get_time);
    flang::createFunction(visitor, "getElapsedTimeMS", &get_elapsed_time);
    flang::createFunction(visitor, "fillList", &fill_list);

    bool success = runSingleFile(file_name, visitor);
    global_context.exitScope();

    return success;
}
