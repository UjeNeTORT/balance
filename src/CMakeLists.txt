cmake_minimum_required(VERSION 3.15)

add_subdirectory(Utils)
add_subdirectory(UniversalAnalysis)
add_subdirectory(MachineLayer)
add_subdirectory(RISCV)
add_subdirectory(IR)
add_subdirectory(AST)
add_subdirectory(Frontend)
add_subdirectory(Grammar)
add_subdirectory(IR2MIR)

add_executable(balance main.cpp)

target_link_libraries(balance Frontend Grammar BalanceAST Utils IR IR2MIR MachineLayer MIRPasses Analysis MIRInfo)
target_include_directories(balance PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
