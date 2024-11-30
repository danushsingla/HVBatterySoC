cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(project3_datastructures)

set(PROJECT3_SOURCES src/main.cpp src/CSVReader.cpp)

add_executable(project3_datastructures ${PROJECT3_SOURCES})
