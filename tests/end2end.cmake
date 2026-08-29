
include(test_lists.cmake)

set(COMPILER ${CMAKE_BINARY_DIR}/bin/balancc)
set(BIN_DIR ${CMAKE_BINARY_DIR})
set(BUILD_DEPS ${COMPILER} runtime/sylib.c runtime/sylib.h)

foreach(test IN LISTS TEST_LIST)
    add_test(NAME test_${test}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}
            python3 end2end_test.py ${test} --compiler ${COMPILER} --build-dir ${BIN_DIR}
    )
    set_tests_properties(test_${test} PROPERTIES LABELS end2end)
endforeach()

foreach(test IN LISTS FAIL_TEST_LIST)
    add_test(NAME fail_test_${test}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}
            python3 end2end_test.py ${test} --compiler ${COMPILER} --build-dir ${BIN_DIR}
    )
    set_property(TEST fail_test_${test} PROPERTY WILL_FAIL TRUE)
    set_tests_properties(fail_test_${test} PROPERTIES LABELS end2end)
endforeach()

