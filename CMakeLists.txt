cmake_minimum_required(VERSION 3.14)
project(chip8)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(chip8 chip8.h chip8.cpp main.cpp)
target_link_libraries(chip8 ${SDL2_LIBRARIES})