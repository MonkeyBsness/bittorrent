cmake_minimum_required(VERSION 3.13)

project(bittorrent-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

find_package(OpenSSL REQUIRED)

add_executable(bittorrent ${SOURCE_FILES})

target_include_directories(bittorrent PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(bittorrent PRIVATE OpenSSL::Crypto)
