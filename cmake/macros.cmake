SET(STORED_PROJECTNAME)
SET(STORED_SOURCES)
SET(STORED_INCLUDE_DIRECTORIES)
SET(STORED_CURRENT_MODULENAME)
SET(STORED_CURRENT_MODULEVERSION)
SET(STORED_CURRENT_TARGET_NAME)

# Set our own module path.
#
# If you define additional FindXXX.cmake files,
# put them in this directory.
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/modules)

# Print a status message.
MACRO(_MESSAGE)
  MESSAGE(STATUS ${ARGV})
ENDMACRO()

# Set the projectname and store it for future use.
MACRO(_PROJECT name)
  _MESSAGE("Configuring Project " ${name})

  PROJECT(${name})

  SET(STORED_PROJECTNAME ${name})
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
  SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
ENDMACRO()

# Set the modulename and store it for future use.
MACRO(_MODULE name version)
  SET(STORED_CURRENT_MODULENAME ${name})
  SET(STORED_CURRENT_MODULEVERSION ${version})

  _MESSAGE("Configuring Module " ${STORED_CURRENT_MODULENAME})
ENDMACRO()

# Set the subdirectories to be parsed for more CMakelists.
MACRO(_SUBDIRS)
  FOREACH(it ${ARGV})
    _MESSAGE("Entering subdirectory " ${it})
    ADD_SUBDIRECTORY(${it})
  ENDFOREACH()
ENDMACRO()

# Create a list of sources to be used.
MACRO(_SOURCES)
  FOREACH(it ${ARGV})
    SET(STORED_SOURCES ${STORED_SOURCES} ${it})
  ENDFOREACH()
ENDMACRO()

# Create the include path list.
MACRO(_INCLUDE_DIRECTORIES)
  FOREACH(it ${ARGV})
    SET(STORED_INCLUDE_DIRECTORIES ${STORED_INCLUDE_DIRECTORIES} ${it})
  ENDFOREACH()

  INCLUDE_DIRECTORIES(${STORED_INCLUDE_DIRECTORIES})
ENDMACRO()

# Add unit tests.
# This macro requires two arguments.
# The first argument is the executable that needs to be called for running unit tests.
# The second argument is a list of source files that contain "TEST" or "TEST_F" macros.
#
# The source files are scanned for the macros and added to CTest with the following form:
# suitename.testname.
#
# Compatibility: Yaffut and Google Test (GTest).
# TODO: Proper support for FUNC test definition of Yaffut.
MACRO(_ADD_UNIT_TESTS EXECUTABLE)
  FOREACH(TEST_SOURCE ${ARGN})
    FILE(READ "${TEST_SOURCE}" CONTENTS)
    STRING(REGEX MATCHALL "[TEST]_?F?\\(([A-Za-z_0-9 ,]+)\\)" FOUND_TESTS ${CONTENTS})
    FOREACH(FOUND_TEST ${FOUND_TESTS})
      STRING(REGEX REPLACE ".*\\(([A-Za-z_0-9]+)[, ]*([A-Za-z_0-9]+)\\).*" "\\1.\\2" TEST_NAME ${FOUND_TEST})
      ADD_TEST(${TEST_NAME} ${EXECUTABLE} ${TEST_NAME})
    ENDFOREACH()
  ENDFOREACH()
ENDMACRO()


# Only link the taget if necessary.
# _name The target name.
macro(LINK_IF_NEEDED _name)
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

###############################################################################
# Add a library target.
# _name The library name.
# ARGN The source files for the library.
macro(THREE_ADD_LIBRARY _name)
  if (STATIC_LIBRARIES)
    add_library(${_name} STATIC ${ARGN})
  else()
    add_library(${_name} SHARED ${ARGN})
  endif()

  set(DEP_LIBS_VAR ${_name}_DEP_LIBS)
  string(TOUPPER ${DEP_LIBS_VAR} DEP_LIBS_VAR)

  target_link_libraries(${_name} ${${DEP_LIBS_VAR}})

  set(DEPENDS_VAR ${_name}_DEPENDS)
  string(TOUPPER ${DEPENDS_VAR} DEPENDS_VAR)

  if (${${DEPENDS_VAR}})
    add_dependencies(${_name} ${${DEPENDS_VAR}})
  endif()

  link_if_needed(${_name})

  if(NOT OPT_STATIC_LIBRARIES)
    set_target_properties(${_name} PROPERTIES COMPILE_DEFINITIONS "THREE_SOURCE")
  endif()
endmacro()


# Create a library.
MACRO(_CREATE_LIBRARY name type)
  SET(STORED_CURRENT_TARGET_NAME ${name})
  IF(${STORED_CURRENT_MODULENAME})
    SET(subcomponentname ${STORED_CURRENT_MODULENAME}.${name})
  ELSE()
    SET(subcomponentname ${name})
  ENDIF()

  _MESSAGE("Creating library " ${subcomponentname})

  ADD_LIBRARY(${subcomponentname} ${type} ${STORED_SOURCES})

  SET_TARGET_PROPERTIES(${subcomponentname} PROPERTIES COMPONENT_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}"
                                                       COMPONENT_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

  INSTALL(TARGETS ${subcomponentname} DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
ENDMACRO()

# Create an executable.
MACRO(_CREATE_EXECUTABLE name)
  SET(STORED_CURRENT_TARGET_NAME ${name})
  IF(${STORED_CURRENT_MODULENAME})
    SET(subcomponentname ${STORED_CURRENT_MODULENAME}.${name})
  ELSE()
    SET(subcomponentname ${name})
  ENDIF()

  _MESSAGE("Creating executable " ${subcomponentname})

  ADD_EXECUTABLE(${subcomponentname} ${STORED_SOURCES})

  INSTALL(TARGETS ${subcomponentname} DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

  #GET_TARGET_PROPERTY(bindir ${subcomponentname} DEBUG_POSTFIX)
  #FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR} targetpath)
  #ADD_CUSTOM_COMMAND(TARGET ${subcomponentname} POST_BUILD COMMAND copy ${sourcepath} ${targetpath}\\debug VERBATIM )
ENDMACRO()

# Lookup a dependency.
#
# This macro tries to find dependencies by looking at:
# - The targets we are building (sequence is important here
#   a dependent project must be build after it's local dependencies).
#   Put projects, modules and subdirectories in the right sequence.
#   We try to give a warning if we suspect an error here.
# - System wide and local CMake Modules (FindXXX.cmake)
MACRO(_USES)
  IF(${STORED_CURRENT_MODULENAME})
    SET(subcomponentname ${STORED_CURRENT_MODULENAME}.${STORED_CURRENT_TARGET_NAME})
  ELSE()
    SET(subcomponentname ${STORED_CURRENT_TARGET_NAME})
  ENDIF()

  # Trying to find build components and extract data from them.
  FOREACH (it ${ARGV})
    # Strip version and name from the given module.
    STRING(REGEX REPLACE "@.*" "" component_name ${it})
    STRING(REGEX REPLACE ".*@" "" version_name ${it})

    # Check if the module is a target we are building.
    GET_TARGET_PROPERTY(component_binary_path ${component_name} COMPONENT_BINARY_PATH)
    STRING(REGEX REPLACE ".*NOTFOUND" "NOTFOUND" component_found ${component_binary_path})

    IF (${component_found} STREQUAL "NOTFOUND")
      SET(TARGET_FOUND 0)
    ELSE()
      # Handle the internal module.
      _MESSAGE("Adding ${component_name} to ${STORED_CURRENT_MODULENAME}")
      GET_TARGET_PROPERTY(component_binary_path ${component_name} COMPONENT_BINARY_PATH)
      GET_TARGET_PROPERTY(component_source_path ${component_name} COMPONENT_SOURCE_PATH)

      LINK_DIRECTORIES(${component_binary_path})

      TARGET_LINK_LIBRARIES(${subcomponentname} ${component_name})
      ADD_DEPENDENCIES(${subcomponentname} ${component_name})

      # TODO: Make a better mechanism to detect and add include directories.
      #       Subdirectories that are not a module itself are not taken into account.
      INCLUDE_DIRECTORIES(${component_source_path})
      SET(TARGET_FOUND 1)
    ENDIF()

    IF(NOT ${TARGET_FOUND})
      MESSAGE("Calling FIND_PACKAGE(${it})")

      # Do some preprocessing on the package name; this follows some assumptions:
      # - Underscores are not used in the name part of XXX_FOUND (e.g. SDL_ttf sets SDLTTF_FOUND).
      # - XXX_FOUND is in uppercase.
      STRING(REGEX REPLACE "_" "" package_name ${it})
      STRING(TOUPPER ${package_name} package_name)

      FIND_PACKAGE(${it})

      IF(${package_name}_FOUND)
        SET(PACKAGE_FOUND 1)
        SET(PACKAGE_LIBS ${${package_name}_LIBRARY})
        SET(PACKAGE_INCLUDES ${${package_name}_INCLUDE_DIR})

        TARGET_LINK_LIBRARIES(${subcomponentname} ${PACKAGE_LIBS})
        INCLUDE_DIRECTORIES(${PACKAGE_INCLUDES})

        _MESSAGE("Found package ${it} by looking at ${package_name}_FOUND")
        _MESSAGE("Linking to ${PACKAGE_LIBS}")
        _MESSAGE("Including ${PACKAGE_INCLUDES}")
      ELSE()
        # Non cab deliverable as target.
        IF (EXISTS ${it})
          TARGET_LINK_LIBRARIES(${subcomponentname} ${it})
        ELSE ()
          IF (NOT ${TARGET_FOUND})
            MESSAGE(FATAL_ERROR "Lib ${it} not found. If ${it} is internal make sure it is added before ${component_name}.")
          ELSE()
            MESSAGE(FATAL_ERROR "Lib ${it} not found. If ${it} is external make sure a Find${it}.cmake file exists.")
          ENDIF()
        ENDIF()
      ENDIF()
    ENDIF ()
  ENDFOREACH()
ENDMACRO()
