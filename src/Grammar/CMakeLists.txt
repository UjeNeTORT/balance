cmake_minimum_required(VERSION 3.15)

include(FetchContent)

if(WIN32)
    include(FetchContent)

    FetchContent_Declare(
        winflexbison
        URL "https://github.com/lexxmark/winflexbison/releases/download/v2.5.25/win_flex_bison-2.5.25.zip"
    )
    FetchContent_MakeAvailable(winflexbison)

    set(BISON_EXECUTABLE "${winflexbison_SOURCE_DIR}/win_bison.exe" CACHE FILEPATH "Path to win_bison" FORCE)
    set(FLEX_EXECUTABLE "${winflexbison_SOURCE_DIR}/win_flex.exe" CACHE FILEPATH "Path to win_flex" FORCE)
endif()

find_package(BISON 3.8.2 REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(parser
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.yy
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
)

FLEX_TARGET(lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer.ll
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

ADD_FLEX_BISON_DEPENDENCY(lexer parser)

add_library(Grammar STATIC
    ${BISON_parser_OUTPUTS}
    ${FLEX_lexer_OUTPUTS}
)

target_include_directories(Grammar PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(Grammar PUBLIC BalanceAST)
