cmake_minimum_required(VERSION 3.27)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(keyboard-cleaner VERSION 0.1.0 LANGUAGES C CXX)
add_compile_options(-Wall -Wextra -Wpedantic -g)
include_directories("./include")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# log.c
add_library(log.c STATIC ./include/log.c/log.c)
target_compile_options(log.c PRIVATE -DLOG_USE_COLOR)

# qt
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

if(${APPLE})
  find_library(CARBON_LIBRARY Carbon)
  set(OS_SPECIFIC_LIBRARIES_SERVICE ${CARBON_LIBRARY})
elseif(${LINUX})
  find_library(EVDEV_LIBRARY evdev)
  set(OS_SPECIFIC_LIBRARIES_TRAY ${EVDEV_LIBRARY})
endif()

configure_file(${CMAKE_SOURCE_DIR}/keyboard-cleaner.png ${CMAKE_BINARY_DIR}/keyboard-cleaner.png COPYONLY)

if(${APPLE})
  qt_add_executable(${PROJECT_NAME} ./src/tray/main.cpp ./src/tray/tray.cpp ./src/tray/command_runner.cpp)
  target_link_libraries(${PROJECT_NAME} PRIVATE stdc++ log.c Qt6::Widgets ${OS_SPECIFIC_LIBRARIES_TRAY})

  add_executable(${PROJECT_NAME}-service ./src/service/service.cpp ./src/service/block_keyboard.cpp ./src/service/pipe_reader.cpp)
  target_link_libraries(${PROJECT_NAME}-service PRIVATE stdc++ log.c ${OS_SPECIFIC_LIBRARIES_SERVICE})
elseif(${LINUX})
  qt_add_executable(${PROJECT_NAME} ./src/tray/main.cpp ./src/tray/tray.cpp ./src/tray/command_runner.cpp ./src/service/service.cpp ./src/service/block_keyboard.cpp ./src/service/pipe_reader.cpp)
  target_link_libraries(${PROJECT_NAME} PRIVATE stdc++ log.c Qt6::Widgets ${OS_SPECIFIC_LIBRARIES_TRAY})
elseif(${WIN32})
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    qt_add_executable(${PROJECT_NAME} ./src/tray/main.cpp ./src/tray/tray.cpp ./src/tray/command_runner.cpp ./src/service/service.cpp ./src/service/block_keyboard.cpp ./src/service/pipe_reader.cpp)
  else()
    qt_add_executable(${PROJECT_NAME} WIN32 ./src/tray/main.cpp ./src/tray/tray.cpp ./src/tray/command_runner.cpp ./src/service/service.cpp ./src/service/block_keyboard.cpp ./src/service/pipe_reader.cpp)
  endif()

  target_link_libraries(${PROJECT_NAME} PRIVATE log.c Qt6::Widgets)
endif()
