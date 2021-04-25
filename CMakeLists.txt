cmake_minimum_required(VERSION 3.10)
project(pars-sql-xml C)

set(CMAKE_C_STANDARD 99)

add_executable(pars-sql-xml pars_in.c)
target_link_libraries(pars-sql-xml -lpq)
include_directories(/usr/include/libxml2)
target_link_libraries(pars-sql-xml -lxml2)