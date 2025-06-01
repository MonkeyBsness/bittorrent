#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

#include "lib/nlohmann/json.hpp"
#include "include/Decoder.hpp"

using json = nlohmann::json;

json decode_bencoded_value(const std::string& encoded_value) {
    size_t index { 0 };
    return decoder::check_value_type(encoded_value, index);
}

namespace decoder{

json check_value_type(const std::string& encoded_value, size_t& index) {

    if (std::isdigit(encoded_value[index])) {
        //std::cerr << "s" << std::endl;
        return decode_string(encoded_value, index);

    } else if (encoded_value[index] == 'i') {
        //std::cerr << "i" << std::endl;
        return decode_int(encoded_value, index);

    } else if (encoded_value[index] == 'l') {
        //std::cerr << "l" << std::endl;
        return decode_list(encoded_value, index);

    } else if (encoded_value[index] == 'd') {
        //std::cerr << "d" << std::endl;
        return decode_dictionarie(encoded_value, index);
        
    }else {
        throw std::runtime_error("Unhandled encoded value: " + encoded_value);
    }

}

json decode_string(const std::string& encoded_value, size_t& index){
    // Example: "5:hello" -> "hello"
    size_t colon_index = encoded_value.find(':', index);

    if (colon_index != std::string::npos) {
        std::string number_string = encoded_value.substr(index, colon_index);
        size_t number = std::atoll(number_string.c_str());
        std::string str = encoded_value.substr(colon_index + 1, number);
        index = colon_index + number;

        //std::cerr << "at s: " <<encoded_value.substr(index) << '\n';
        //std::cerr << "i: "<< index<< "num:"<<number << '\n';

        return json(str);
    } else {
        throw std::runtime_error("Invalid encoded value: " + encoded_value);
    }
}

json decode_int(const std::string& encoded_value, size_t& index){
    size_t end = encoded_value.find('e', index);
    index++;
    std::string num_str = encoded_value.substr(index, (end - index));
    int64_t num = std::atoll(num_str.c_str());
    index = end;
    return json(num);
}

json decode_list(const std::string& encoded_value, size_t& index){
    index++;
    json list = json::array();
    while (encoded_value[index] != 'e'){
        //std::cerr << "at dl: " <<encoded_value.substr(index) << '\n';
        list.push_back(check_value_type(encoded_value, index));
        index++;
        //std::cerr << "index at dl: " << index << std::endl;
    }

    return list;
}

json decode_dictionarie(const std::string& encoded_value, size_t& index){
    index++;
    auto dictionarie = nlohmann::ordered_map<json, json>();

    while (encoded_value[index] != 'e') {
        json key = check_value_type(encoded_value, index);
        index++;

        //std::cerr << "at d: " <<encoded_value.substr(index) << '\n';
        
        json val = check_value_type(encoded_value, index);
        index++;
        dictionarie.insert({key, val});
    }

    return json(dictionarie);
}

}