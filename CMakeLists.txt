cmake_minimum_required(VERSION 3.10)
project(GPSProcessor)

set(CMAKE_CXX_STANDARD 17)

add_executable(GPSProcessor
        src/main.cpp
        src/MetricConverter.cpp
        src/ImperialConverter.cpp
        include/ImperialConverter.h
        include/MetricConverter.h
)

target_include_directories(GPSProcessor PRIVATE include)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(GPSProcessor PRIVATE nlohmann_json::nlohmann_json)