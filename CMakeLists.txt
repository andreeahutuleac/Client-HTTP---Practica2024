cmake_minimum_required(VERSION 3.10)
project(HttpClient)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(HttpClient main.cpp HttpClient.cpp HttpOptions.cpp Weather.cpp)

include_directories(/home/andreea/Desktop/git/Client-HTTP---Practica2024/build/jsoncpp/include)
include_directories(/home/andreea/Desktop/git/Client-HTTP---Practica2024/json-develop/include)
include_directories(/home/andreea/Desktop/git/Client-HTTP---Practica2024/tinyxml2-master/)
include_directories(/usr/include)

set ( SOURCE FILES
src/main.cpp
src/HttpClient.cpp
src/HttpOptions.cpp
src/Weather.cpp
src/tinyxml2.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

find_package(CURL REQUIRED)
find_library(TINYXML2_LIBRARY tinyxml2 /usr/lib/x86_64-linux-gnu)
target_link_libraries(HttpClient CURL::libcurl jsoncpp ${TINYXML2_LIBRARY})


