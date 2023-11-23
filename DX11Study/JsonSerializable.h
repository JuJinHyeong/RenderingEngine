#pragma once
#include "json.hpp"

class JsonSerializable {
public:
    using json = nlohmann::json;
    virtual json ToJson() const noexcept = 0;
    virtual ~JsonSerializable() {}
};