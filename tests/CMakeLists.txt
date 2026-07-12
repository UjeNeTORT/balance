find_package(GTest REQUIRED)

add_executable(unit_tests
    FrontendTests.cpp
)

target_link_libraries(unit_tests PRIVATE
    frontend
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(unit_tests PRIVATE ${BALANCE_COMPILE_OPTIONS})

target_compile_definitions(unit_tests PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

add_test(NAME UnitTests COMMAND unit_tests)
