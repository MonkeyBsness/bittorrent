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
#include "include/Decoder.hpp"
#include "include/Encoder.hpp"

using json = nlohmann::json;

std::string sha1(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);

    std::ostringstream result;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
        result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    return result.str();
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

    }else if (command == "info"){
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " path to .torrent file" << std::endl;
            return 1;
        }

        std::ifstream file(argv[2], std::ios::binary);
        std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        // std::cout << "main: "<< fileContent << std::endl;


        file.close();

        json decoded_value = decode_bencoded_value(fileContent);

        json info = decoded_value["info"];

        std::cout << "Tracker URL: " << decoded_value["announce"].get<std::string>() << std::endl;
        std::cout << "Length: " << info["length"] << std::endl;

        std::string out{};
        bencode(info, out);
        std::cout << "Info Hash: " << sha1(out) << std::endl;

    } else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
