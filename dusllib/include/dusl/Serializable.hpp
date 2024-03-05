#pragma once

#include "nlohmann/json.hpp"

class Serializable {
public:
    [[nodiscard]] virtual nlohmann::ordered_json toJson() const = 0;
    virtual ~Serializable() = default;
};