cmake_minimum_required(VERSION 3.15)

add_library(UniversalAnalysis STATIC
    RPOTraversal.cpp
    DomTree.cpp
    DomFrontier.cpp
)

target_link_libraries(UniversalAnalysis PRIVATE Utils MachineLayer)

target_include_directories(UniversalAnalysis PRIVATE ${PROJECT_SOURCE_DIR}/src)

