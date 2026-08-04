cmake_minimum_required(VERSION 3.15)

add_library(IR2MIR STATIC
    MIRBuilder.cpp
)

target_link_libraries(IR2MIR PRIVATE Utils IR MIR Traversals)

target_include_directories(IR2MIR PRIVATE ${PROJECT_SOURCE_DIR}/src)

