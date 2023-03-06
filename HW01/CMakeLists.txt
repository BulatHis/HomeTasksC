cmake_minimum_required(VERSION 3.5)
project(homework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(HomeWork homework.cpp)

find_package(Catch2)
target_link_libraries(HomeWork Catch2::Catch2WithMain)