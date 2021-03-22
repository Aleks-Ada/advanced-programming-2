cmake_minimum_required(VERSION 3.10)
project(advanced-programming-2)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)