#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

json decode_bencoded_value(const std::string& encoded_value, size_t& index);

json decode_string(const std::string& encoded_value, size_t& index){
    // Example: "5:hello" -> "hello"
    size_t colon_index = encoded_value.find(':', index);

    if (colon_index != std::string::npos) {
        std::string number_string = encoded_value.substr(index, colon_index);
        size_t number = std::atoll(number_string.c_str());
        std::string str = encoded_value.substr(colon_index + 1, number);
        index = index + number + 1;

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
        list.push_back(decode_bencoded_value(encoded_value, index));
        index++;
        //std::cerr << "index at dl: " << index << std::endl;
    }

    return list;
}

json decode_dictionarie(const std::string& encoded_value, size_t& index){
    index++;
    auto dictionarie = nlohmann::ordered_map<json, json>();

    while (encoded_value[index] != 'e') {
        json key = decode_bencoded_value(encoded_value, index);
        index++;
        json val = decode_bencoded_value(encoded_value, index);
        index++;
        dictionarie.insert({key, val});
    }

    return json(dictionarie);
}

json decode_bencoded_value(const std::string& encoded_value, size_t& index){

    if (std::isdigit(encoded_value[index])) {
        return decode_string(encoded_value, index);

    } else if (encoded_value[index] == 'i') {
        return decode_int(encoded_value, index);

    } else if (encoded_value[index] == 'l') {
        return decode_list(encoded_value, index);

    } else if (encoded_value[index] == 'd') {
        return decode_dictionarie(encoded_value, index);
        
    }else {
        throw std::runtime_error("Unhandled encoded value: " + encoded_value);
    }
}


json decode_bencoded_value(const std::string& encoded_value) {
    size_t index = 0;
    return decode_bencoded_value(encoded_value, index);
}

int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "decode") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }
        // You can use print statements as follows for debugging, they'll be visible when running tests.
        std::cerr << "Logs from your program will appear here!" << std::endl;

        // Uncomment this block to pass the first stage
        std::string encoded_value = argv[2];
        json decoded_value = decode_bencoded_value(encoded_value);
        std::cout << decoded_value.dump() << std::endl;
    } else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
