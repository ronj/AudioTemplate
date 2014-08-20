include_directories("${EXTERNAL_DIR}/yaffut" ${LIBRARIES_INCLUDE_DIR})

file(GLOB TEST_SOURCES "test_*.cpp")

add_executable(test_dsp ${TEST_SOURCES} "${EXTERNAL_DIR}/yaffut/testrunner.cpp")
target_link_libraries(test_dsp ${DSP_LIBRARY})

invent_add_unit_tests("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_dsp" ${TEST_SOURCES})
