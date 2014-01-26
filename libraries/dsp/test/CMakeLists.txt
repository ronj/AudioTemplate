INCLUDE_DIRECTORIES("../../../external/yaffut" ${LIBRARIES_INCLUDE_DIR})

FILE(GLOB TEST_SOURCES "test_*.cpp")

ADD_EXECUTABLE(test_dsp ${TEST_SOURCES} "../../../external/yaffut/testrunner.cpp")
TARGET_LINK_LIBRARIES(test_dsp ${DSP_LIBRARY})

enable_testing()

_ADD_UNIT_TESTS(test_dsp ${TEST_SOURCES})
