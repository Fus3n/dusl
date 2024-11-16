#include <dusl/Interpreter.hpp>
#include <dusl/DObject.hpp>
#include <dusl/utils/dusl_core.hpp>
#include <cmath>

// Function to check if a number is prime
dusl::FResult is_prime(dusl::Interpreter& visitor,
                      dusl::ArgumentObject& args,
                      const dusl::Token& tok) {
    if (args.args.empty()) {
        return dusl::FResult::createError(
            dusl::NameError,
            "isPrime() requires one integer argument",
            tok
        );
    }

    auto* num_obj = dynamic_cast<dusl::IntObject*>(args.args[0].get());
    if (!num_obj) {
        return dusl::FResult::createError(
            dusl::TypeError,
            "isPrime() argument must be an integer",
            tok
        );
    }

    int64_t n = num_obj->value;
    if (n <= 1) return dusl::FResult::createResult(
        std::make_shared<dusl::BooleanObject>(false, tok),
        tok
    );
    
    if (n <= 3) return dusl::FResult::createResult(
        std::make_shared<dusl::BooleanObject>(true, tok),
        tok
    );

    if (n % 2 == 0 || n % 3 == 0) return dusl::FResult::createResult(
        std::make_shared<dusl::BooleanObject>(false, tok),
        tok
    );

    for (int64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return dusl::FResult::createResult(
            std::make_shared<dusl::BooleanObject>(false, tok),
            tok
        );
    }

    return dusl::FResult::createResult(
        std::make_shared<dusl::BooleanObject>(true, tok),
        tok
    );
}


extern "C" __declspec(dllexport) bool dusl_init_module(dusl::Interpreter& visitor) {
    // Register all math functions
    dusl::createFunction(visitor, 
                        "isPrime",
                        &is_prime,
                        "isPrime(n): Returns true if n is prime");
    return true;
}