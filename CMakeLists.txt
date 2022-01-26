cmake_minimum_required(VERSION 3.22)

project(fwoop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()
add_subdirectory(basis)
add_subdirectory(http)
add_subdirectory(examples)
