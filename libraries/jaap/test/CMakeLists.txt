INCLUDE_DIRECTORIES("../../../external/yaffut" ${JAAP_INCLUDE_DIRS})

FILE(GLOB TEST_SOURCES "test_*.cpp")

ADD_EXECUTABLE(test_jaap ${TEST_SOURCES} "../../../external/yaffut/testrunner.cpp")
TARGET_LINK_LIBRARIES(test_jaap ${JAAP_LIBRARY})

enable_testing()

_ADD_UNIT_TESTS(test_jaap ${TEST_SOURCES})