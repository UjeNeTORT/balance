cmake_minimum_required(VERSION 3.15)

add_library(Traversals STATIC
    RPOTraversal.cpp
)

target_link_libraries(Traversals PRIVATE Utils MachineLayer)

target_include_directories(Traversals PRIVATE ${PROJECT_SOURCE_DIR}/src)

