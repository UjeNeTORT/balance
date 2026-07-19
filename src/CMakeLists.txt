cmake_minimum_required(VERSION 3.15)

add_subdirectory(Utils)
add_subdirectory(Traversals)
add_subdirectory(MachineLayer)
add_subdirectory(RISCV)
add_subdirectory(IR)

add_executable(balance main.cpp)

target_link_libraries(balance Utils IR MachineLayer MIRPasses Analysis MIRInfo)
