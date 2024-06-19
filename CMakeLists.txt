cmake_minimum_required(VERSION 3.10)
project(HttpClient)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(HttpClient main.cpp HttpClient.cpp HttpOptions.cpp)

find_package(CURL REQUIRED)
target_link_libraries(HttpClient CURL::libcurl)