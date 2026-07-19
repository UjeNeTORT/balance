add_library(MIRPasses STATIC
    Pass.cpp
    VerifierPass.cpp
    LivenessAnalysis.cpp
    PhiElimination.cpp
    LinearScanRAL.cpp
)

target_link_libraries(MIRPasses PRIVATE Traversals RISCV Utils)

target_include_directories(MIRPasses PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(MIRPasses PRIVATE ${PROJECT_SOURCE_DIR}/src/MachineLayer)
target_include_directories(MIRPasses PRIVATE ${PROJECT_SOURCE_DIR}/src/MachineLayer/Analysis)
