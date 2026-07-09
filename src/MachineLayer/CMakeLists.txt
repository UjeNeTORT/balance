cmake_minimum_required(VERSION 3.15)

add_executable(balancc CodeGen.cpp)

add_library(MachineLayer STATIC
    MachineFunction.cpp
    MachineBB.cpp
    MachineInst.cpp
    MachineOperand.cpp
)

add_library(MIRInfo STATIC
    MIROpcodes.cpp
)

target_link_libraries(balancc MachineLayer MIRInfo Utils)

target_include_directories(balancc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
