cmake_minimum_required(VERSION 2.8)

project("utils")

set (CMAKE_CXX_STANDARD 11)

find_package(Threads)

add_library(utils STATIC "logger.cpp" "event_handler.cpp")

set_target_properties(utils PROPERTIES PUBLIC_HEADER "bits_reader.h;bits_swap.h;bits_writer.h;event_handler.h;logger.h;log.h;now.h;object_pool.h;object_queue.h")

install(TARGETS utils
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)