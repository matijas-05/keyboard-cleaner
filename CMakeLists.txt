cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(keyboard-cleaner VERSION 0.1.0)
find_package(PkgConfig REQUIRED)

# local libraries
include_directories("./include")

# gtk3 for tray
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# appindicator for tray
pkg_check_modules(APPINDICATOR3 REQUIRED appindicator3-0.1)
include_directories(${APPINDICATOR3_INCLUDE_DIRS})
link_directories(${APPINDICATOR3_LIBRARY_DIRS})

# log.c
add_library(log.c STATIC ./include/log.c/log.c)
target_compile_options(log.c PRIVATE -DLOG_USE_COLOR)

add_compile_options(-Wall -Wextra -Wpedantic -g)

add_executable(${PROJECT_NAME} ./src/tray.c)
target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES} ${APPINDICATOR3_LIBRARIES} log.c)

add_executable(${PROJECT_NAME}-service ./src/service.c)
target_link_libraries(${PROJECT_NAME}-service log.c)
