cmake_minimum_required(VERSION 3.12)

add_library(
    move_generator SHARED
    position.cpp 
    tables.cpp
    types.cpp
)

add_executable(Engine Engine.cpp EntryPoint.cpp Evaluate.cpp)

target_link_libraries(Engine PRIVATE move_generator)