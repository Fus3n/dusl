#include <chrono>
#include <dusl/utils/dusl_standard.hpp>
#include <dusl/utils/dusl_core.hpp>
#include <dusl/utils/common.h>
#include <thread>
#include <random>

#include <filesystem>

namespace fs = std::filesystem;

bool dusl::loadSTL(dusl::Interpreter &visitor) {
    auto dusl_true = dusl::createBool(true);
    auto dusl_false = dusl::createBool(false);

    // set global variables
    dusl::addGlobalVariable(visitor, "true",  &dusl_true);
    dusl::addGlobalVariable(visitor, "false",  &dusl_false);

    // set global functions
    dusl::createFunction(visitor, "println", &print_line, "println(object): print something to console with a newline");
    dusl::createFunction(visitor, "print", &print_same_line, "print(object): something to console without a newline");
    dusl::createFunction(visitor, "readLine", &read_line, "readLine(): pause terminal and get input as string");
    dusl::createFunction(visitor, "type", &get_type, "type(object): get type of the object passed as a string");
    dusl::createFunction(visitor, "getTime", &get_time, "getTime(): returns current time in ms");
    dusl::createFunction(visitor, "getElapsedTimeMS", &get_elapsed_time_ms, "getElapsedTimeMS(): returns time since last time in milliseconds");
    dusl::createFunction(visitor, "getElapsedTimeNS", &get_elapsed_time_ns, "getElapsedTimeNS(): returns time since last time in nanoseconds");
    dusl::createFunction(visitor, "fillList", &fill_list, "fillList(list, count): fills the provided list with zero with the amount provided");
    dusl::createFunction(visitor, "fromCharCode", &from_char_code, "fromCharCode(int or list): converts the provided list or integer with ascii codes into it's string representation");
    dusl::createFunction(visitor, "hash", &hash_object, "hash(object): returns hash for the given object, throws error if unhashable object is provided");
    dusl::createFunction(visitor, "rnd", &random_int, "rnd(max): returns a random number between 0 and max");
    dusl::createFunction(visitor, "round", &math_round, "round(value): returns rounded value");
    dusl::createFunction(visitor, "floor", &math_floor, "floor(value): returns floored value");
    dusl::createFunction(visitor, "sleep", &time_sleep, "sleep(ms): sleep for a specific amount of time (ms)");
    dusl::createFunction(visitor, "system", &system, "system(command): run system command strings");
    dusl::createFunction(visitor, "error", &error_dusl, "error(msg): throws an error with message");
    dusl::createFunction(visitor, "parseJSON", &parse_json, "error(msg): throws an error with message");
    dusl::createFunction(visitor, "exit", &exit_dusl, "exit(): exit dusl");

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
    if (const auto listObject = std::dynamic_pointer_cast<dusl::ListObject>(args_node.args[0])) {
        int64_t count = 0;
        if (const auto intObject = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[1])) {
            count = intObject->value;
        }
        while (listObject->items.size() < count) {
            listObject->items.push_back(std::make_shared<dusl::IntObject>(0, tok));
        }

        return FResult::createResult(listObject, tok);
    }

    return FResult::createError(
        TypeError,
        "fillList expects a list as it's first argument",
        tok
    );
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

dusl::FResult dusl::time_sleep(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token &tok) {
    /// sleep(value)
    if (const auto r = verifyArgsCount(args_node.args.size(), 1, tok); r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    if (auto intObj = std::dynamic_pointer_cast<dusl::IntObject>(args_node.args[0])) {
        std::this_thread::sleep_for(std::chrono::milliseconds(intObj->value));
        return FResult::createResult(std::make_shared<dusl::NoneObject>(dusl::Token()), tok);
    }

    return FResult::createError(
            RunTimeError,
            "sleep: argument must be an integer (milliseconds)",
            tok);
}

dusl::FResult dusl::parse_json(dusl::Interpreter &visitor, ArgumentObject &args_node, const dusl::Token &tok) {
    /// parseJSON(value: str)
    if (const auto r = verifyArgsCount(args_node.args.size(), 1, tok); r.has_value()) {
        return FResult::createError(RunTimeError, r.value(), tok);
    }

    if (const auto strObj = std::dynamic_pointer_cast<dusl::StringObject>(args_node.args[0])) {
        return DictionaryObject::loadJson(strObj->value, visitor, tok);
    }

    return FResult::createError(
        RunTimeError,
        "parseJson: only accepts single string argument",
        tok
    );
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

dusl::DFile::DFile(std::string _file_name) {
    file_name = _file_name;
}

dusl::DBaseStruct::StructCreationResult dusl::DFile::init_(dusl::ArgumentObject &args_node) {

    auto err_res = verifyArgsCount(args_node.args.size(), 1, args_node.tok);
    if (err_res.has_value()) {
        return StructCreationResult(RunTimeError, err_res.value(), args_node.tok);
    }

    auto file_name_obj = args_node.args[0];
    return StructCreationResult(std::make_shared<DFile>(file_name_obj->toString()));
}

std::string dusl::DFile::getTypeString() {
    return getClassName();
}

dusl::FResult dusl::DFile::exists(std::shared_ptr<DFile> cls, dusl::Interpreter &visitor,
                                  const std::shared_ptr<dusl::FunctionCallNode> &fn_node) {

   auto err_res = verifyArgsCount(fn_node->args_node.args.size(), 0, fn_node->tok);
   if (err_res.has_value()) {
            return FResult::createError(RunTimeError, err_res.value(), fn_node->tok);
   }

    auto res = createBool(fs::exists(cls->file_name));
    return FResult::createResult(std::make_shared<BooleanObject>(res), fn_node->tok);
}

dusl::FResult dusl::DFile::file_size(std::shared_ptr<DFile> cls, dusl::Interpreter &visitor,
                                     const std::shared_ptr<dusl::FunctionCallNode> &fn_node) {
    auto err_res = verifyArgsCount(fn_node->args_node.args.size(), 0, fn_node->tok);
    if (err_res.has_value()) {
        return FResult::createError(RunTimeError, err_res.value(), fn_node->tok);
    }

    auto res = dusl::createInt(fs::file_size(cls->file_name));
    return FResult::createResult(std::make_shared<IntObject>(res), fn_node->tok);
}

dusl::FResult dusl::DFile::read(std::shared_ptr<DFile> cls, dusl::Interpreter &visitor,
                                const std::shared_ptr<dusl::FunctionCallNode> &fn_node) {
    auto err_res = verifyArgsCount(fn_node->args_node.args.size(), 0, fn_node->tok);
    if (err_res.has_value()) {
        return FResult::createError(RunTimeError, err_res.value(), fn_node->tok);
    }
    auto contents = read_file(cls->file_name);
    auto res = createString(contents);
    return FResult::createResult(std::make_shared<StringObject>(res), fn_node->tok);
}

std::vector<std::shared_ptr<dusl::StructProxyObject>> dusl::loadObjects(dusl::Interpreter &visitor) {

    std::vector<std::shared_ptr<dusl::StructProxyObject>> items;

    auto init = reinterpret_cast<dusl::CreatorFunction>(&dusl::DFile::init_);
    auto file_struct = dusl::createStruct(visitor, "File", init, "File: Handles opening and reading files");
    file_struct->addFunc("read", reinterpret_cast<dusl::ExposedFunctions>(&dusl::DFile::read));
    file_struct->addFunc("exists", reinterpret_cast<dusl::ExposedFunctions>(&dusl::DFile::exists));
    file_struct->addFunc("size", reinterpret_cast<dusl::ExposedFunctions>(&dusl::DFile::file_size));

    items.push_back(file_struct);

    return items;
}