cmake_minimum_required(VERSION 2.8.7)

project(invent)

include(cmake/macros.cmake)
include(CheckCXXCompilerFlag)

## Set our own module path.
#
# If you define additional FindXXX.cmake files,
# put them in this directory.
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)


## Install support with CPack
#

set(CPACK_GENERATOR "WIX")
include(CPack)


## Global properties section.
#

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


## Global options section.
#
# Use cmake-gui or cmake (command-line) to set these options.
# E.g. cmake -DSTOP_ON_FIRST_ERROR=Off or
# -DCMAKE_BUID_TYPE=Debug to generate build files for a debug build.

option(TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors."                                      ON)
option(STOP_ON_FIRST_ERROR      "Stop compilation on first error."                               ON)
option(BUILD_EXAMPLES           "Build included example applications."                           ON)
option(BUILD_TESTS              "Build included unit tests."                                     ON)
option(STATIC_LIBRARIES         "Build libraries that are built by this project statically."     ON)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()


## Compiler options section.
#
# Based upon the options set by the user and the detected compiler
# the compilation flags are determined.

## Clang/GCC config section.

if (CMAKE_CXX_COMPILER MATCHES ".*clang")
  set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  if (MINGW)
    option(BUILD_64_BIT "Enable 64-bit build." OFF)
  else()
    option(BUILD_64_BIT "Enable 64-bit build." ON)
  endif()

  if (BUILD_64_BIT)
    set(PROJECT_COMMON_FLAGS "-m64" CACHE INTERNAL "Common flags." FORCE)
    set(PROJECT_SIZE_TYPE x64 CACHE INTERNAL "" FORCE)
  else()
    set(PROJECT_COMMON_FLAGS "-m32" CACHE INTERNAL "Common flags." FORCE)
    set(PROJECT_SIZE_TYPE x86 CACHE INTERNAL "" FORCE)
  endif()

  set(PROJECT_COMMON_FLAGS "${PROJECT_COMMON_FLAGS} -Wall -Wextra -Wno-missing-braces")

  if (TREAT_WARNINGS_AS_ERRORS)
    set(PROJECT_COMMON_FLAGS "${PROJECT_COMMON_FLAGS} -Werror")
  endif()

  if (STOP_ON_FIRST_ERROR)
    set(PROJECT_COMMON_FLAGS "${PROJECT_COMMON_FLAGS} -Wfatal-errors")
  endif()

  if (MINGW)
    set(PROJECT_COMMON_FLAGS "${PROJECT_COMMON_FLAGS} -static-libstdc++ -static-libgcc -static")
  endif()

  foreach (flag "-std=c++11" "-std=c++0x")
    CHECK_CXX_COMPILER_FLAG(${flag} CXX_COMPILER_SUPPORTS_NEW_ISO_STD)

    if (CXX_COMPILER_SUPPORTS_NEW_ISO_STD)
      set(CXX_NEW_ISO_STD_FLAG ${flag})
      break()
    endif()
  endforeach()

  if (NOT CXX_COMPILER_SUPPORTS_NEW_ISO_STD)
    message(FATAL_ERROR "The project requires C++11 features.\n"
                        "You need at least one of the following compilers:\n"
                        "- gcc 4.7 (see: http://gcc.gnu.org/projects/cxx0x.html).\n"
                        "- clang 3.1 (see: http://clang.llvm.org/cxx_status.html).")
  endif()

  if (CMAKE_COMPILER_IS_CLANGXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_NEW_ISO_STD_FLAG} -stdlib=libc++")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_NEW_ISO_STD_FLAG}")
    # Workaround for usage of thread::sleep (see: http://stackoverflow.com/questions/12523122/what-is-glibcxx-use-nanosleep-all-about).
    add_definitions("-D_GLIBCXX_USE_NANOSLEEP")
  endif()

  set(CMAKE_C_FLAGS             "${CMAKE_C_FLAGS}             ${PROJECT_COMMON_FLAGS}")
  set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS}           ${PROJECT_COMMON_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    ${PROJECT_COMMON_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${PROJECT_COMMON_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${PROJECT_COMMON_FLAGS}")
endif()


## MSVC config section.

if (MSVC OR MSVC_IDE)
  # See http://binglongx.wordpress.com/2013/06/28/set-non-default-platform-toolset-in-cmake/
  # for information on setting a non default platform toolset as is done below.

  if (MSVC_VERSION LESS 1700) # VC10-/VS2010-.
    message(FATAL_ERROR "The project requires C++11 features.\n"
                        "You need at least Visual Studio 11 (Microsoft Visual Studio 2012), "
                        "with Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012).")
  elseif (MSVC_VERSION EQUAL 1700) # VC11/VS2012.
    message("VC11: use Microsoft Visual Studio 2012 "
            "with Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012)")
    set(CMAKE_GENERATOR_TOOLSET "v120_CTP_Nov2012" CACHE STRING "Platform toolset." FORCE)
  else() # VC12+, assuming C++11 supported.
  endif()

  if (CMAKE_SIZEOF_VOID_P MATCHES 4)
    set(PROJECT_SIZE_TYPE x86 CACHE INTERNAL "" FORCE)
    set(BUILD_64_BIT FALSE CACHE INTERNAL "" FORCE)
  else()
    set(PROJECT_SIZE_TYPE x64 CACHE INTERNAL "" FORCE)
    set(BUILD_64_BIT TRUE CACHE INTERNAL "" FORCE)
  endif()

  add_definitions("-D_VARIADIC_MAX=10")
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")

  option(LINK_STATIC_RUNTIME "Link statically against C++ runtime." OFF)
  if (LINK_STATIC_RUNTIME)
    foreach (flag CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
        string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
    endforeach()

    set(CMAKE_C_FLAGS_DEBUG            "${CMAKE_C_FLAGS_DEBUG}"            CACHE STRING "MSVC C Debug MT flags."     FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}"          CACHE STRING "MSVC CXX Debug MT flags."   FORCE)
    set(CMAKE_C_FLAGS_RELEASE          "${CMAKE_C_FLAGS_RELEASE}"          CACHE STRING "MSVC C Release MT flags."   FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}"        CACHE STRING "MSVC CXX Release MT flags." FORCE)
    set(CMAKE_C_FLAGS_MINSIZEREL       "${CMAKE_C_FLAGS_MINSIZEREL}"       CACHE STRING "MSVC C Debug MT flags."     FORCE)
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL}"     CACHE STRING "MSVC C Release MT flags."   FORCE)
    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO}"   CACHE STRING "MSVC CXX Debug MT flags."   FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "MSVC CXX Release MT flags." FORCE)
  endif()
endif()


## Output directory section.

set(PROJECT_OUTPUT_SUBDIR ${PROJECT_SIZE_TYPE} CACHE INTERNAL "" FORCE)
set(PROJECT_BINARY_PATH  ${CMAKE_BINARY_DIR}/bin/${PROJECT_OUTPUT_SUBDIR} CACHE INTERNAL "" FORCE)
set(PROJECT_LIBRARY_PATH ${CMAKE_BINARY_DIR}/lib/${PROJECT_OUTPUT_SUBDIR} CACHE INTERNAL "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_PATH}
    CACHE PATH
    "Single Directory for all Executables." FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_PATH}
    CACHE PATH
    "Single Directory for all Libraries" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_LIBRARY_PATH}
    CACHE PATH
    "Single Directory for all static libraries." FORCE)


# Find a clean way to detect SSE support.
add_definitions(-DSSE_SUPPORT -DSSE3_SUPPORT)

set(EXTERNAL_DIR ${CMAKE_HOME_DIRECTORY}/external)

if (BUILD_TESTS)
  include(CTest)
  enable_testing()
endif()

if (BUILD_EXAMPLES)
  set(INCLUDE_EXAMPLE_DIR examples)
endif()

_SUBDIRS(
  ${EXTERNAL_DIR}        # Contains external dependencies that are built as part of this project.
  libraries              # Contains internal libraries.
  ${INCLUDE_EXAMPLE_DIR} # Contains example applications.
)
