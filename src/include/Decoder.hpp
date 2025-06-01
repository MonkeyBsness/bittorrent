#pragma once

#include "../lib/nlohmann/json.hpp"

using json = nlohmann::json;

json decode_bencoded_value(const std::string& encoded_value);

namespace decoder
{

json check_value_type(const std::string& encoded_value, size_t& index);

json decode_string(const std::string& encoded_value, size_t& index);

json decode_int(const std::string& encoded_value, size_t& index);

json decode_list(const std::string& encoded_value, size_t& index);

json decode_dictionarie(const std::string& encoded_value, size_t& index);

} // namespace decoder
