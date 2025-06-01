#pragma once 

#include "../lib/nlohmann/json.hpp"

using json = nlohmann::json;

int bencode(const json& value, std::string& encoded_value);

