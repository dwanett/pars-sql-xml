cmake_minimum_required(VERSION 3.10)
project(diplom C)

set(CMAKE_C_STANDARD 99)

add_executable(diplom main.c)
target_link_libraries(diplom -lpq)
include_directories(/usr/include/libxml2)
target_link_libraries(diplom -lxml2)