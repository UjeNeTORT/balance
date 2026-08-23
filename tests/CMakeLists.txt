cmake_minimum_required(VERSION 3.15)

project(balance-tests)

if (BUILD_END2END_TESTS)
    include(end2end.cmake)
endif()

if(BUILD_UNIT_TESTS)
    CPMAddPackage(
        NAME GTest
        GITHUB_REPOSITORY google/googletest
        VERSION 1.17.0
        OPTIONS
            "gtest_force_shared_crt ON" # Avoids MSVC runtime conflicts
    )
    include(GoogleTest)

    add_executable(FrontendTests
        FrontendTests.cpp
    )

    target_link_libraries(FrontendTests PRIVATE
        Frontend
        GTest::gtest GTest::gtest_main
    )

    target_include_directories(FrontendTests PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_compile_options(FrontendTests PRIVATE ${BALANCE_COMPILE_OPTIONS})

    target_compile_definitions(FrontendTests PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    )

    gtest_discover_tests(
        FrontendTests 
        TEST_PREFIX "Frontend."
        PROPERTIES LABELS unit LABELS frontend
    )

    add_executable(MachineLayerTests
        MachineLayerTests.cpp
    )

    target_link_libraries(MachineLayerTests PRIVATE
        MachineLayer MIRInfo MIRPasses Utils RISCV Analysis
        GTest::gtest GTest::gtest_main
    )

    target_include_directories(MachineLayerTests PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_compile_options(MachineLayerTests PRIVATE ${BALANCE_COMPILE_OPTIONS})

    gtest_discover_tests(
        MachineLayerTests
        TEST_PREFIX "MachineLayer."
        PROPERTIES LABELS unit LABELS machine
    )
endif()
