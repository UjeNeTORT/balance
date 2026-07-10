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

target_include_directories(balancc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(balancc PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(MachineLayer PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(MIRInfo PRIVATE ${PROJECT_SOURCE_DIR}/src)
