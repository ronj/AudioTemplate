set(STORED_SOURCES)
set(STORED_INCLUDE_DIRECTORIES)

# Print a status message.
macro(invent_message)
  message(STATUS ${ARGV})
endmacro()

# Set the subdirectories to be parsed for more CMakelists.
macro(_SUBDIRS)
  foreach(it ${ARGV})
    invent_message("Entering subdirectory " ${it})
    add_subdirectory(${it})
  endforeach()
endmacro()

# Create a list of sources to be used.
macro(invent_sources)
  foreach(it ${ARGV})
    list(APPEND STORED_SOURCES ${it})
  endforeach()
endmacro()

# Create the include path list.
macro(invent_include_directories)
  foreach(it ${ARGV})
    list(APPEND STORED_INCLUDE_DIRECTORIES ${it})
  endforeach()

  include_directories(${STORED_INCLUDE_DIRECTORIES})
endmacro()

## Add unit tests.
#
# This macro requires two arguments.
# The first argument is the executable that needs to be called for running unit tests.
# This executable is expected to output zero in case of no failed test and non-zero in
# case one or more of the tests failed.
# The second argument is a list of source files that contain "TEST" or "TEST_F" macros.
#
# The source files are scanned for the macros and added to CTest with the following form:
# suitename::testname.
#
# Compatibility: Yaffut and Google Test (GTest).
# TODO: Proper support for FUNC test definition of Yaffut.
macro(invent_add_unit_tests _executable)
  foreach(TEST_SOURCE ${ARGN})
    file(READ "${TEST_SOURCE}" CONTENTS)
    string(REGEX MATCHALL "TEST_?F?\\(([A-Za-z_0-9 ,]+)\\)" FOUND_TESTS ${CONTENTS})

    foreach(FOUND_TEST ${FOUND_TESTS})
      string(REGEX REPLACE ".*\\(([A-Za-z_0-9]+)[, ]*([A-Za-z_0-9]+)\\).*" "\\1::\\2" TEST_NAME ${FOUND_TEST})

      add_test(${TEST_NAME} ${_executable} ${TEST_NAME})
    endforeach()
  endforeach()
endmacro()

# Only link the taget if necessary.
# _name The target name.
macro(invent_link_if_needed _name)
  if(WIN32 AND MSVC)
    set_target_properties(${_name} PROPERTIES LINK_FLAGS_RELEASE /OPT:REF)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    #set_target_properties(${_name} PROPERTIES LINK_FLAGS -Wl)
  elseif(__COMPILER_PATHSCALE)
    set_target_properties(${_name} PROPERTIES LINK_FLAGS -mp)
  else()
    set_target_properties(${_name} PROPERTIES LINK_FLAGS -Wl,--as-needed)
  endif()
endmacro()

# Add a library target.
# _name The library name.
# ARGN The source files for the library.
macro(invent_add_library _name)
  if (STATIC_LIBRARIES)
    add_library(${_name} STATIC ${ARGN})
  else()
    add_library(${_name} SHARED ${ARGN})
  endif()

  invent_link_if_needed(${_name})

  if(NOT STATIC_LIBRARIES)
    set_target_properties(${_name} PROPERTIES COMPILE_DEFINITIONS "INVENT_SOURCE")
  endif()
endmacro()
