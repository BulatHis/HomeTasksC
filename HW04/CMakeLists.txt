cmake_minimum_required(VERSION 3.5)
project(HomeTaskC LANGUAGES CXX)
find_package(Boost REQUIRED COMPONENTS filesystem program_options)

add_executable(Work4 homework04.cpp)
target_link_libraries(Work4 Boost::filesystem)
target_link_libraries(Work4 Boost::program_options)