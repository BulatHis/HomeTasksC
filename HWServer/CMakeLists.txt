cmake_minimum_required(VERSION 3.10)

project(Server)

find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(Server Server.cpp)
target_link_libraries(Server pthread  ${Boost_LIBRARIES})
target_link_libraries(Server Boost::filesystem)