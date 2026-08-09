cmake_minimum_required(VERSION 3.15)

add_library(BalanceAST STATIC
    Node.cpp
    OpNames.cpp
    ConstantFolding.cpp
    AstDumper.cpp
    AstDotDumper.cpp
    IRBuilder.cpp
)

target_link_libraries(BalanceAST PRIVATE Utils)

target_include_directories(BalanceAST PUBLIC ${CMAKE_SOURCE_DIR}/src)
