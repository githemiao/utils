cmake_minimum_required(VERSION 2.8)

project("utils")

set (CMAKE_CXX_STANDARD 11)

find_package(Threads)

add_library(utils STATIC "logger.cpp" "event_handler.cpp")