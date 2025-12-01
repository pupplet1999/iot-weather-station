cmake_minimum_required(VERSION 3.16)
project(iot_weather_station)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
add_compile_definitions(CSV_PATH="${CMAKE_CURRENT_SOURCE_DIR}/weather_log.csv")

add_executable(main
        main.cpp
        api_pull.hpp
        gather_weather.hpp
)

target_link_libraries(main
        CURL::libcurl
        nlohmann_json::nlohmann_json
)