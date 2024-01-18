#include <chrono>
#include "flang/utils/fstandard.hpp"
#include "flang/ErrorType.hpp"
#include "flang/utils/fcore.h"
#include <random>

bool flang::loadSTL(flang::Interpreter &visitor) {
    auto flang_true = flang::createBool(true);
    auto flang_false = flang::createBool(false);

    // set global variables
    flang::addGlobalVariable(visitor, "true",  &flang_true);
    flang::addGlobalVariable(visitor, "false",  &flang_false);

    // set global functions
    flang::createFunction(visitor, "println", &print_line);
    flang::createFunction(visitor, "print", &print_same_line);
    flang::createFunction(visitor, "readLine", &read_line);
    flang::createFunction(visitor, "type", &get_type);
    flang::createFunction(visitor, "getTime", &get_time);
    flang::createFunction(visitor, "getElapsedTimeMS", &get_elapsed_time_ms);
    flang::createFunction(visitor, "getElapsedTimeNS", &get_elapsed_time_ns);
    flang::createFunction(visitor, "fillList", &fill_list);
    flang::createFunction(visitor, "hash", &hash_object);
    flang::createFunction(visitor, "rnd", &random_int);
    flang::createFunction(visitor, "system", &system);

    return true;
}

flang::FResult
flang::print_line(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    /// signature print(items,...)
    for (const auto& arg:  args) {
        std::cout << arg->toString() << " ";
    }
    std::cout << std::endl;
    return FResult::createResult(std::make_shared<flang::NoneObject>(flang::Token()), tok);
}

flang::FResult
flang::print_same_line(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    /// signature print(items,...)
    for (const auto& arg:  args) {
        std::cout << arg->toString();
    }
    return FResult::createResult(std::make_shared<flang::NoneObject>(flang::Token()), tok);
}

flang::FResult
flang::read_line(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    auto r = flang::verifyArgsCount(args.size(), 0, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }
    std::string line;
    std::getline(std::cin, line);
    return FResult::createResult(std::make_shared<flang::StringObject>(line, tok), tok);
}

flang::FResult
flang::get_type(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    /// signature type(type)
    auto r = verifyArgsCount(args.size(), 1, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }
    return FResult::createResult(std::make_shared<flang::StringObject>(args[0]->getTypeString(), args[0]->tok), tok);
}

flang::FResult
flang::get_time(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    auto r = verifyArgsCount(args.size(), 0, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }
    // Get the current time point
    auto now = std::chrono::high_resolution_clock::now();

    // Convert it to milliseconds
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds >(now);

    // Return the number of ticks of the duration
    return FResult::createResult(
        std::make_shared<flang::IntObject>(now_ms.time_since_epoch().count(), tok),
        tok
    );
}

flang::FResult
flang::get_elapsed_time_ms(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    /// signature getElapsedTimeMS(start, end)
    auto r = verifyArgsCount(args.size(), 2, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
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

    return FResult::createResult(
            std::make_shared<flang::IntObject>(duration.count(), tok),
            tok
    );
}

flang::FResult
flang::get_elapsed_time_ns(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    /// signature getElapsedTimeMS(start, end)
    auto r = verifyArgsCount(args.size(), 2, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    int64_t start = 0;
    int64_t end = 0;
    if (auto intObject = std::dynamic_pointer_cast<flang::IntObject>(args[0]))
        start = intObject->value;

    if (auto intObject = std::dynamic_pointer_cast<flang::IntObject>(args[1]))
        end = intObject->value;

    // Convert the integers to duration objects
    auto start_ms = std::chrono::nanoseconds(start);
    auto end_ms = std::chrono::nanoseconds(end);

    // Calculate the difference in duration
    auto duration = end_ms - start_ms;

    return FResult::createResult(std::make_shared<flang::IntObject>(duration.count(), tok), tok);
}
// fill a list with 0
flang::FResult
flang::fill_list(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    /// signature fillList(list, count)
    auto r = verifyArgsCount(args.size(), 2, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }
    if (auto listObject = std::dynamic_pointer_cast<flang::ListObject>(args[0])) {
        int64_t count = 0;
        if (auto intObject = std::dynamic_pointer_cast<flang::IntObject>(args[1])) {
            count = intObject->value;
        }
        while (listObject->items.size() < count) {
            listObject->items.push_back(std::make_shared<flang::IntObject>(0, tok));
        }

        return FResult::createResult(listObject, tok);
    }
}

flang::FResult
flang::hash_object(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>>& args, const flang::Token& tok) {
    /// hash(object)
    auto r = verifyArgsCount(args.size(), 1, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    return FResult::createResult(std::make_shared<flang::IntObject>(std::hash<std::string>()(args[0]->toString()), tok), tok);
}

flang::FResult
flang::random_int(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>> &args, const flang::Token &tok) {
    /// rnd(int)
    auto r = verifyArgsCount(args.size(), 1, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    if (auto intObject = std::dynamic_pointer_cast<flang::IntObject>(args[0])) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist_rand(0,intObject->value);
        return FResult::createResult(std::make_shared<flang::IntObject>(dist_rand(rng), tok), tok);
    }

    return FResult::createError(RunTimeError, "rnd: argument must be an integer", tok);
}

flang::FResult flang::system(flang::Interpreter &visitor, const std::vector<std::shared_ptr<flang::Object>> &args,
                             const flang::Token &tok) {
    /// system(int)
    auto r = verifyArgsCount(args.size(), 1, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    if (auto strObject = std::dynamic_pointer_cast<flang::StringObject>(args[0])) {
        auto res = std::system(strObject->value.c_str());
        return FResult::createResult(std::make_shared<flang::IntObject>(res, tok), tok);
    }

    return FResult::createError(RunTimeError, "system: argument must be an string", tok);
}

