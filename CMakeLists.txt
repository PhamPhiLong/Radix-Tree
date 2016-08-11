cmake_minimum_required(VERSION 3.5)
project(codejam)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Werror")

set(SOURCE_FILES main.cpp)
add_executable(radix_tree ${SOURCE_FILES})