cmake_minimum_required(VERSION 3.15)
project(SPRING2026CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(CURL CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(app vibecode.cpp)

target_link_libraries(app PRIVATE
        CURL::libcurl
        Crow::Crow
        nlohmann_json::nlohmann_json
        ws2_32
        mswsock
)