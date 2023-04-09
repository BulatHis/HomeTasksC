cmake_minimum_required(VERSION 3.5)
project(HomeTaskC LANGUAGES CXX)

find_package(Threads REQUIRED)

add_executable(Threads Thread1.cpp Thread1.cpp)
target_link_libraries(Threads Threads::Threads)