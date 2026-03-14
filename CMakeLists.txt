cmake_minimum_required(VERSION 3.10)
project(coding C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

add_executable(coding test.c)

target_link_libraries(coding gomp)