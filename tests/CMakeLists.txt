cmake_minimum_required(VERSION 3.15)

project(balance-tests)

include(end2end.cmake)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_executable(unit_tests
        FrontendTests.cpp
    )

    target_link_libraries(unit_tests PRIVATE
        Frontend
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(unit_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_compile_definitions(unit_tests PRIVATE BALANCE_TESTING_)
    target_compile_options(unit_tests PRIVATE ${BALANCE_COMPILE_OPTIONS})

    target_compile_definitions(unit_tests PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    )

    add_test(NAME UnitTests COMMAND unit_tests)
endif()
