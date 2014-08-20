include_directories("${EXTERNAL_DIR}/yaffut" ${LIBRARIES_INCLUDE_DIR} .)

file(GLOB TEST_SOURCES "unit/math/*_test.cpp")

add_executable(test_three ${TEST_SOURCES} "${EXTERNAL_DIR}/yaffut/testrunner.cpp" "test_constants.cpp")
target_link_libraries(test_three ${THREE_LIBRARY})

invent_add_unit_tests("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_three" ${TEST_SOURCES})
