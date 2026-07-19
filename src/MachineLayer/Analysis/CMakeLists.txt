add_library(Analysis STATIC
    Liveness.cpp
)

target_link_libraries(Analysis PRIVATE RISCV Utils)

target_include_directories(Analysis INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(Analysis PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(Analysis PUBLIC ${PROJECT_SOURCE_DIR}/src/MachineLayer)
