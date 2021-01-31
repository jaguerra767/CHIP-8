cmake_minimum_required(VERSION 3.19)

project(Chip8)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(Chip8 src/chip8.cpp src/main.cpp src/platform.cpp)
target_compile_options(Chip8 PRIVATE -Wall)
target_link_libraries(Chip8 PRIVATE SDL2::SDL2)
target_include_directories(Chip8 PRIVATE ${PROJECT_SOURCE_DIR}/include)
