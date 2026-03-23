#cmake_minimum_required(VERSION 3.10)
#project(coding C)
#
#set(CMAKE_C_STANDARD 11)
#
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
#
#add_executable(coding test.c)
#
#target_link_libraries(coding gomp)

cmake_minimum_required(VERSION 3.10)
project(coding C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

#add_executable(coding test.c)
#add_executable(coding sort.c)
#add_executable(coding matricMultiplicationSerial.c)
#add_executable(coding matrixMultiplicationParallel.c)
add_executable(coding calculatePrime.c)

#add_executable(coding add_Numbers.c)

target_link_libraries(coding gomp)
#target_link_libraries(coding gomp)