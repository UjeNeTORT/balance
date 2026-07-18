cmake_minimum_required(VERSION 3.15)

add_library(IR STATIC
    Instruction.cpp
    IR.cpp
)

target_link_libraries(IR INTERFACE Utils)

target_include_directories(IR INTERFACE ${CMAKE_CURRENT_SOURCE_SIR})

