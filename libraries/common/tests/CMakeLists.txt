include_directories("${EXTERNAL_DIR}/yaffut" ${LIBRARIES_INCLUDE_DIR})

file(GLOB TEST_SOURCES "test_*.cpp")

add_executable(test_common ${TEST_SOURCES} "${EXTERNAL_DIR}/yaffut/testrunner.cpp")

if (NOT WIN32)
  target_link_libraries(test_common pthread)
endif()

target_link_libraries(test_common ${COMMON_LIBRARY})

invent_add_unit_tests("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_common" ${TEST_SOURCES})
