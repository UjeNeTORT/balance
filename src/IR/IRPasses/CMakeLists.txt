add_library(IRPasses STATIC
    SSAConstruction.cpp
)

target_link_libraries(IRPasses PRIVATE IR UniversalAnalysis Utils)

target_include_directories(IRPasses PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(IRPasses PRIVATE ${PROJECT_SOURCE_DIR}/src/IR)
