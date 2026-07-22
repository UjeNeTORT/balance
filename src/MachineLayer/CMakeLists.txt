cmake_minimum_required(VERSION 3.15)

add_executable(balancc CodeGen.cpp)

add_library(MachineLayer STATIC
    MachineFunction.cpp
    MachineBB.cpp
    MachineInst.cpp
    MachineOperand.cpp
    PassManager.cpp
    Register.cpp
)

add_library(MIRInfo STATIC
    MIROpcodes.cpp
)

target_link_libraries(MIRInfo PRIVATE RISCV Utils)
target_include_directories(MIRInfo PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(MachineLayer MIRPasses RISCV Utils Analysis)
target_include_directories(MachineLayer PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(balancc MachineLayer MIRInfo MIRPasses Utils RISCV Analysis)
target_include_directories(balancc PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(MIRPasses)
add_subdirectory(Analysis)
