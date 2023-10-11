cmake_minimum_required(VERSION 3.27)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(keyboard-cleaner VERSION 0.1.0 LANGUAGES C CXX)
add_compile_options(-Wall -Wextra -Wpedantic -g)

find_package(PkgConfig REQUIRED)
include_directories("./include")

# log.c
add_library(log.c STATIC ./include/log.c/log.c)
target_compile_options(log.c PRIVATE -DLOG_USE_COLOR)

# qt
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

if(${APPLE})
  find_library(CARBON_LIBRARY Carbon)
  set(OS_SPECIFIC_LIBRARIES ${CARBON_LIBRARY})
endif()

qt_add_executable(${PROJECT_NAME} ./src/main.cpp ./src/os_specific.cpp ./src/service.cpp ./src/tray.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE stdc++ log.c Qt6::Widgets ${OS_SPECIFIC_LIBRARIES})
