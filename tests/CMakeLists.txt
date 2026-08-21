cmake_minimum_required(VERSION 3.15)

project(balance-tests)

if (BUILD_END2END_TESTS)
    include(end2end.cmake)
endif()

if(BUILD_UNIT_TESTS)
    add_executable(unit_tests
        FrontendTests.cpp
    )

    target_link_libraries(unit_tests PRIVATE
        Frontend
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(unit_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_compile_options(unit_tests PRIVATE ${BALANCE_COMPILE_OPTIONS})

    target_compile_definitions(unit_tests PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    )

    gtest_discover_tests(unit_tests TEST_PREFIX "Frontend.")
endif()
