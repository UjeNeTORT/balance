cmake_minimum_required(VERSION 3.15)

include(FetchContent)

if(WIN32)
    FetchContent_Declare(
        winflexbison
        URL "https://github.com/lexxmark/winflexbison/releases/download/v2.5.25/win_flex_bison-2.5.25.zip"
    )
    FetchContent_MakeAvailable(winflexbison)

    set(BISON_EXECUTABLE "${winflexbison_SOURCE_DIR}/win_bison.exe" CACHE FILEPATH "Path to win_bison" FORCE)
    set(FLEX_EXECUTABLE "${winflexbison_SOURCE_DIR}/win_flex.exe" CACHE FILEPATH "Path to win_flex" FORCE)

elseif(UNIX)
    find_package(BISON 3.8.2 QUIET)

    if(NOT BISON_FOUND)
        set(LOCAL_BISON_DIR "${CMAKE_BINARY_DIR}/deps_install/bison")
        set(LOCAL_BISON_EXEC "${LOCAL_BISON_DIR}/bin/bison")

        if(NOT EXISTS "${LOCAL_BISON_EXEC}")
            message(STATUS "System Bison not found or too old. Fetching and compiling Bison 3.8.2 from source...")
            FetchContent_Declare(
                bison_src
                URL "https://ftp.gnu.org/gnu/bison/bison-3.8.2.tar.gz"
            )
            FetchContent_MakeAvailable(bison_src)

            execute_process(COMMAND ./configure --prefix=${LOCAL_BISON_DIR} WORKING_DIRECTORY "${bison_src_SOURCE_DIR}")
            execute_process(COMMAND make -j install WORKING_DIRECTORY "${bison_src_SOURCE_DIR}")
        endif()

        set(BISON_EXECUTABLE "${LOCAL_BISON_EXEC}" CACHE FILEPATH "Path to local bison" FORCE)
    endif()

    find_package(FLEX QUIET)

    if(NOT FLEX_FOUND)
        set(LOCAL_FLEX_DIR "${CMAKE_BINARY_DIR}/deps_install/flex")
        set(LOCAL_FLEX_EXEC "${LOCAL_FLEX_DIR}/bin/flex")

        if(NOT EXISTS "${LOCAL_FLEX_EXEC}")
            message(STATUS "System Flex not found. Fetching and compiling Flex 2.6.4 from source...")
            FetchContent_Declare(
                flex_src
                URL "https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz"
            )
            FetchContent_MakeAvailable(flex_src)

            execute_process(COMMAND ./configure --prefix=${LOCAL_FLEX_DIR} WORKING_DIRECTORY "${flex_src_SOURCE_DIR}")
            execute_process(COMMAND make -j install WORKING_DIRECTORY "${flex_src_SOURCE_DIR}")
        endif()

        set(FLEX_EXECUTABLE "${LOCAL_FLEX_EXEC}" CACHE FILEPATH "Path to local flex" FORCE)
    endif()

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
