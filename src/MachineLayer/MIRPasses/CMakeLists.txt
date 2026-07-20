add_library(MIRPasses STATIC
    Pass.cpp
    VerifierPass.cpp
    LivenessAnalysis.cpp
    PhiElimination.cpp
    LinearScanRAL.cpp
    DCE.cpp
)

target_include_directories(MIRPasses PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(MIRPasses PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(MIRPasses PUBLIC ${PROJECT_SOURCE_DIR}/src/MachineLayer)
target_include_directories(MIRPasses PUBLIC ${PROJECT_SOURCE_DIR}/src/MachineLayer/Analysis)
