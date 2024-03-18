#include <chrono>
#include <dusl/utils/dusl_standard.hpp>
#include <dusl/ErrorType.hpp>
#include <dusl/utils/dusl_core.hpp>
#include <random>

bool dusl::loadSTL(dusl::Interpreter &visitor) {
    auto dusl_true = dusl::createBool(true);
    auto dusl_false = dusl::createBool(false);

    // set global variables
    dusl::addGlobalVariable(visitor, "true",  &dusl_true);
    dusl::addGlobalVariable(visitor, "false",  &dusl_false);

    // set global functions
    dusl::createFunction(visitor, "println", &print_line);
    dusl::createFunction(visitor, "print", &print_same_line);
    dusl::createFunction(visitor, "readLine", &read_line);
    dusl::createFunction(visitor, "type", &get_type);
    dusl::createFunction(visitor, "getTime", &get_time);
    dusl::createFunction(visitor, "getElapsedTimeMS", &get_elapsed_time_ms);
    dusl::createFunction(visitor, "getElapsedTimeNS", &get_elapsed_time_ns);
    dusl::createFunction(visitor, "fillList", &fill_list);
    dusl::createFunction(visitor, "fromCharCode", &from_char_code);
    dusl::createFunction(visitor, "hash", &hash_object);
    dusl::createFunction(visitor, "rnd", &random_int);
    dusl::createFunction(visitor, "round", &math_round);
    dusl::createFunction(visitor, "floor", &math_floor);
    dusl::createFunction(visitor, "system", &system);
    dusl::createFunction(visitor, "error", &error_dusl);
    dusl::createFunction(visitor, "exit", &exit_dusl);

    return true;
}

dusl::FResult
dusl::print_line(dusl::Interpreter&, ArgumentObject &args_node, const dusl::Token& tok) {
    /// signature print(items,...)
    for (const auto& arg:  args_node.args) {
        std::cout << arg->toString() << " ";
    }
    std::cout << std::endl;
    return FResult::createResult(std::make_shared<dusl::NoneObject>(dusl::Token()), tok);
}

dusl::FResult
dusl::print_same_line(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok) {
    /// signature print(items,...)
    for (const auto& arg:  args_node.args) {
        std::cout << arg->toString();
    }
    return FResult::createResult(std::make_shared<dusl::NoneObject>(dusl::Token()), tok);
}

dusl::FResult
dusl::read_line(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok) {
    /// signature read_line()
    auto r = dusl::verifyArgsCount(args_node.args.size(), 0, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }
    std::string line;
    std::getline(std::cin, line);
    return FResult::createResult(std::make_shared<dusl::StringObject>(line, tok), tok);
}

dusl::FResult
dusl::get_type(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok) {
    /// signature type(type)
    auto r = verifyArgsCount(args_node.args.size(), 1, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }
    return FResult::createResult(std::make_shared<dusl::StringObject>(args_node.args[0]->getTypeString(), args_node.args[0]->tok), tok);
}

dusl::FResult
dusl::get_time(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok) {
    auto r = verifyArgsCount(args_node.args.size(), 0, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }
    // Get the current time point
    auto now = std::chrono::high_resolution_clock::now();

    // Convert it to milliseconds
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds >(now);

    // Return the number of ticks of the duration
    return FResult::createResult(
        std::make_shared<dusl::IntObject>(now_ms.time_since_epoch().count(), tok),
        tok
    );
}

dusl::FResult
dusl::get_elapsed_time_ms(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok) {
    /// signature getElapsedTimeMS(start, end)
    auto r = verifyArgsCount(args_node.args.size(), 2, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    int64_t start = 0;
    int64_t end = 0;
    if (auto intObject = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[0]))
        start = intObject->value;

    if (auto intObject = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[1]))
        end = intObject->value;

    // Convert the integers to duration objects
    auto start_ms = std::chrono::milliseconds (start);
    auto end_ms = std::chrono::milliseconds (end);

    // Calculate the difference in duration
    auto duration = end_ms - start_ms;

    return FResult::createResult(
            std::make_shared<dusl::IntObject>(duration.count(), tok),
            tok
    );
}

dusl::FResult
dusl::get_elapsed_time_ns(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok) {
    /// signature getElapsedTimeMS(start, end)
    auto r = verifyArgsCount(args_node.args.size(), 2, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    int64_t start = 0;
    int64_t end = 0;
    if (auto intObject = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[0]))
        start = intObject->value;

    if (auto intObject = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[1]))
        end = intObject->value;

    // Convert the integers to duration objects
    auto start_ms = std::chrono::nanoseconds(start);
    auto end_ms = std::chrono::nanoseconds(end);

    // Calculate the difference in duration
    auto duration = end_ms - start_ms;

    return FResult::createResult(std::make_shared<dusl::IntObject>(duration.count(), tok), tok);
}
// fill a list with 0
dusl::FResult
dusl::fill_list(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok) {
    /// signature fillList(list, count)
    auto r = verifyArgsCount(args_node.args.size(), 2, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }
    if (auto listObject = std::dynamic_pointer_cast<dusl::ListObject>(args_node.args[0])) {
        int64_t count = 0;
        if (auto intObject = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[1])) {
            count = intObject->value;
        }
        while (listObject->items.size() < count) {
            listObject->items.push_back(std::make_shared<dusl::IntObject>(0, tok));
        }

        return FResult::createResult(listObject, tok);
    }
}

dusl::FResult
dusl::hash_object(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token& tok) {
    /// hash(object)
    auto r = verifyArgsCount(args_node.args.size(), 1, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    return FResult::createResult(std::make_shared<dusl::IntObject>(std::hash<std::string>()(args_node.args[0]->toString()), tok), tok);
}

dusl::FResult
dusl::random_int(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token &tok) {
    /// rnd(int)
    auto r = verifyArgsCount(args_node.args.size(), 1, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    if (auto intObject = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[0])) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist_rand(0,intObject->value);
        return FResult::createResult(std::make_shared<dusl::IntObject>(dist_rand(rng), tok), tok);
    }

    return FResult::createError(RunTimeError, "rnd: argument must be an integer", tok);
}

dusl::FResult dusl::system(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token &tok) {
    /// system() -> int
    auto r = verifyArgsCount(args_node.args.size(), 1, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    if (auto strObject = std::dynamic_pointer_cast<dusl::StringObject>(args_node.args[0])) {
        auto res = std::system(strObject->value.c_str());
        return FResult::createResult(std::make_shared<dusl::IntObject>(res, tok), tok);
    }

    return FResult::createError(RunTimeError, "system: argument must be an string", tok);
}

dusl::FResult dusl::exit_dusl(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token &tok) {
    /// exit()
    auto r = verifyArgsCount(args_node.args.size(), 0, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    std::exit(0);
}

dusl::FResult dusl::error_dusl(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token &tok) {
    /// error()
    auto r = verifyArgsCount(args_node.args.size(), 1, tok);
    if (r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    if (const auto str = std::dynamic_pointer_cast<dusl::StringObject>(args_node.args[0])) {
        return FResult::createError(RunTimeError, str->value, tok);
    }

    return FResult::createError(RunTimeError, "error: takes string as an argument", tok);
}

dusl::FResult dusl::math_round(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token &tok) {
    /// round(value)
    if (const auto r = verifyArgsCount(args_node.args.size(), 1, tok); r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    if (auto _ = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[0])) {
        return FResult::createResult(args_node.args[0], tok);
    }
    if (const auto floatObj = std::dynamic_pointer_cast<dusl::FloatObject>(args_node.args[0])) {
        auto res = std::round(floatObj->value);
        return FResult::createResult(std::make_shared<dusl::IntObject>(res, tok), tok);
    }


    return FResult::createError(
        RunTimeError,
        "round: argument must be an integer or a float",
        tok);
}

dusl::FResult dusl::math_floor(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token &tok) {
    /// floor(value)
    if (const auto r = verifyArgsCount(args_node.args.size(), 1, tok); r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    if (auto _ = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[0])) {
        return FResult::createResult(args_node.args[0], tok);
    }
    if (const auto floatObj = std::dynamic_pointer_cast<dusl::FloatObject>(args_node.args[0])) {
        auto res = std::floor(floatObj->value);
        return FResult::createResult(std::make_shared<dusl::IntObject>(res, tok), tok);
    }

    return FResult::createError(
        RunTimeError,
        "floor: argument must be an integer or a float",
        tok);
}

dusl::FResult dusl::from_char_code(dusl::Interpreter& visitor, ArgumentObject& args_node, const dusl::Token& tok) {
    /// fromCharCode(value)
	if (const auto r = verifyArgsCount(args_node.args.size(), 1, tok); r.has_value()) {
		return FResult::createError(RunTimeError, r.value(), tok);
	}

	if (auto intVal = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[0])) {
        auto res = std::string();
		res = char(intVal->value);
		return FResult::createResult(std::make_shared<dusl::StringObject>(res, tok), tok);
    } else if (auto listVal = std::dynamic_pointer_cast<dusl::ListObject>(args_node.args[0])) {
	    auto res = std::string();
	    for (auto& item : listVal->items) {
		    if (auto intVal = std::dynamic_pointer_cast<dusl::IntObject>(item)) {
			    res += char(intVal->value);
            }
            else {
			    return FResult::createError(RunTimeError, 
                    fmt::format("fromCharCode: argument must be an integer or a list of integers, got item {}", item->getTypeString()),
                    tok
                );
            }
	    }

	    return FResult::createResult(std::make_shared<dusl::StringObject>(res, tok), tok);
    }

    return FResult::createError(RunTimeError,
        fmt::format("fromCharCode: argument must be an integer or a list of integers, got {}", args_node.args[0]->getTypeString()),
        tok
    );
}
