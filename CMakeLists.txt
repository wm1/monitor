cmake_minimum_required(VERSION 3.0)

project(Monitor)

include_directories(inc)
add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_subdirectory(findapp)
