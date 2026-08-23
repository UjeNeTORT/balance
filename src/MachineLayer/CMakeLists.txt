cmake_minimum_required(VERSION 3.15)

add_library(MachineLayer STATIC
    MachineFunction.cpp
    MachineBB.cpp
    MachineInst.cpp
    MachineOperand.cpp
    PassManager.cpp
    Register.cpp
    AsmEmitter.cpp
)

add_library(MIRInfo STATIC
    MIROpcodes.cpp
)

target_link_libraries(MIRInfo PRIVATE RISCV Utils)
target_include_directories(MIRInfo PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(MachineLayer MIRPasses RISCV Utils Analysis UniversalAnalysis)
target_include_directories(MachineLayer PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(MIRPasses)
add_subdirectory(Analysis)
