cmake_minimum_required(VERSION 3.16)
project(elevator_simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(elevator_simulator
    src/main.cpp
    src/console.cpp
    src/building.cpp
    src/elevator.cpp
    src/dispatcher.cpp
    src/simulator.cpp
)

target_include_directories(elevator_simulator PRIVATE include)

if(MSVC)
    target_compile_options(elevator_simulator PRIVATE /W4)
else()
    target_compile_options(elevator_simulator PRIVATE -Wall -Wextra -pedantic)
endif()
