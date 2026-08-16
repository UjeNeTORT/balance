cmake_minimum_required(VERSION 3.15)

add_library(Frontend STATIC
    Driver.cpp
)

target_link_libraries(Frontend PUBLIC Grammar BalanceAST)
