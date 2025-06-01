#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <openssl/sha.h>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

json check_value_type(const std::string& encoded_value, size_t& index);

std::string sha1(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);

    std::ostringstream result;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
        result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    return result.str();
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
    json dictionarie = json::object();

    while (encoded_value[index] != 'e') {
        json key = check_value_type(encoded_value, index);
        index++;

        //std::cerr << "at d: " <<encoded_value.substr(index) << '\n';
        
        json val = check_value_type(encoded_value, index);
        index++;
        dictionarie[key] = val;
    }

    return json(dictionarie);
}

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

json decode_bencoded_value(const std::string& encoded_value) {
    size_t index { 0 };
    return check_value_type(encoded_value, index);
}

int bencode(const json& value, std::string& encoded_value){
    switch (value.type())
    {
    case json::value_t::string:
        encoded_value.append(std::to_string(value.get<std::string>().size()) 
                        + ":" 
                        + value.get<std::string>());
        break;
    
    case json::value_t::array:
        encoded_value.append("l");
        for(int i = 0; i < value.size(); ++i) {
            bencode(value[i], encoded_value);
        }
        encoded_value.append("e");
        break;

    case json::value_t::object:
        encoded_value.append("d");
        for(auto& [key, val] : value.items()) {
            bencode(key, encoded_value);
            bencode(val, encoded_value);
        }
        encoded_value.append("e");
        break;

    case json::value_t::number_integer:
        encoded_value.append("i" 
                        + std::to_string(value.get<json::number_integer_t>()) 
                        + "e");
        break;

    default:
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
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

        std::string encoded_value = argv[2];
        json decoded_value = decode_bencoded_value(encoded_value);

        std::cout << decoded_value.dump() << std::endl;

    }else if (command == "info"){
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " path to .torrent file" << std::endl;
            return 1;
        }

        std::ifstream file(argv[2], std::ios::binary);
        std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());


        file.close();

        json decoded_value = decode_bencoded_value(fileContent);

        json info = decoded_value["info"];

        std::cout << "Tracker URL: " << decoded_value["announce"].get<std::string>() << std::endl;
        std::cout << "Length: " << info["length"] << std::endl;

        std::string out{};
        bencode(info, out);
        std::cout << "Info Hash: " << sha1(out) << std::endl;

    } else if (command == "test") {
        json t = json::object();
        t["a"] = 123;
        std::string out{};
        bencode(t, out);
        std::cout << out << std::endl;
    

    } else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
