cmake_minimum_required(VERSION 3.15)

add_library(IR STATIC
    Instruction.cpp
    BasicBlock.cpp
    Function.cpp
    IR.cpp
)

target_link_libraries(IR PRIVATE Utils)

target_include_directories(IR PRIVATE ${PROJECT_SOURCE_DIR}/src)

