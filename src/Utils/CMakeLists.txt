cmake_minimum_required(VERSION 3.15)

add_library(Utils STATIC Utils.cpp)
target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
