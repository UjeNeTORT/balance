cmake_minimum_required(VERSION 3.15)

project(balance_compiler CXX)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified, default to Release")
    set(CMAKE_BUILD_TYPE Release)
else()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()


if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wpedantic -g -O0 -DDEBUG")
endif()



add_subdirectory(src)
