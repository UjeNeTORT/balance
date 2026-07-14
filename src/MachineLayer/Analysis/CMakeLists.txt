add_library(Analysis STATIC
    RPOTraversal.cpp
    LiveRanges.cpp
)

target_include_directories(Analysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Analysis PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(Analysis PUBLIC ${PROJECT_SOURCE_DIR}/src/MachineLayer)
