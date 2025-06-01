#include <string>
#include <iostream>

#include "lib/nlohmann/json.hpp"
#include "include/Encoder.hpp"

using json = nlohmann::json;

namespace encoder
{

void encodeStr(const json& value, std::string& encoded_value) {

    encoded_value.append(std::to_string(value.get<std::string>().size()) 
                        + ":" 
                        + value.get<std::string>());

}

void encodeInt(const json& value, std::string& encoded_value) {

    encoded_value.append("i" 
                        + std::to_string(value.get<json::number_integer_t>()) 
                        + "e");

}

void encodeList(const json& value, std::string& encoded_value) {

    encoded_value.append("l");
    for(int i = 0; i < value.size(); ++i) {
        bencode(value[i], encoded_value);
    }
    encoded_value.append("e");

}

void encodeDic(const json& value, std::string& encoded_value) {

    encoded_value.append("d");
    for(auto& [key, val] : value.items()) {
        bencode(key, encoded_value);
        bencode(val, encoded_value);
    }
    encoded_value.append("e");
}


} // namespace encoder


int bencode(const json& value, std::string& encoded_value){
    switch (value.type())
    {
    case json::value_t::string:
        encoder::encodeStr(value, encoded_value);
        break;
    
    case json::value_t::array:
        encoder::encodeList(value, encoded_value);
        break;

    case json::value_t::object:
        encoder::encodeDic(value, encoded_value);
        break;

    case json::value_t::number_integer:
        encoder::encodeInt(value, encoded_value);
        break;

    default:
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}