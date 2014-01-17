CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

INCLUDE(cmake/macros.cmake)
INCLUDE(CheckCXXCompilerFlag)

SET(BUILD_STATIC_LIBRARIES TRUE CACHE BOOL "Build libraries that are built by this project statically.")

IF(BUILD_STATIC_LIBRARIES)
  SET(LIBRARY_TYPE STATIC)
ELSE()
  SET(LIBRARY_TYPE SHARED)
ENDIF()

IF(WIN32)

ELSE()
  CHECK_CXX_COMPILER_FLAG("-std=c++11" CXX_COMPILER_SUPPORTS_NEW_ISO_STD)

  IF(${CXX_COMPILER_SUPPORTS_NEW_ISO_STD})
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -ansi -std=c++11")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -ansi -std=c++11")
  ELSE()
    MESSAGE(FATAL_ERROR "The selected compiler does not support C++11 features while this project requires it.")
  ENDIF()

  IF(APPLE)
    # Workaround for usage of thread::sleep (see: http://stackoverflow.com/questions/12523122/what-is-glibcxx-use-nanosleep-all-about).
    ADD_DEFINITIONS(-D_GLIBCXX_USE_NANOSLEEP)
  ENDIF()
ENDIF()

_PROJECT(AudioTemplate)

_SUBDIRS(
  external      # Contains external dependencies that are built as part of this project.
  libraries     # Contains internal libraries.
  applications
)
