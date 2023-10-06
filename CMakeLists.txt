cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(keyboard-cleaner)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_executable(${PROJECT_NAME} ./src/main.c)
