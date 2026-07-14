cmake_minimum_required(VERSION 3.15)

add_executable(balanccir
    Instruction.cpp
    IR.cpp
)

target_link_libraries(balanccir Utils)

