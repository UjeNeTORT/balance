cmake_minimum_required(VERSION 3.15)

add_library(frontend STATIC
    Driver.cpp
)

target_link_libraries(frontend PUBLIC Grammar BalanceAST)
