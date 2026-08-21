if (BUILD_UNIT_TESTS)
    CPMAddPackage(
        NAME GTest
        GITHUB_REPOSITORY google/googletest
        VERSION 1.17.0
        OPTIONS
            "gtest_force_shared_crt ON" # Avoids MSVC runtime conflicts
    )
    include(GoogleTest)
endif()

