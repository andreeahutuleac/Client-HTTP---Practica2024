cmake_minimum_required(VERSION 3.10)
project(HttpClient)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(HttpClient main.cpp HttpClient.cpp HttpOptions.cpp)
include_directories(/home/andreea/Desktop/git/Client-HTTP---Practica2024/build/jsoncpp/include)
include_directories(/home/andreea/Desktop/git/Client-HTTP---Practica2024/json-develop/include)

set ( SOURCE FILES
src/main.cpp
src/HttpClient.cpp
src/HttpOptions.cpp
include/HttpClient.h
include/HttpOptions.h
include/json/json.h
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

find_package(CURL REQUIRED)
target_link_libraries(HttpClient CURL::libcurl jsoncpp)
