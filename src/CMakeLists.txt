cmake_minimum_required(VERSION 3.15)

add_subdirectory(Utils)
add_subdirectory(MachineLayer)
add_subdirectory(RISCV)
add_subdirectory(IR)

add_executable(balancc main.cpp)

target_link_libraries(balancc Utils IR MachineLayer MIRPasses Analysis MIRInfo)
