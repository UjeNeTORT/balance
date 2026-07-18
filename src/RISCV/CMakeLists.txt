cmake_minimum_required(VERSION 3.15)

add_library(RISCV STATIC RISCV.cpp)

target_include_directories(RISCV PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

